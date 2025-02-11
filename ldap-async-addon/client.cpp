#include <napi.h>
extern "C" {
    #include <ldap.h>
}

#include <vector>
#include <unordered_map>
#include <variant>

#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <functional>
#include <cassert>

#include "search_values.h"
#include "tls_validation_settings.h"

#include "utils.h"

#include "client.h"
#include "async-bind-ldap.h"
#include "async-ldap-search.h"
#include "async-ldap-close.h"
#include "assert.h"

LDAPURLDesc get_default_lud(){
    return {
        .lud_next       = nullptr,      // this should never be used. if you want to make a linked list, do it in js then run
        .lud_scheme     = nullptr,
        .lud_host       = nullptr,
        .lud_port       = -1,
        .lud_dn         = nullptr,
        .lud_attrs      = nullptr,
        .lud_scope      = -1,
        .lud_filter     = nullptr,
        .lud_exts       = nullptr,
        .lud_crit_exts  = -1
    };
}

Napi::Object LDAP_Client::Init(Napi::Env env, Napi::Object exports) {
    // This method is used to hook the accessor and method callbacks
    Napi::Function func = DefineClass(env, "LDAP", {
        InstanceMethod<&LDAP_Client::exec>("exec", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&LDAP_Client::bind>("bind", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&LDAP_Client::search>("search", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&LDAP_Client::close>("close", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    exports.Set("LDAP", func);

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}

LDAP_Client::LDAP_Client(const Napi::CallbackInfo& info) : Napi::ObjectWrap<LDAP_Client>(info)
{
    Napi::Env env = info.Env();

    Napi::Object config_params = info[0].As<Napi::Object>();

    std::string uri = config_params.Get("uri").ToString().Utf8Value();

    int status = ldap_initialize(&(this->client),uri.c_str());

    int ldap_version = LDAP_VERSION3;
    ldap_set_option(this->client, LDAP_OPT_PROTOCOL_VERSION, &ldap_version);

    if( config_params.Has("TLS_CHECK") ){
        TLS_CHECK ldap_opt_x_tls_require_cert = static_cast<TLS_CHECK>(extract_int(config_params,"TLS_CHECK"));
        int v = std::to_underlying(ldap_opt_x_tls_require_cert);
        ldap_set_option(this->client, LDAP_OPT_X_TLS_REQUIRE_CERT, &v );
    }
    //ldap_set_option(this->client, LDAP_OPT_X_TLS_REQUIRE_CERT, &tls_check);

    //ldap_set_option(this->client,LDAP_OPT_X_TLS_CACERTFILE, "/code/ldap-certs/myCA.crt");
    //ldap_set_option(this->client,LDAP_OPT_X_TLS_CERTFILE, "/code/ldap-certs/ldap.crt");
    //ldap_set_option(this->client,LDAP_OPT_X_TLS_KEYFILE, "/code/ldap-certs/ldap.key");
    
    printf("called client constructor\n");
}


Napi::Value LDAP_Client::bind(const Napi::CallbackInfo& info){
    Napi::Env env = info.Env();

    js_assert(env,"Bind input must be object",assert_type<Napi::Object>(info[0]));
    Napi::Object bind_params = info[0].As<Napi::Object>();

    //js_assert(env,"Connection must be closed",this->connection_status == LDAP_Client::connection_status::CLOSED );

    js_assert(env,"dn must be string",assert_object_key<Napi::String>(bind_params,"dn") );
    js_assert(env,"password must be string",assert_object_key<Napi::String>(bind_params,"password") );
    std::string dn = bind_params.Get("dn").ToString().Utf8Value();
    std::string pw = bind_params.Get("password").ToString().Utf8Value();

    /*
    struct berval* servercreds = NULL;
    struct berval cred;
    cred.bv_val = "adminpassword";
    cred.bv_len = strlen(cred.bv_val);
    int ldap_status = ldap_sasl_bind_s(this->client,"dn=admin,dc=example,dc=org",LDAP_SASL_SIMPLE,&cred, NULL, NULL,&servercreds);
    */

    //auto worker = new GenericAsyncWorker(env, bind_function);
    AsyncBindWorker* worker = new AsyncBindWorker(env,this->client,pw,dn,this->connection_status);
    worker->Queue();
    return worker->getPromise();
}

Napi::Value LDAP_Client::search(const Napi::CallbackInfo& info){
    Napi::Env env = info.Env();

    js_assert(env,"Search input must be object",assert_type<Napi::Object>(info[0]));
    Napi::Object search_params = info[0].As<Napi::Object>();

    js_assert(env, "scope must be a number", assert_object_key<Napi::Number>(search_params,"scope") );
    js_assert(env, "invalid scope value", [&](){
        uint32_t scope_int = extract_int(search_params,"scope");
        SEARCH_SCOPES s = static_cast<SEARCH_SCOPES>(scope_int);
        return is_valid_search_scope(s);
    }());
    SEARCH_SCOPES scope = static_cast<SEARCH_SCOPES>(extract_int(search_params,"scope"));

    //uint32_t scope = extract_int(search_params,"scope");

    js_assert(env, "filter must be a string", assert_object_key<Napi::String>(search_params,"filter") );
    std::string filter = extract_string(search_params,"filter");

    js_assert(env, "base must be a string", assert_object_key<Napi::String>(search_params,"base") );
    std::string base = extract_string(search_params,"base");

    js_assert(env, "attributes is array", assert_object_key<Napi::Array>(search_params,"attributes"));
    Napi::Array attribute_arr = search_params.Get("attributes").As<Napi::Array>();

    //std::string single_attribute_string = extract_string(attribute_arr,0);
    std::vector<std::string> attarr = std::vector<std::string>(attribute_arr.Length()+1);
    for(int i=0;i<attarr.size();i++)
        attarr[i] = extract_string(attribute_arr,i);

    {
        std::lock_guard<std::mutex> lock(this->make_request_mutex);
        js_assert(env,"Connection but be opened to make a search request",this->connection_status == LDAP_Client::status::OPEN );
        this->pending_requests++;
    }

    AsyncSearchWorker* worker = new AsyncSearchWorker(env, this->client, filter,base, scope, this->pending_requests, attarr);
    worker->Queue();
    return worker->getPromise();
}

Napi::Value LDAP_Client::close(const Napi::CallbackInfo& info){
    //printf("closed called\n");
    Napi::Env env = info.Env();

    {
        std::lock_guard<std::mutex> lock(this->make_request_mutex);
        this->connection_status = LDAP_Client::status::CLOSING;
    }

    AsyncCloseWorker* worker = new AsyncCloseWorker(env,this->client,this->pending_requests, this->connection_status);

    worker->Queue();
    return worker->getPromise();
}

LDAP_Client::~LDAP_Client() {
    //ldap_unbind_ext_s(this->client,NULL,NULL);
    printf("JAHSFKLJAHLKSFJHLKASJHFKL\n");
}

Napi::Value LDAP_Client::exec(const Napi::CallbackInfo& info){
    Napi::Env env = info.Env();
    
    int output_int = 0;

    int  scope = LDAP_SCOPE_SUBTREE;
    char* filter = "(|(uid=user01)(uid=user02))";
    char* attributes[] = {NULL};
    int  attributes_only = 0;
    LDAPMessage *answer;

    struct timeval timeout = {
        .tv_sec = 100,
        .tv_usec = 0
    };

/*
int ldap_search_ext_s(
              LDAP *ld,
              char *base,
              int scope,
              char *filter,
              char *attrs[],
              int attrsonly,
              LDAPControl **serverctrls,
              LDAPControl **clientctrls,
              struct timeval *timeout,
              int sizelimit,
              LDAPMessage **res );

*/
    struct berval* servercreds = NULL;
    struct berval cred;
    cred.bv_val = "adminpassword";
    cred.bv_len = strlen(cred.bv_val);


    int ldap_status = ldap_sasl_bind_s(this->client,"dn=admin,dc=example,dc=org",LDAP_SASL_SIMPLE,&cred, NULL, NULL,&servercreds);

    std::unordered_map<
        std::string,
        std::unordered_map<
            std::string,
            std::variant<
                std::string,
                std::vector<std::string>
            >
        >
    > return_map;
    /*
    int status3 = ldap_search_ext_s(this->client, "dc=example,dc=org", scope, filter,attributes, attributes_only, NULL, NULL, &timeout, -1, &answer);

    LDAPMessage* entry;
    

    for ( entry = ldap_first_entry(this->client, answer); entry != NULL; entry = ldap_next_entry(this->client, entry)) {

        // Print the DN string of the object 
        char* dn = ldap_get_dn(this->client, entry);

        printf("Found Object: %s\n", dn);
        BerElement *ber;

        std::unordered_map<
            std::string,
            std::variant<
                std::string,
                std::vector<std::string>
            >
        > dn_entry;

        dn_entry["dn"] = std::string(dn);

        for ( char* attribute = ldap_first_attribute(this->client, entry, &ber); attribute != NULL; attribute = ldap_next_attribute(this->client, entry, ber)){
            struct berval** ber_arr = ldap_get_values_len(this->client,entry, attribute);
            std::vector<std::string> attribute_values;

            for(int i=0;;i++){
                struct berval* ber_item = ber_arr[i];
                if(ber_item == NULL)
                    break;
                attribute_values.push_back(std::string(ber_item->bv_val));

                //printf("%s: %s\n",attribute,ber_item->bv_val);
            }

            dn_entry[attribute] = attribute_values;

            ldap_value_free_len(ber_arr);
            ldap_memfree(attribute);
        }

        return_map[dn] = dn_entry;
        ber_free(ber,0);
        ldap_memfree(dn);
    }
    ldap_msgfree(answer);
    ldap_unbind_ext(this->client,NULL,NULL);
    */

    ldap_unbind_ext(this->client,NULL,NULL);

    Napi::Object ldap_response_obj = Napi::Object::New(env);

    for(auto const [dn,data]: return_map){
        Napi::Object user_obj = Napi::Object::New(env);

        for(auto const [attribute, values]: data){
            
            if (attribute == "dn"){
                user_obj.Set("dn",std::get<std::string>(values));
                continue;
            }

            Napi::Array attribute_arr = Napi::Array::New(env);
            user_obj.Set(attribute, attribute_arr);
            for(int attribute_index = 0; std::string s: std::get<std::vector<std::string>>(values)){
                attribute_arr.Set(attribute_index,Napi::String::New(env,s));
                attribute_index++;
            }
        }

        ldap_response_obj.Set(dn,user_obj);
    }

    return ldap_response_obj;
}
