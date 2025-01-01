#include "napi.h"


/*
int ldap_sasl_bind(LDAP *ld, const char *dn, const char *mechanism,
       struct berval *cred, LDAPControl *sctrls[],
       LDAPControl *cctrls[], int *msgidp);
*/

LDAP_bind_worker::LDAP_bind_worker(const Napi::Env& env, LDAP* ld, struct berval *cred, int *msgidp): Napi::AsyncWorker(env),
    m_deferred(Napi::Deferred::New(env)),
    target_ldap_client{ldap}
{}


Napi::Promise::Deferred getPromise(){
    return m_deferred.Promise();
}

void LDAP_bind_worker::OnOK(){
    m_deferred.Resolve();
}

void LDAP_bind_worker::OnError(const Napi::Error& err){
    m_deferred.Reject(err.Value());
}

void LDAP_bind_worker::Execute(){
    int ldap_status = ldap_simple_bind_s(this->target_ldap_client)
    if (ldap_status != LDAP_SUCCESS){
        SetError(ldap_status);
        return;
    }
}