#pragma once

#include <napi.h>
#include <mutex>
#include <atomic>

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
        enum class connection_status {
            CLOSED,
            OPEN,
            CLOSING
        };
        LDAP_Client(const Napi::CallbackInfo& info);
        ~LDAP_Client();

    private:
        LDAPURLDesc client_settings;
        LDAP* client;
        Napi::Value exec(const Napi::CallbackInfo& info);
        Napi::Value bind(const Napi::CallbackInfo& info);
        Napi::Value search(const Napi::CallbackInfo& info);
        Napi::Value close(const Napi::CallbackInfo& info);

        std::mutex make_request_mutex;
        std::atomic<connection_status> connection_status = LDAP_Client::connection_status::CLOSED;
        std::atomic<int> pending_requests = 0;
};