#include <map>
#include <mdbx.h>
#include <mutex>
#include <napi.h>
#include <string>

class MDBX_Env : public Napi::ObjectWrap<MDBX_Env> {
  private:
	static std::map<std::string, std::shared_ptr<MDBX_env>> envMap;
	static std::mutex mutex;

	static std::shared_ptr<MDBX_env> createSharedEnv(const std::string &path, std::function<void(MDBX_env *)> configure);
	static void deleteSharedEnv(const std::string &path);

	std::string path;
	std::shared_ptr<MDBX_env> env;

	Napi::Value Info(const Napi::CallbackInfo &info);
	Napi::Value Stat(const Napi::CallbackInfo &info);
	Napi::Value GetDbi(const Napi::CallbackInfo &info);
	Napi::Value GetTxn(const Napi::CallbackInfo &info);
	Napi::Value GcInfo(const Napi::CallbackInfo &info);
	Napi::Value Readers(const Napi::CallbackInfo &info);

	Napi::Value GetOption(const Napi::CallbackInfo &info);
	void SetOption(const Napi::CallbackInfo &info);

	Napi::Value Sync(const Napi::CallbackInfo &info);
	void Copy(const Napi::CallbackInfo &info);
	void Close(const Napi::CallbackInfo &info);

  public:
	static void Init(Napi::Env env, Napi::Object exports);
	MDBX_Env(const Napi::CallbackInfo &info);
	~MDBX_Env();
};