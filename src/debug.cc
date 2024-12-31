#include "debug.h"

#include "utils.h"

const char* LogLevelToString(MDBX_log_level_t loglevel) {
    switch (loglevel) {
        case MDBX_LOG_FATAL: return "FATAL";
        case MDBX_LOG_ERROR: return "ERROR";
        case MDBX_LOG_WARN: return "WARN";
        case MDBX_LOG_NOTICE: return "NOTICE";
        case MDBX_LOG_VERBOSE: return "VERBOSE";
        case MDBX_LOG_DEBUG: return "DEBUG";
        case MDBX_LOG_TRACE: return "TRACE";
        case MDBX_LOG_EXTRA: return "EXTRA";
        case MDBX_LOG_DONTCHANGE: return "DONTCHANGE";
        default: return "UNKNOWN";
    }
}


void MDBX_Debug::Init(Napi::Env env, Napi::Object exports) {
	exports.Set("setup_debug", Napi::Function::New<MDBX_Debug::Setup>(env));
}

void MDBX_Debug::Setup(const Napi::CallbackInfo &info) {
	int flags = MDBX_DBG_NONE;
	MDBX_log_level_t log_level = MDBX_LOG_DONTCHANGE;
	MDBX_debug_func *logger = nullptr;

	Napi::Object options = info[0].ToObject();

	Utils::setFromObject(&flags, MDBX_DBG_DUMP, options, "dump");
	Utils::setFromObject(&flags, MDBX_DBG_LEGACY_MULTIOPEN, options, "legacyMultiopen");
	Utils::setFromObject(&flags, MDBX_DBG_LEGACY_OVERLAP, options, "legacyOverlap");
	Utils::setFromObject(&flags, MDBX_DBG_DONT_UPGRADE, options, "dontUpgrade");

	if (options.Get("logger").IsBoolean() && options.Get("logger").ToBoolean().Value()) {
        // Сохраняем JavaScript функцию в глобальной ссылке
        logger = &Logger;
		log_level = MDBX_LOG_EXTRA;
    }

	mdbx_setup_debug(log_level, static_cast<MDBX_debug_flags>(flags), &Logger);
}

void MDBX_Debug::Logger(MDBX_log_level_t loglevel, const char *function, int line, const char *fmt, va_list args) noexcept {
    char message[1024];
    vsnprintf(message, sizeof(message), fmt, args);
	printf("[%s] %s():%d %s", LogLevelToString(loglevel), function, line, message);
}