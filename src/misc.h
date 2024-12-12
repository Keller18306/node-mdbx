#include <mdbx.h>
#include <napi.h>

class MDBX_Misc : public Napi::ObjectWrap<MDBX_Misc> {
  private:
	Napi::Value Version(const Napi::CallbackInfo &info);

  public:
	static void Init(Napi::Env env, Napi::Object exports);
};