#include "client.h"
#include <napi.h>
extern "C" {
    #include <ldap.h>
}

#include <vector>
#include <unordered_map>
#include <variant>

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
        InstanceMethod<&LDAP_Client::bind>("bind", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
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

LDAP_Client::LDAP_Client(const Napi::CallbackInfo& info) : Napi::ObjectWrap<LDAP_Client>(info),
    client_settings{get_default_lud()}
{
    Napi::Env env = info.Env();

    Napi::Object config_params = info[0].As<Napi::Object>();

    std::string hostname = config_params.Get("host").ToString().Utf8Value();
    //size_t hostname_string_length = hostname.length();

    this->client_settings.lud_host = strdup(hostname.c_str());
    this->client_settings.lud_port = config_params.Get("port").As<Napi::Number>().Uint32Value();

    std::string uri = config_params.Get("uri").ToString().Utf8Value();

    int status = ldap_initialize(&(this->client),uri.c_str());

    printf("called constructor\n");
}

LDAP_Client::~LDAP_Client() {
    free(this->client_settings.lud_host);
    ldap_unbind_ext(this->client,NULL,NULL);
    printf("deconstructor called for ldap\n");
}

Napi::Value LDAP_Client::bind(const Napi::CallbackInfo& info){
    Napi::Env env = info.Env();
    printf("host = %s:%d\n",this->client_settings.lud_host,this->client_settings.lud_port);
    
    struct berval cred;
    cred.bv_val = "adminpassword";  // The password is used as the credential for binding
    cred.bv_len = strlen(cred.bv_val);  // The length of the password string

    int output_int = 0;
    int  ldap_version = LDAP_VERSION3;

    struct berval *servercreds = NULL;
    int status1 = ldap_set_option(this->client, LDAP_OPT_PROTOCOL_VERSION, &ldap_version);
    int status2 = ldap_sasl_bind_s(this->client,"cn=admin,dc=example,dc=org",LDAP_SASL_SIMPLE,&cred, NULL, NULL,&servercreds);

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
    int status3 = ldap_search_ext_s(this->client, "dc=example,dc=org", scope, filter,attributes, attributes_only, NULL, NULL, &timeout, -1, &answer);

    LDAPMessage* entry;
    
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
    {
        "dn": {
            "attribute": ["v1","v2"]
            "dn": "dn"
        }
    }
    */

    for ( entry = ldap_first_entry(this->client, answer); entry != NULL; entry = ldap_next_entry(this->client, entry)) {

        /* Print the DN string of the object */
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
            free(attribute);
        }

        return_map[dn] = dn_entry;
        ber_free(ber,0);
        ldap_memfree(dn);
    }
    ldap_msgfree(answer);

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
