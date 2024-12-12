#include <napi.h>
#include <vector>

#include "cursor.h"
#include "dbi.h"
#include "env.h"
#include "misc.h"
#include "txn.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
	MDBX_Env::Init(env, exports);
	MDBX_Txn::Init(env);
	MDBX_Dbi::Init(env);
	MDBX_Cursor::Init(env);
	MDBX_Misc::Init(env, exports);

	return exports;
}

NODE_API_MODULE(hello, Init)