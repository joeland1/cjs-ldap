#include <functional>
#include <napi.h>
#include <stacktrace>

#define js_assert(env,error_msg,f) if( (js_assert((env),(error_msg),(f))) == false) return (env).Undefined();

bool (js_assert)(Napi::Env env, std::string error_message, std::function<bool()> f){
    if( f() ){
        return true;
    }
    Napi::Error::New((env), (error_message)).ThrowAsJavaScriptException();
    return false;
}