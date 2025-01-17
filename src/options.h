#include <mdbx.h>
#include <napi.h>

class MDBX_Options : public Napi::ObjectWrap<MDBX_Options> {
  public:
	static void Init(Napi::Env env, Napi::Object exports);
};