#include <mdbx.h>
#include <napi.h>

class MDBX_Native_Version : public Napi::ObjectWrap<MDBX_Native_Version> {
  private:
	Napi::Value Version(const Napi::CallbackInfo &info);

  public:
	static void Init(Napi::Env env, Napi::Object exports);
};