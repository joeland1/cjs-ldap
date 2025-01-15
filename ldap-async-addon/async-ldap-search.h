#include <ldap.h>
#include <atomic>
#include <functional>

#include "search_values.h"

class AsyncSearchWorker : public Napi::AsyncWorker {
    public:
        AsyncSearchWorker(const Napi::Env& env, LDAP* ldap, std::string filter, std::string base, SEARCH_SCOPES scope, std::function<void()> func);
        ~AsyncSearchWorker();
        Napi::Promise getPromise();

    protected:
        void Execute();
        void OnOK();
        void OnError(const Napi::Error& err);

    private:
        LDAP* target_ldap_client;
        Napi::Promise::Deferred m_deferred;
        std::string filter;
        std::string base;
        SEARCH_SCOPES scope;
        std::function<void()> once_done;
};