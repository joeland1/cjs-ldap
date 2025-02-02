#include <napi.h>
#include "ldap-result.h"
/*
class JS_LDAP_Response : public Napi::ObjectWrap<JS_LDAP_Response> {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports){
        Napi::Function func = DefineClass(env, "LDAP Response", {
            InstanceMethod<&JS_LDAP_Response::GetValue>("GetValue", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        });
        Napi::FunctionReference* constructor = new Napi::FunctionReference();
        *constructor = Napi::Persistent(func);
        env.SetInstanceData<Napi::FunctionReference>(constructor);
    }
    JS_LDAP_Response(const Napi::CallbackInfo& info): Napi::ObjectWrap<JS_LDAP_Response>(info){
        _value = 2.0;
    }
    static Napi::Value create(Napi::Env env){
        Napi::FunctionReference* constructor = env.GetInstanceData<Napi::FunctionReference>();
        return constructor->New({ });
    }

  private:
    double _value;
    Napi::Value GetValue(const Napi::CallbackInfo& info){
        return Napi::Number::New(info.Env(),this->_value);
    }
};
*/

class JS_LDAP_Response : public Napi::ObjectWrap<JS_LDAP_Response> {
  public:
    JS_LDAP_Response(const Napi::CallbackInfo& info);    
    ~JS_LDAP_Response();
    inline static Napi::FunctionReference *New;
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Value create(Napi::Env env, std::shared_ptr<LDAP_Response> r);
  private:
    Napi::Value getDNs(const Napi::CallbackInfo& info);
    Napi::Value toObject(const Napi::CallbackInfo& info);
    std::shared_ptr<LDAP_Response> underlying;
};
