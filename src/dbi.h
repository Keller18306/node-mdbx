#include <mdbx.h>
#include <napi.h>

class MDBX_Dbi : public Napi::ObjectWrap<MDBX_Dbi> {
  private:
	MDBX_env *env = nullptr;
	MDBX_dbi dbi;
	unsigned int flags;

	Napi::Value Stat(const Napi::CallbackInfo &info);
	Napi::Value GetCursor(const Napi::CallbackInfo &info);
	Napi::Value MinKeySize(const Napi::CallbackInfo &info);
	Napi::Value MaxKeySize(const Napi::CallbackInfo &info);
	Napi::Value MinValSize(const Napi::CallbackInfo &info);
	Napi::Value MaxValSize(const Napi::CallbackInfo &info);
	void Rename(const Napi::CallbackInfo &info);
	void Drop(const Napi::CallbackInfo &info);
	void Close(const Napi::CallbackInfo &info);

  public:
	static void Init(Napi::Env env);
	MDBX_Dbi(const Napi::CallbackInfo &info);
	~MDBX_Dbi();
};