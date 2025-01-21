#include <napi.h>
#include <ldap.h>
#include <atomic>

#include <functional>

#include "generic-async.h"

/*
int ldap_sasl_bind(LDAP *ld, const char *dn, const char *mechanism,
       struct berval *cred, LDAPControl *sctrls[],
       LDAPControl *cctrls[], int *msgidp);
*/

GenericAsyncWorker::GenericAsyncWorker(const Napi::Env& env, std::function<void()> func): Napi::AsyncWorker(env),
    m_deferred(Napi::Promise::Deferred::New(env)),
    to_run{func}
{}

GenericAsyncWorker::~GenericAsyncWorker(){
}

Napi::Promise GenericAsyncWorker::getPromise(){
    return this->m_deferred.Promise();
}


void GenericAsyncWorker::OnOK(){
    this->m_deferred.Resolve(Napi::Number::New(this->Env(),LDAP_SUCCESS));
}

void GenericAsyncWorker::OnError(const Napi::Error& err){
    this->m_deferred.Reject(err.Value());
}

void GenericAsyncWorker::Execute(){
    this->to_run();
}