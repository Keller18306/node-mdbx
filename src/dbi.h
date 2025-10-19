#include <mdbx.h>
#include <napi.h>

class MDBX_Native_Dbi : public Napi::ObjectWrap<MDBX_Native_Dbi> {
  private:
	Napi::ObjectReference nativeTxnRef_;

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
	MDBX_env *env = nullptr;
	MDBX_txn *txn = nullptr;
	MDBX_dbi dbi;
	unsigned int flags;

	static void Init(Napi::Env env);
	MDBX_Native_Dbi(const Napi::CallbackInfo &info);
	~MDBX_Native_Dbi();
};