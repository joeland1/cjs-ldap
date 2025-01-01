#include "napi.h"

class AsyncLDAPBindWorker : public Napi::AsyncWorker {
 public:
  AsyncLDAPBindWorker(const Napi::Env& env, LDAP* ldap, )
  Napi::Promise GetPromise();

 protected:
  void Execute();
  void OnOK();
  void OnError(const Napi::Error& err);

 private:
  Napi::Promise::Deferred m_deferred;
};