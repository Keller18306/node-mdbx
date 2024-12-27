#include <mdbx.h>
#include <napi.h>

class MDBX_Txn : public Napi::ObjectWrap<MDBX_Txn> {
  private:

	Napi::Value Info(const Napi::CallbackInfo &info);
	Napi::Value CommitWithLatency(const Napi::CallbackInfo &info);
	void Commit(const Napi::CallbackInfo &info);
	void Abort(const Napi::CallbackInfo &info);
	void Park(const Napi::CallbackInfo &info);
	void Unpark(const Napi::CallbackInfo &info);
	void Reset(const Napi::CallbackInfo &info);
	void Renew(const Napi::CallbackInfo &info);

  public:
	MDBX_txn *txn = nullptr;

	static void Init(Napi::Env env);
	MDBX_Txn(const Napi::CallbackInfo &info);
	~MDBX_Txn();
};