#include <napi.h>

extern "C" {
    #include <ldap.h>
}

#include <atomic>

class AsyncCloseWorker : public Napi::AsyncWorker {
    public:
        AsyncCloseWorker(const Napi::Env& env, LDAP* ldap,std::atomic<bool>& ok_to_close);
        ~AsyncCloseWorker();
        Napi::Promise getPromise();

    protected:
        void Execute();
        void OnOK();
        void OnError(const Napi::Error& err);

    private:
        LDAP* target_ldap_client;
        Napi::Promise::Deferred m_deferred;
        std::atomic<bool>& ok_to_close;
};