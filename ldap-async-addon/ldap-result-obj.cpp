#include <napi.h>
#include <ranges>
#include "ldap-result-obj.h"
#include "ldap-result.h"

JS_LDAP_Response::JS_LDAP_Response(const Napi::CallbackInfo& info): Napi::ObjectWrap<JS_LDAP_Response>(info){
    printf("created ldap js response\n");
}

JS_LDAP_Response::~JS_LDAP_Response(){
    printf("deleted js resonse\n");
}

Napi::Value JS_LDAP_Response::toObject(const Napi::CallbackInfo& info){
    Napi::Env env = info.Env();
    ldap_response_map r = this->underlying.get()->response;

    Napi::Object js_response_obj = Napi::Object::New(env);

    for(auto const [dn,data]: r){
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

        js_response_obj.Set(dn,user_obj);
    }
    return js_response_obj;
}

Napi::Value JS_LDAP_Response::getDNs(const Napi::CallbackInfo& info){
    //std::vector<long> keys(*(this->underlying.size());
    Napi::Env env = info.Env();
    ldap_response_map r = this->underlying.get()->response;
    Napi::Object set = Napi::Object::New(env);

    Napi::Value setConstructor = env.Global().Get("Set");
    Napi::Object result = setConstructor.As<Napi::Function>().New({});
    Napi::Function add_to_set = result.Get("add").As<Napi::Function>();
    
    for (auto const& key : std::views::keys(r))
        add_to_set.Call(result,{Napi::String::New(env,key)});

    return result;
}

Napi::Object JS_LDAP_Response::Init(Napi::Env env, Napi::Object exports){
    Napi::Function func = DefineClass(env, "LDAP Response", {
        InstanceMethod<&JS_LDAP_Response::getDNs>("getDNs", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&JS_LDAP_Response::toObject>("toObject", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });
    /*
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData<Napi::FunctionReference>(constructor);
    */
    JS_LDAP_Response::New = new Napi::FunctionReference();
    *(JS_LDAP_Response::New) = Napi::Persistent(func);

    return exports;
}

Napi::Value JS_LDAP_Response::create(Napi::Env env, std::shared_ptr<LDAP_Response> r){
    Napi::Object v = JS_LDAP_Response::New->New({ });
    JS_LDAP_Response* x = Napi::ObjectWrap<JS_LDAP_Response>::Unwrap(v);
    x->underlying = std::move(r);
    return v;
}