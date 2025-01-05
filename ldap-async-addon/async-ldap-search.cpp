#include <napi.h>

extern "C" {
    #include <ldap.h>
}

#include "async-ldap-search.h"

AsyncSearchWorker::AsyncSearchWorker(const Napi::Env& env, LDAP* ld, std::string filter, std::string base, SEARCH_SCOPES scope): Napi::AsyncWorker(env),
    m_deferred(Napi::Promise::Deferred::New(env)),
    target_ldap_client{ld},
    filter{filter},
    base{base},
    scope{scope}
{}

AsyncSearchWorker::~AsyncSearchWorker(){
}

Napi::Promise AsyncSearchWorker::getPromise(){
    return this->m_deferred.Promise();
}


void AsyncSearchWorker::OnOK(){
    struct timeval timeout = {
        .tv_sec = 100,
        .tv_usec = 0
    };

    char* attributes[] = {NULL};
    LDAPMessage *answer;
    int attributes_only = 0;

    printf("base = %s\n",this->base.c_str());

    int ldap_search_status = ldap_search_ext_s(this->target_ldap_client, this->base.c_str(), std::to_underlying(this->scope), this->filter.c_str(),attributes, attributes_only, NULL, NULL, &timeout, -1, &answer);

    Napi::Object o = Napi::Object::New(this->Env());
    o.Set("value",Napi::Number::New(this->Env(),ldap_search_status));
    this->m_deferred.Resolve(o);
}

void AsyncSearchWorker::OnError(const Napi::Error& err){
    this->m_deferred.Reject(err.Value());
}

void AsyncSearchWorker::Execute(){
    printf("some work goes here\n");
}