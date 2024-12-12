#include <mdbx.h>
#include <napi.h>

class MDBX_Env : public Napi::ObjectWrap<MDBX_Env> {
  private:
	MDBX_env *env = nullptr;

	// Napi::Value Test(const Napi::CallbackInfo &info);
	Napi::Value Info(const Napi::CallbackInfo &info);
	Napi::Value Stat(const Napi::CallbackInfo &info);
	Napi::Value GetDbi(const Napi::CallbackInfo &info);
	Napi::Value GetTxn(const Napi::CallbackInfo &info);
	void Close(const Napi::CallbackInfo &info);

  public:
	static void Init(Napi::Env env, Napi::Object exports);
	MDBX_Env(const Napi::CallbackInfo &info);
	~MDBX_Env();
};