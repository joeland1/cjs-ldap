#include <napi.h>
#include <ldap.h>
#include <atomic>

#include "client.h"
#include "async-ldap-close.h"
/*
int ldap_sasl_bind(LDAP *ld, const char *dn, const char *mechanism,
       struct berval *cred, LDAPControl *sctrls[],
       LDAPControl *cctrls[], int *msgidp);
*/

AsyncCloseWorker::AsyncCloseWorker(const Napi::Env& env, LDAP*& ld, std::atomic<int>& requests_left, std::atomic<LDAP_Client::status>& ldap_client_status ): Napi::AsyncWorker(env),
    m_deferred(Napi::Promise::Deferred::New(env)),
    target_ldap_client{ld},
    requests_left{requests_left},
    ldap_client_status{ldap_client_status}
{}

AsyncCloseWorker::~AsyncCloseWorker(){
    printf("async close deconstructor called\n");
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
    while(this->requests_left != 0){
        this->requests_left.wait(this->requests_left);
    }

    int ldap_status = ldap_unbind_ext_s(this->target_ldap_client,NULL,NULL);
    printf("ldap unbind status: %s\n",ldap_err2string(ldap_status));

    if (ldap_status != LDAP_SUCCESS){
        printf("ldap close error\n");
        this->SetError("ldap error");
        return;
    }
}