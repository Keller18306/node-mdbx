#include "errors.h"

void MDBX_Errors::Init(Napi::Env env, Napi::Object exports) {
	Napi::Object code = Napi::Object::New(env);

	code.Set("MDBX_SUCCESS", static_cast<int>(MDBX_SUCCESS));
	code.Set("MDBX_RESULT_FALSE", static_cast<int>(MDBX_RESULT_FALSE));
	code.Set("MDBX_RESULT_TRUE", static_cast<int>(MDBX_RESULT_TRUE));
	code.Set("MDBX_KEYEXIST", static_cast<int>(MDBX_KEYEXIST));
	code.Set("MDBX_FIRST_LMDB_ERRCODE", static_cast<int>(MDBX_FIRST_LMDB_ERRCODE));
	code.Set("MDBX_NOTFOUND", static_cast<int>(MDBX_NOTFOUND));
	code.Set("MDBX_PAGE_NOTFOUND", static_cast<int>(MDBX_PAGE_NOTFOUND));
	code.Set("MDBX_CORRUPTED", static_cast<int>(MDBX_CORRUPTED));
	code.Set("MDBX_PANIC", static_cast<int>(MDBX_PANIC));
	code.Set("MDBX_VERSION_MISMATCH", static_cast<int>(MDBX_VERSION_MISMATCH));
	code.Set("MDBX_INVALID", static_cast<int>(MDBX_INVALID));
	code.Set("MDBX_MAP_FULL", static_cast<int>(MDBX_MAP_FULL));
	code.Set("MDBX_DBS_FULL", static_cast<int>(MDBX_DBS_FULL));
	code.Set("MDBX_READERS_FULL", static_cast<int>(MDBX_READERS_FULL));
	code.Set("MDBX_TXN_FULL", static_cast<int>(MDBX_TXN_FULL));
	code.Set("MDBX_CURSOR_FULL", static_cast<int>(MDBX_CURSOR_FULL));
	code.Set("MDBX_PAGE_FULL", static_cast<int>(MDBX_PAGE_FULL));
	code.Set("MDBX_UNABLE_EXTEND_MAPSIZE", static_cast<int>(MDBX_UNABLE_EXTEND_MAPSIZE));
	code.Set("MDBX_INCOMPATIBLE", static_cast<int>(MDBX_INCOMPATIBLE));
	code.Set("MDBX_BAD_RSLOT", static_cast<int>(MDBX_BAD_RSLOT));
	code.Set("MDBX_BAD_TXN", static_cast<int>(MDBX_BAD_TXN));
	code.Set("MDBX_BAD_VALSIZE", static_cast<int>(MDBX_BAD_VALSIZE));
	code.Set("MDBX_BAD_DBI", static_cast<int>(MDBX_BAD_DBI));
	code.Set("MDBX_PROBLEM", static_cast<int>(MDBX_PROBLEM));
	code.Set("MDBX_LAST_LMDB_ERRCODE", static_cast<int>(MDBX_LAST_LMDB_ERRCODE));
	code.Set("MDBX_BUSY", static_cast<int>(MDBX_BUSY));
	code.Set("MDBX_FIRST_ADDED_ERRCODE", static_cast<int>(MDBX_FIRST_ADDED_ERRCODE));
	code.Set("MDBX_EMULTIVAL", static_cast<int>(MDBX_EMULTIVAL));
	code.Set("MDBX_EBADSIGN", static_cast<int>(MDBX_EBADSIGN));
	code.Set("MDBX_WANNA_RECOVERY", static_cast<int>(MDBX_WANNA_RECOVERY));
	code.Set("MDBX_EKEYMISMATCH", static_cast<int>(MDBX_EKEYMISMATCH));
	code.Set("MDBX_TOO_LARGE", static_cast<int>(MDBX_TOO_LARGE));
	code.Set("MDBX_THREAD_MISMATCH", static_cast<int>(MDBX_THREAD_MISMATCH));
	code.Set("MDBX_TXN_OVERLAPPING", static_cast<int>(MDBX_TXN_OVERLAPPING));
	code.Set("MDBX_BACKLOG_DEPLETED", static_cast<int>(MDBX_BACKLOG_DEPLETED));
	code.Set("MDBX_DUPLICATED_CLK", static_cast<int>(MDBX_DUPLICATED_CLK));
	code.Set("MDBX_DANGLING_DBI", static_cast<int>(MDBX_DANGLING_DBI));
	code.Set("MDBX_OUSTED", static_cast<int>(MDBX_OUSTED));
	code.Set("MDBX_MVCC_RETARDED", static_cast<int>(MDBX_MVCC_RETARDED));
	code.Set("MDBX_LAST_ADDED_ERRCODE", static_cast<int>(MDBX_LAST_ADDED_ERRCODE));
	code.Set("MDBX_ENODATA", static_cast<int>(MDBX_ENODATA));
	code.Set("MDBX_EINVAL", static_cast<int>(MDBX_EINVAL));
	code.Set("MDBX_EACCESS", static_cast<int>(MDBX_EACCESS));
	code.Set("MDBX_ENOMEM", static_cast<int>(MDBX_ENOMEM));
	code.Set("MDBX_EROFS", static_cast<int>(MDBX_EROFS));
	code.Set("MDBX_ENOSYS", static_cast<int>(MDBX_ENOSYS));
	code.Set("MDBX_EIO", static_cast<int>(MDBX_EIO));
	code.Set("MDBX_EPERM", static_cast<int>(MDBX_EPERM));
	code.Set("MDBX_EINTR", static_cast<int>(MDBX_EINTR));
	code.Set("MDBX_ENOFILE", static_cast<int>(MDBX_ENOFILE));
	code.Set("MDBX_EREMOTE", static_cast<int>(MDBX_EREMOTE));
	code.Set("MDBX_EDEADLK", static_cast<int>(MDBX_EDEADLK));

	exports.Set("ErrorCode", code);
}