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
#include "utils.h"

#include "client.h"
#include "async-bind-ldap.h"
#include "async-ldap-search.h"
#include "async-ldap-close.h"
#include "assert.h"
#include "generic-async.h"

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

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    // Create a persistent reference to the class constructor. This will allow
    // a function called on a class prototype and a function
    // called on instance of a class to be distinguished from each other.
    *constructor = Napi::Persistent(func);
    exports.Set("LDAP", func);

    // Store the constructor as the add-on instance data. This will allow this
    // add-on to support multiple instances of itself running on multiple worker
    // threads, as well as multiple instances of itself running in different
    // contexts on the same thread.
    //
    // By default, the value set on the environment here will be destroyed when
    // the add-on is unloaded using the `delete` operator, but it is also
    // possible to supply a custom deleter.
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
    int tls_check = LDAP_OPT_X_TLS_ALLOW;
    ldap_set_option(this->client, LDAP_OPT_PROTOCOL_VERSION, &ldap_version);
    ldap_set_option(this->client, LDAP_OPT_X_TLS_REQUIRE_CERT, &tls_check);
    //ldap_set_option(this->client,LDAP_OPT_X_TLS_CACERTFILE, "/code/ldap-certs/myCA.crt");
    //ldap_set_option(this->client,LDAP_OPT_X_TLS_CERTFILE, "/code/ldap-certs/ldap.crt");
    //ldap_set_option(this->client,LDAP_OPT_X_TLS_KEYFILE, "/code/ldap-certs/ldap.key");
    
    printf("called constructor\n");
}


Napi::Value LDAP_Client::bind(const Napi::CallbackInfo& info){
    Napi::Env env = info.Env();

    js_assert(env,"Bind input must be object",assert_type<Napi::Object>(info[0]));
    Napi::Object bind_params = info[0].As<Napi::Object>();

    js_assert(env,"Connection must be closed",this->connection_status == LDAP_Client::connection_status::CLOSED );

    js_assert(env,"dn must be string",assert_object_key<Napi::String>(bind_params,"dn") );
    js_assert(env,"password must be string",assert_object_key<Napi::String>(bind_params,"password") );
    std::string dn = bind_params.Get("dn").ToString().Utf8Value();
    std::string pw = bind_params.Get("password").ToString().Utf8Value();
    printf("pw = %s\n",pw.c_str());

    
    std::function<void()> mark_connection_as_open = [&](){
        this->connection_status = LDAP_Client::connection_status::OPEN;
    };
    //ldap_sasl_bind_s(this->client,dn.c_str(),LDAP_SASL_SIMPLE,&cred, NULL, NULL,&servercreds);
    AsyncBindWorker* worker = new AsyncBindWorker(env, this->client,pw, dn, mark_connection_as_open);
    
    /*
    std::function<void()> bind_function = [&,dn,pw](){
        printf("lambda\n");
        struct berval* servercreds = NULL;
        
        struct berval cred;

        //make a copy so underlying isnt modified
        char pw_char_array[pw.length() + 1];
        strcpy(pw_char_array,pw.c_str());
        cred.bv_val = pw_char_array;

        cred.bv_len = pw.length();

        ldap_sasl_bind_s(this->client,dn.c_str(),LDAP_SASL_SIMPLE,&cred, NULL, NULL,&servercreds);
        this->connection_status = LDAP_Client::connection_status::OPEN;
    };

    GenericAsyncWorker* worker = new GenericAsyncWorker(env,bind_function);
    */
    

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

    {
        std::lock_guard<std::mutex> lock(this->make_request_mutex);
        js_assert(env,"Connection but be opened to make a search request",this->connection_status == LDAP_Client::connection_status::OPEN );
        this->pending_requests++;
    }

    std::function<void()> decrement_on_finish = [&](){
        this->pending_requests--;
        this->pending_requests.notify_one();
    };

    AsyncSearchWorker* worker = new AsyncSearchWorker(env, this->client, filter,base, scope,decrement_on_finish);
    worker->Queue();
    return worker->getPromise();
}

Napi::Value LDAP_Client::close(const Napi::CallbackInfo& info){
    printf("closed called\n");
    Napi::Env env = info.Env();

    {
        std::lock_guard<std::mutex> lock(this->make_request_mutex);
        this->connection_status = LDAP_Client::connection_status::CLOSING;
    }

    std::function<void()> set_ldap_client_status = [&](){
        this->connection_status = LDAP_Client::connection_status::CLOSED;
    };

    std::function<void()> mark_close_ldap = [&](){
        this->connection_status = LDAP_Client::connection_status::CLOSED;
    };

    std::function<void()> close_ldap = [&](){
        ldap_unbind_ext_s(this->client,NULL,NULL);
    };

    AsyncCloseWorker* worker = new AsyncCloseWorker(env,this->client,this->pending_requests, set_ldap_client_status);
    //GenericAsyncWorker* worker = new GenericAsyncWorker(env,close_ldap);
    worker->Queue();
    return worker->getPromise();
}

LDAP_Client::~LDAP_Client() {
    //ldap_unbind_ext(this->client,NULL,NULL);
    //printf("deconstructor called for ldap\n");
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
