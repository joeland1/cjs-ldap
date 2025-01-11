#include <napi.h>
#include <ldap.h>

#include "client.h"
#include "search_values.h"

// https://linux.die.net/man/3/ldap_url_parse
// https://linux.die.net/man/3/ldap_init

/*
Napi::Value create_client(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (!info[0].IsObject()){
        Napi::TypeError::New(env, "input is LDAP_client dict TODO: make better errors").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    Napi::Object config_params = info[0].As<Napi::Object>();

    //char* ldap_host = config_params.Get("host").ToString().Utf8Value().data();
    LDAPURLDesc lud = get_default_lud();

    lud.lud_host = config_params.Get("host").ToString().Utf8Value().data();
    lud.lud_port = config_params.Get("port").As<Napi::Number>().Uint32Value();

    LDAP_client* c = new LDAP_client(env, lud);

    Napi::Object obj = Napi::Object::New(env);
    obj.Set(Napi::String::New(env, "fn"), c->bind);

    return obj;
}

*/

Napi::Object Init (Napi::Env env, Napi::Object exports) {
    LDAP_Client::Init(env, exports);
    CREATE_SEARCH_SCOPE_ENUM(env, exports);
    return exports;
}

NODE_API_MODULE(addon, Init);