#include <napi.h>
#include <atomic>

#include "client.h"

extern "C" {
    #include <ldap.h>
}

class AsyncBindWorker : public Napi::AsyncWorker {
    public:
        AsyncBindWorker(const Napi::Env& env, LDAP* ldap, struct berval* cred, std::string dn, std::function<void()> onOK);
        ~AsyncBindWorker();
        Napi::Promise getPromise();

    protected:
        void Execute();
        void OnOK();
        void OnError(const Napi::Error& err);

    private:
        LDAP* target_ldap_client;
        Napi::Promise::Deferred m_deferred;
        std::string dn;
        int ldap_status_code;
        struct berval* my_creds;
        std::function<void()> on_success_callback;
};