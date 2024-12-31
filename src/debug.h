#include <mdbx.h>
#include <napi.h>

class MDBX_Debug : public Napi::ObjectWrap<MDBX_Debug> {
    private:
	  static void Setup(const Napi::CallbackInfo &info);
	  static void Logger(MDBX_log_level_t loglevel, const char *function, int line, const char *fmt, va_list args) noexcept;

	public:
	  static void Init(Napi::Env env, Napi::Object exports);
};