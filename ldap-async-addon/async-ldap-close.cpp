#include <napi.h>
#include <ldap.h>
#include <atomic>

#include "async-ldap-close.h"
/*
int ldap_sasl_bind(LDAP *ld, const char *dn, const char *mechanism,
       struct berval *cred, LDAPControl *sctrls[],
       LDAPControl *cctrls[], int *msgidp);
*/

AsyncCloseWorker::AsyncCloseWorker(const Napi::Env& env, LDAP*& ld, std::atomic<int>& requests_left, std::function<void()> on_close): Napi::AsyncWorker(env),
    m_deferred(Napi::Promise::Deferred::New(env)),
    target_ldap_client{ld},
    requests_left{requests_left},
    on_close{on_close}
{}

AsyncCloseWorker::~AsyncCloseWorker(){
}

Napi::Promise AsyncCloseWorker::getPromise(){
    return this->m_deferred.Promise();
}


void AsyncCloseWorker::OnOK(){
    this->on_close();
    this->m_deferred.Resolve(Napi::Number::New(this->Env(),LDAP_SUCCESS));
}

void AsyncCloseWorker::OnError(const Napi::Error& err){
    this->m_deferred.Reject(err.Value());
}

void AsyncCloseWorker::Execute(){
    while(this->requests_left != 0){
        this->requests_left.wait(this->requests_left);
    }

    int ldap_status = ldap_unbind_ext(this->target_ldap_client,NULL,NULL);
    printf("ldap status: %s\n",ldap_err2string(ldap_status));

    if (ldap_status != LDAP_SUCCESS){
        printf("ldap close error\n");
        this->SetError("ldap error");
        return;
    }
}