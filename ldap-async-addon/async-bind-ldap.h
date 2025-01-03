#include <napi.h>

extern "C" {
    #include <ldap.h>
}

class AsyncBindWorker : public Napi::AsyncWorker {
    public:
        AsyncBindWorker(const Napi::Env& env, LDAP* ldap, struct berval* cred, std::string dn);
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
};