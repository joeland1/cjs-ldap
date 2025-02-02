#include "tls_validation_settings.h"

static Napi::PropertyDescriptor create_read_only_int(Napi::Env env, std::string key, TLS_CHECK value){
    return Napi::PropertyDescriptor::Value(
        key, 
        Napi::Number::New(env,static_cast<int>(value)), 
        static_cast<napi_property_attributes>(napi_enumerable)
    );
}

Napi::Object CREATE_TLS_CHECK_ENUM(Napi::Env env){
    Napi::Object scopes = Napi::Object::New(env);

    scopes.DefineProperties({
        create_read_only_int(env,"never",TLS_CHECK::NEVER),
        create_read_only_int(env,"hard",TLS_CHECK::HARD),
        create_read_only_int(env,"demand",TLS_CHECK::DEMAND),
        create_read_only_int(env,"allow",TLS_CHECK::ALLOW),
        create_read_only_int(env,"try",TLS_CHECK::TRY)
    });

    return scopes;
}

bool is_valid_tls_scope(TLS_CHECK s){
    switch(s){
        case TLS_CHECK::NEVER:
        case TLS_CHECK::HARD:
        case TLS_CHECK::DEMAND:
        case TLS_CHECK::ALLOW:
        case TLS_CHECK::TRY:
            return true;
        default: return false;

    }
}