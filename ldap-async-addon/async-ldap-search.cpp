#include <napi.h>

extern "C" {
    #include <ldap.h>
}

#include "async-ldap-search.h"
#include <atomic>
#include <functional>
#include <thread>
#include "ldap-result.h"

AsyncSearchWorker::AsyncSearchWorker(const Napi::Env& env, LDAP* ld, std::string filter, std::string base, SEARCH_SCOPES scope, std::function<void()> func): Napi::AsyncWorker(env),
    m_deferred(Napi::Promise::Deferred::New(env)),
    target_ldap_client{ld},
    filter{filter},
    base{base},
    scope{scope},
    once_done{func}
{}

AsyncSearchWorker::~AsyncSearchWorker(){
}

Napi::Promise AsyncSearchWorker::getPromise(){
    return this->m_deferred.Promise();
}


void AsyncSearchWorker::OnOK(){
    this->m_deferred.Resolve(Napi::Number::New(Env(),3));
}

void AsyncSearchWorker::OnError(const Napi::Error& err){
    this->m_deferred.Reject(err.Value());
}

void AsyncSearchWorker::Execute(){
    struct timeval timeout = {
        .tv_sec = 100,
        .tv_usec = 0
    };

    LDAPMessage* answer;
    char* attributes[] = {NULL};
    int attributes_only = 0;

    int ldap_search_status = ldap_search_ext_s(this->target_ldap_client, this->base.c_str(), std::to_underlying(this->scope), this->filter.c_str(),attributes, attributes_only, NULL, NULL, &timeout, -1, &answer);
    LDAP_Response* x = new LDAP_Response(this->target_ldap_client,answer);
    delete x;
    //LDAP_Response::free_response(this->target_ldap_client,answer);
    
    this->once_done();
}