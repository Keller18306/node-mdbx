#include "debug.h"

#include "utils.h"

void MDBX_Debug::Init(Napi::Env env, Napi::Object exports) {
	exports.Set("setup_debug", Napi::Function::New<MDBX_Debug::Setup>(env));
}

void MDBX_Debug::Setup(const Napi::CallbackInfo &info) {
	int flags = MDBX_DBG_NONE;

	Napi::Object options = info[0].ToObject();

	Utils::setFromObject(&flags, MDBX_DBG_DUMP, options, "dump");
	Utils::setFromObject(&flags, MDBX_DBG_LEGACY_MULTIOPEN, options, "legacyMultiopen");
	Utils::setFromObject(&flags, MDBX_DBG_LEGACY_OVERLAP, options, "legacyOverlap");
	Utils::setFromObject(&flags, MDBX_DBG_DONT_UPGRADE, options, "dontUpgrade");

	mdbx_setup_debug(MDBX_log_level_t::MDBX_LOG_DONTCHANGE, static_cast<MDBX_debug_flags>(flags), nullptr);
}