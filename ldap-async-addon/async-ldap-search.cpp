#include <napi.h>

extern "C" {
    #include <ldap.h>
}

#include "async-ldap-search.h"
#include <atomic>
#include <functional>
#include <thread>
#include "ldap-result.h"
#include "ldap-result-obj.h"

AsyncSearchWorker::AsyncSearchWorker(const Napi::Env& env, LDAP* ld, std::string filter, std::string base, SEARCH_SCOPES scope, std::atomic<int>& pending_connections, std::vector<std::string> attributes): Napi::AsyncWorker(env),
    m_deferred(Napi::Promise::Deferred::New(env)),
    target_ldap_client{ld},
    filter{filter},
    base{base},
    scope{scope},
    pending_connections{pending_connections},
    attributes{attributes}
{}

AsyncSearchWorker::~AsyncSearchWorker(){
}

Napi::Promise AsyncSearchWorker::getPromise(){
    return this->m_deferred.Promise();
}


void AsyncSearchWorker::OnOK(){
    this->m_deferred.Resolve(
        JS_LDAP_Response::create(this->Env(),this->result)
    );
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
    int attributes_only = 0;

    //char** attributes_2d_arr = (char**)(void*)(this->attributes.get());
    char* attributes_2d_arr[this->attributes.size() + 1];
    attributes_2d_arr[this->attributes.size()] = NULL;

    if(this->attributes.size() == 0 )
        attributes_2d_arr[0] = "dn";
    else
        for(size_t i=0;i<this->attributes.size();i++)
            attributes_2d_arr[i] = this->attributes[i].data();


    int ldap_search_status = ldap_search_ext_s(this->target_ldap_client, this->base.c_str(), std::to_underlying(this->scope), this->filter.c_str(),attributes_2d_arr, attributes_only, NULL, NULL, &timeout, -1, &answer);
    this->pending_connections--;
    this->pending_connections.notify_one();
    std::shared_ptr<LDAP_Response> better_ldap = std::make_shared<LDAP_Response>(this->target_ldap_client,answer);
    this->result = better_ldap;
    //delete better_ldap;
    ldap_msgfree(answer);
    //LDAP_Response::free_response(this->target_ldap_client,answer);
    
}