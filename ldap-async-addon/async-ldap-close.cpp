#include <napi.h>
#include <ldap.h>

#include "async-ldap-close.h"
/*
int ldap_sasl_bind(LDAP *ld, const char *dn, const char *mechanism,
       struct berval *cred, LDAPControl *sctrls[],
       LDAPControl *cctrls[], int *msgidp);
*/

AsyncCloseWorker::AsyncCloseWorker(const Napi::Env& env, LDAP* ld, std::atomic<bool>& ok_to_close): Napi::AsyncWorker(env),
    m_deferred(Napi::Promise::Deferred::New(env)),
    target_ldap_client{ld},
    ok_to_close{ok_to_close}
{}

AsyncCloseWorker::~AsyncCloseWorker(){
}

Napi::Promise AsyncCloseWorker::getPromise(){
    return this->m_deferred.Promise();
}


void AsyncCloseWorker::OnOK(){
    this->m_deferred.Resolve(Napi::Number::New(this->Env(),LDAP_SUCCESS));
}

void AsyncCloseWorker::OnError(const Napi::Error& err){
    this->m_deferred.Reject(err.Value());
}

void AsyncCloseWorker::Execute(){
    while(!this->ok_to_close)
        this->ok_to_close.wait(false);

    int ldap_status = ldap_unbind_ext(this->target_ldap_client,NULL,NULL);

    if (ldap_status != LDAP_SUCCESS){
        printf("ldap close error\n");
        this->SetError("ldap error");
        return;
    }
    printf("ldap close ok\n");
}