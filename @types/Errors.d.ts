import '../index';

declare module '../index' {
    enum ErrorCode {
        MDBX_SUCCESS,
        MDBX_RESULT_FALSE,
        MDBX_RESULT_TRUE,
        MDBX_KEYEXIST,
        MDBX_FIRST_LMDB_ERRCODE,
        MDBX_NOTFOUND,
        MDBX_PAGE_NOTFOUND,
        MDBX_CORRUPTED,
        MDBX_PANIC,
        MDBX_VERSION_MISMATCH,
        MDBX_INVALID,
        MDBX_MAP_FULL,
        MDBX_DBS_FULL,
        MDBX_READERS_FULL,
        MDBX_TXN_FULL,
        MDBX_CURSOR_FULL,
        MDBX_PAGE_FULL,
        MDBX_UNABLE_EXTEND_MAPSIZE,
        MDBX_INCOMPATIBLE,
        MDBX_BAD_RSLOT,
        MDBX_BAD_TXN,
        MDBX_BAD_VALSIZE,
        MDBX_BAD_DBI,
        MDBX_PROBLEM,
        MDBX_LAST_LMDB_ERRCODE,
        MDBX_BUSY,
        MDBX_FIRST_ADDED_ERRCODE,
        MDBX_EMULTIVAL,
        MDBX_EBADSIGN,
        MDBX_WANNA_RECOVERY,
        MDBX_EKEYMISMATCH,
        MDBX_TOO_LARGE,
        MDBX_THREAD_MISMATCH,
        MDBX_TXN_OVERLAPPING,
        MDBX_BACKLOG_DEPLETED,
        MDBX_DUPLICATED_CLK,
        MDBX_DANGLING_DBI,
        MDBX_OUSTED,
        MDBX_MVCC_RETARDED,
        MDBX_LAST_ADDED_ERRCODE,
        MDBX_ENODATA,
        MDBX_EINVAL,
        MDBX_EACCESS,
        MDBX_ENOMEM,
        MDBX_EROFS,
        MDBX_ENOSYS,
        MDBX_EIO,
        MDBX_EPERM,
        MDBX_EINTR,
        MDBX_ENOFILE,
        MDBX_EREMOTE,
        MDBX_EDEADLK
    }
}