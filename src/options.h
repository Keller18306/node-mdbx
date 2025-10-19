#include <mdbx.h>
#include <napi.h>

class MDBX_Native_Options : public Napi::ObjectWrap<MDBX_Native_Options> {
  public:
	static void Init(Napi::Env env, Napi::Object exports);
};