#pragma once

#include <functional>
#include <napi.h>
#include <typeinfo>
#include <any>
#include <type_traits>

static 


#define js_assert(env,info,f) { if( !js_assert_impl((env),(info),(f))) return (env).Undefined(); }

bool js_assert_impl(Napi::Env env, std::string info, std::function<bool()> f){
    if( f() ){
        return true;
    }
    Napi::Error::New((env), (info)).ThrowAsJavaScriptException();
    return false;
}

bool js_assert_impl(Napi::Env env, std::string info, bool b){
    if( b ){
        return true;
    }
    Napi::Error::New((env), (info)).ThrowAsJavaScriptException();
    return false;
}

/*
#define assert_type(env,info,v,T) if( (assert_type_impl<T>((env),(info),(v))) == false) return (env).Undefined();

#define CREATE_FUNCTION_NAPI_TYPE_ASSERT(napi_type,f)                    \
    template <>                                              \
    bool assert_type_impl<napi_type>(Napi::Env env, std::string info, std::any v){               \
        Napi::Value* x = std::any_cast<Napi::Value>(&v);               \
        if(x == nullptr) {  \
            Napi::Error::New((env), (info)).ThrowAsJavaScriptException();                \
            return false;            \
        };                       \
        if( x->f() ) return true;       \
        Napi::Error::New((env), (info)).ThrowAsJavaScriptException(); \
        return false;                                      \
    }
*/


template <typename T, typename R>
static R run_decleration(T& v, R (T::*member)() const ) {
    return (v.*member)();}
/*
#define CREATE_NAPI_ASSERT_TYPE(type,ref)       \
    template <>                                     \
    bool assert_type<type>(std::any v){ \
        if (!assert_type<Napi::Value>(v)) return false; \
        Napi::Value* x = std::any_cast<Napi::Value>(&v);    \
        bool run_value = run_decleration<Napi::Value,bool>(*x,(ref)); \
        return run_value;                       \
    }
*/
/*
--- Sample typecheck for napi values below ----

template <>
bool assert_type<Napi::Number>(std::any v){
    if (!assert_type<Napi::Value>(v)) return false;
    Napi::Value* x = std::any_cast<Napi::Value>(&v);
    bool run_value = run_decleration<Napi::Value,bool>(*x,&Napi::Value::IsNumber);
    return run_value;
}

--------
*/

// exact type check
template < typename T > 
bool is_class(std::any v){
    T* x = std::any_cast<T>(&v);
    return x != nullptr;
};

// F is a subclass of T
template < typename T , typename F >
bool assert_type(F v){
    return std::is_nothrow_convertible<F,T>::value;
};

// this is used if you run a member function to check type
template< typename T , typename F >
bool assert_type(F v, bool (F::*f)() const ){
    return run_decleration<F,bool>(v,f);
}

template < typename T, typename K >
bool assert_object_key(Napi::Value v,K k){
    if ( !assert_type<Napi::Object>(v) ) return false;
    Napi::Object o = v.As<Napi::Object>();

    if ( !o.Has(k) ) return false;

    Napi::Value maybe_valid = o.Get(k);
    return assert_type<T>(maybe_valid);
}

#define CREATE_NAPI_ASSERT_TYPE(napi_type,napi_function)                    \
    template<>                                                              \
    bool assert_type<napi_type>(Napi::Value v){                             \
        return assert_type<napi_type,Napi::Value>(v,(napi_function));       \
    }


/*
---- Example of function above ----

template<>
bool assert_type<Napi::Number>(Napi::Value v){
    return assert_type<napi_type,Napi::Value>(v,&Napi::Value::IsNumber);
}

-----------------------------------
*/

//CREATE_NAPI_ASSERT_TYPE(Napi::Number,&Napi::Value::IsNumber);
CREATE_NAPI_ASSERT_TYPE(Napi::Array,                   &Napi::Value::IsArray);
CREATE_NAPI_ASSERT_TYPE(Napi::ArrayBuffer,             &Napi::Value::IsArrayBuffer);
CREATE_NAPI_ASSERT_TYPE(Napi::BigInt,                  &Napi::Value::IsBigInt);
CREATE_NAPI_ASSERT_TYPE(Napi::Boolean,                 &Napi::Value::IsBoolean);
//CREATE_NAPI_ASSERT_TYPE(Napi::Buffer<std::any>,        &Napi::Value::IsBuffer); //todo
CREATE_NAPI_ASSERT_TYPE(Napi::DataView,                &Napi::Value::IsDataView);
CREATE_NAPI_ASSERT_TYPE(Napi::Date,                    &Napi::Value::IsDate);
//CREATE_NAPI_ASSERT_TYPE(Napi::External<std::any>,      &Napi::Value::IsExternal); //todo
CREATE_NAPI_ASSERT_TYPE(Napi::String,                  &Napi::Value::IsString);
CREATE_NAPI_ASSERT_TYPE(Napi::Function,                &Napi::Value::IsFunction);
CREATE_NAPI_ASSERT_TYPE(Napi::Number,                  &Napi::Value::IsNumber);
CREATE_NAPI_ASSERT_TYPE(Napi::Object,                  &Napi::Value::IsObject);
CREATE_NAPI_ASSERT_TYPE(Napi::Promise,                 &Napi::Value::IsPromise);
CREATE_NAPI_ASSERT_TYPE(Napi::Symbol,                  &Napi::Value::IsSymbol);
CREATE_NAPI_ASSERT_TYPE(Napi::TypedArray,              &Napi::Value::IsTypedArray);

//template<>
//bool assert_type<Napi::Number,Napi::Value>(Napi::Value,&Napi::Value::IsNumber);

/*
template<>
bool assert_type<Napi::Number>(std::any v){
    if( !assert_type<Napi::Value>(v) ) return false;
    printf("is napi value\n");
    return false;
}
*/

// CREATE_NAPI_ASSERT_TYPE(Napi::Object, &Napi::Value::IsObject);
/*
template<>
bool assert_type<Napi::String>(std::any v){
    if (!assert_type<Napi::Value>(v)) return false;
    Napi::Value x = std::any_cast<Napi::Value>(v);
    return run_decleration<Napi::Value, bool>(x,&Napi::Value::IsString);
}
*/


