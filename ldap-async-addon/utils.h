#include <napi.h>

template < typename T >
std::string extract_string(Napi::Object o, T k){
    return o.Get(k).ToString().Utf8Value();
}

// must go from value -> number... idk
template < typename T >
uint32_t extract_int(Napi::Object o, T k){
    Napi::Value v = o.Get(k);
    Napi::Number n = v.As<Napi::Number>();
    return n.Uint32Value();
}