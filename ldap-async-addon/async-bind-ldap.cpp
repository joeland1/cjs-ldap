#include <napi.h>
#include <ldap.h>
#include <atomic>

#include "async-bind-ldap.h"
/*
int ldap_sasl_bind(LDAP *ld, const char *dn, const char *mechanism,
       struct berval *cred, LDAPControl *sctrls[],
       LDAPControl *cctrls[], int *msgidp);
*/

AsyncBindWorker::AsyncBindWorker(const Napi::Env& env, LDAP* ld, struct berval* cred, std::string dn, std::function<void()> onOK): Napi::AsyncWorker(env),
    m_deferred(Napi::Promise::Deferred::New(env)),
    target_ldap_client{ld},
    my_creds{ber_dupbv(NULL, cred)},
    dn{dn},
    on_success_callback{onOK}
{}

AsyncBindWorker::~AsyncBindWorker(){
    ber_bvfree(this->my_creds);
}

Napi::Promise AsyncBindWorker::getPromise(){
    return this->m_deferred.Promise();
}


void AsyncBindWorker::OnOK(){
    this->on_success_callback();
    this->m_deferred.Resolve(Napi::Number::New(this->Env(),LDAP_SUCCESS));
}

void AsyncBindWorker::OnError(const Napi::Error& err){
    this->m_deferred.Reject(err.Value());
}

void AsyncBindWorker::Execute(){

    struct berval* servercreds = NULL;
    int ldap_status = ldap_sasl_bind_s(this->target_ldap_client,this->dn.c_str(),LDAP_SASL_SIMPLE,this->my_creds, NULL, NULL,&servercreds);

    if (ldap_status != LDAP_SUCCESS){
        printf("ldap error\n");
        this->SetError("ldap error");
        return;
    }
}