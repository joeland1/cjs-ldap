#include <napi.h>
#include <atomic>
#include <functional>

extern "C" {
    #include <ldap.h>
}

#include <atomic>

class AsyncCloseWorker : public Napi::AsyncWorker {
    public:
        AsyncCloseWorker(const Napi::Env& env, LDAP*& ldap,std::atomic<int>& requests_left, std::atomic<LDAP_Client::status>& ldap_client_status);
        ~AsyncCloseWorker();
        Napi::Promise getPromise();

    protected:
        void Execute();
        void OnOK();
        void OnError(const Napi::Error& err);

    private:
        LDAP*& target_ldap_client;
        Napi::Promise::Deferred m_deferred;
        std::atomic<int>& requests_left;
        std::atomic<LDAP_Client::status>& ldap_client_status;
};