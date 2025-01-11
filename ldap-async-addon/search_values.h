#include <ldap.h>
#include <napi.h>

enum class SEARCH_SCOPES { 
    Base = LDAP_SCOPE_BASE,
    One_Level = LDAP_SCOPE_ONELEVEL,
    Subtree = LDAP_SCOPE_SUBTREE,
    Children = LDAP_SCOPE_CHILDREN
};

static Napi::PropertyDescriptor create_read_only_int(Napi::Env env, std::string key, SEARCH_SCOPES value){
    return Napi::PropertyDescriptor::Value(
        key, 
        Napi::Number::New(env,static_cast<int>(value)), 
        static_cast<napi_property_attributes>(napi_enumerable)
    );
}

static Napi::Object CREATE_SEARCH_SCOPE_ENUM(Napi::Env env, Napi::Object exports){
    Napi::Object scopes = Napi::Object::New(env);

    scopes.DefineProperties({
        create_read_only_int(env,"base",SEARCH_SCOPES::Base),
        create_read_only_int(env,"one_level",SEARCH_SCOPES::One_Level),
        create_read_only_int(env,"subtree",SEARCH_SCOPES::Subtree),
        create_read_only_int(env,"children",SEARCH_SCOPES::Children)
    });

    exports.Set("SEARCH_SCOPES", scopes);
}