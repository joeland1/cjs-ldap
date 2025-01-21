#include <napi.h>
#include <atomic>
#include <functional>

extern "C" {
    #include <ldap.h>
}

#include <atomic>

class GenericAsyncWorker : public Napi::AsyncWorker {
    public:
        GenericAsyncWorker(const Napi::Env& env, std::function<void()> func);
        ~GenericAsyncWorker();
        Napi::Promise getPromise();

    protected:
        void Execute();
        void OnOK();
        void OnError(const Napi::Error& err);

    private:
        Napi::Promise::Deferred m_deferred;
        std::function<void()> to_run;
};