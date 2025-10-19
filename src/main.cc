#include <napi.h>
#include <vector>

#include "cursor.h"
#include "dbi.h"
#include "debug.h"
#include "env.h"
#include "errors.h"
#include "options.h"
#include "txn.h"
#include "version.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
	MDBX_Native_Env::Init(env, exports);
	MDBX_Native_Txn::Init(env);
	MDBX_Native_Dbi::Init(env);
	MDBX_Native_Cursor::Init(env);
	MDBX_Native_Version::Init(env, exports);
	MDBX_Native_Errors::Init(env, exports);
	MDBX_Native_Options::Init(env, exports);
	MDBX_Native_Debug::Init(env, exports);

	return exports;
}

NODE_API_MODULE(hello, Init)