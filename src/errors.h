#include <mdbx.h>
#include <napi.h>

class MDBX_Errors : public Napi::ObjectWrap<MDBX_Errors> {
  public:
	static void Init(Napi::Env env, Napi::Object exports);
};