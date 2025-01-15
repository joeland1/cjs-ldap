#include "search_values.h"

static Napi::PropertyDescriptor create_read_only_int(Napi::Env env, std::string key, SEARCH_SCOPES value){
    return Napi::PropertyDescriptor::Value(
        key, 
        Napi::Number::New(env,static_cast<int>(value)), 
        static_cast<napi_property_attributes>(napi_enumerable)
    );
}

Napi::Object CREATE_SEARCH_SCOPE_ENUM(Napi::Env env){
    Napi::Object scopes = Napi::Object::New(env);

    scopes.DefineProperties({
        create_read_only_int(env,"base",SEARCH_SCOPES::Base),
        create_read_only_int(env,"one_level",SEARCH_SCOPES::One_Level),
        create_read_only_int(env,"subtree",SEARCH_SCOPES::Subtree),
        create_read_only_int(env,"children",SEARCH_SCOPES::Children)
    });

    return scopes;
}

bool is_valid_search_scope(SEARCH_SCOPES s){
    switch(s){
        case SEARCH_SCOPES::Base:
        case SEARCH_SCOPES::One_Level:
        case SEARCH_SCOPES::Subtree:
        case SEARCH_SCOPES::Children:
            return true;
        default: return false;

    }
}