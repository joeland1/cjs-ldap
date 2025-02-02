#include <napi.h>
#include <ldap.h>
#include <atomic>

#include "async-bind-ldap.h"
#include "client.h"
/*
int ldap_sasl_bind(LDAP *ld, const char *dn, const char *mechanism,
       struct berval *cred, LDAPControl *sctrls[],
       LDAPControl *cctrls[], int *msgidp);
*/

AsyncBindWorker::AsyncBindWorker(const Napi::Env& env, LDAP*& ld, std::string cred, std::string dn, std::atomic<LDAP_Client::status>& connection_status): Napi::AsyncWorker(env),
    m_deferred(Napi::Promise::Deferred::New(env)),
    target_ldap_client{ld},
    dn{dn},
    my_creds{cred},
    conn{connection_status}
{}

AsyncBindWorker::~AsyncBindWorker(){
}

Napi::Promise AsyncBindWorker::getPromise(){
    return this->m_deferred.Promise();
}


void AsyncBindWorker::OnOK(){
    this->m_deferred.Resolve(Napi::Number::New(this->Env(),LDAP_SUCCESS));
}

void AsyncBindWorker::OnError(const Napi::Error& err){
    this->m_deferred.Reject(err.Value());
}

void AsyncBindWorker::Execute(){
    struct berval* servercreds = NULL;
    struct berval creds;
    creds.bv_val = this->my_creds.data();
    creds.bv_len = strlen(creds.bv_val);

    int ldap_status = ldap_sasl_bind_s(this->target_ldap_client,this->dn.c_str(),LDAP_SASL_SIMPLE,&creds, NULL, NULL,&servercreds);

    if (ldap_status != LDAP_SUCCESS){
        printf("ldap error bind %s\n",ldap_err2string(ldap_status));
        this->SetError("ldap error");
        return;
    }
    this->conn = LDAP_Client::status::OPEN;
}