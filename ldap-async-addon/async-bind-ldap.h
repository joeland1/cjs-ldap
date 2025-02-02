#include <napi.h>
#include <atomic>

#include "client.h"

extern "C" {
    #include <ldap.h>
}

class AsyncBindWorker : public Napi::AsyncWorker {
    public:
        AsyncBindWorker(const Napi::Env& env, LDAP*& ldap, std::string cred, std::string dn, std::atomic<LDAP_Client::status>& connection_status);
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
        std::string my_creds;
        std::atomic<LDAP_Client::status>& conn;
};