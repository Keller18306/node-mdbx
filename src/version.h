#include <mdbx.h>
#include <napi.h>

class MDBX_Version : public Napi::ObjectWrap<MDBX_Version> {
  private:
	Napi::Value Version(const Napi::CallbackInfo &info);

  public:
	static void Init(Napi::Env env, Napi::Object exports);
};