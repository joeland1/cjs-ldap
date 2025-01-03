#include <napi.h>
#include <ldap.h>

#include "async-bind-ldap.h"
/*
int ldap_sasl_bind(LDAP *ld, const char *dn, const char *mechanism,
       struct berval *cred, LDAPControl *sctrls[],
       LDAPControl *cctrls[], int *msgidp);
*/

AsyncBindWorker::AsyncBindWorker(const Napi::Env& env, LDAP* ld, struct berval* cred, std::string dn): Napi::AsyncWorker(env),
    m_deferred(Napi::Promise::Deferred::New(env)),
    target_ldap_client{ld},
    my_creds{ber_dupbv(NULL, cred)},
    dn{dn}
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
    printf("start bind async worker\n");

    struct berval* servercreds = NULL;
    printf("pw = %s\n",this->my_creds->bv_val);
    printf("dn = %s\n",this->dn.c_str());
    int ldap_status = ldap_sasl_bind_s(this->target_ldap_client,this->dn.c_str(),LDAP_SASL_SIMPLE,this->my_creds, NULL, NULL,&servercreds);

    printf("end bind async worker\n");
    printf("error %s\n",ldap_err2string(ldap_status));
    if (ldap_status != LDAP_SUCCESS){
        printf("ldap error\n");
        this->SetError("ldap error");
        return;
    }
}