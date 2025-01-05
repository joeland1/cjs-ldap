#include <napi.h>

extern "C" {
    #include <ldap.h>
}


/*
class LDAP_client : public Napi::AsyncWorker, public Napi::ObjectWrap<LDAP_client> {
    public:
        LDAP_client(const Napi::Env &env, LDAPURLDesc c);
        ~LDAP_client();
        static Napi::Object Init(const Napi::Env &env, Napi::Object exports);
        static Napi::Value CreateNewItem(const Napi::CallbackInfo& info);
        Napi::Promise GetPromise();
        std::string bind();

    protected:
        void Execute();
        void OnOK();
        void OnError(const Napi::Error& err);

    private:
        const Napi::Promise::Deferred m_deferred;
};
*/
class LDAP_Client : public Napi::ObjectWrap<LDAP_Client> {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    LDAP_Client(const Napi::CallbackInfo& info);
    ~LDAP_Client();

  private:
    LDAPURLDesc client_settings;
    LDAP* client;
    Napi::Value exec(const Napi::CallbackInfo& info);
    Napi::Value bind(const Napi::CallbackInfo& info);
    Napi::Value search(const Napi::CallbackInfo& info);
    const std::string dn;
};