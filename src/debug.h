#include <mdbx.h>
#include <napi.h>

class MDBX_Debug : public Napi::ObjectWrap<MDBX_Debug> {
    private:
	  static void Setup(const Napi::CallbackInfo &info);

	public:
	  static void Init(Napi::Env env, Napi::Object exports);
};