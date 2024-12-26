#include "env.h"

#include <filesystem>

#include "dbi.h"
#include "txn.h"
#include "utils.h"

std::mutex MDBX_Env::mutex;
std::map<std::string, std::shared_ptr<MDBX_env>> MDBX_Env::envMap;

std::shared_ptr<MDBX_env> MDBX_Env::createSharedEnv(const std::string &path, std::function<void(MDBX_env *)> configure) {
	std::lock_guard<std::mutex> lock(MDBX_Env::mutex);
	auto &map = MDBX_Env::envMap;

	if (map.find(path) != map.end()) {
		return map[path];
	}

	MDBX_env *rawEnv = nullptr;
	int rc = mdbx_env_create(&rawEnv);
	if (rc != MDBX_SUCCESS) {
		throw std::runtime_error("Failed to create MDBX_env");
	}

	if (configure) {
		configure(rawEnv);
	}

	std::shared_ptr<MDBX_env> sharedEnv(rawEnv, [](MDBX_env *env) {
		if (env) {
			// mdbx_env_close(env);
		}
	});

	map[path] = sharedEnv;
	return sharedEnv;
}

void MDBX_Env::deleteSharedEnv(const std::string &path) {
	std::lock_guard<std::mutex> lock(MDBX_Env::mutex);
	auto &map = MDBX_Env::envMap;
	auto it = map.find(path);
	if (it != map.end()) {
		map.erase(it);
	}
}

void MDBX_Env::Init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(env, "MDBXEnv",
		{
			InstanceMethod("info", &MDBX_Env::Info),
			InstanceMethod("stat", &MDBX_Env::Stat),
			InstanceMethod("getDbi", &MDBX_Env::GetDbi),
			InstanceMethod("getTxn", &MDBX_Env::GetTxn),
			InstanceMethod("gcInfo", &MDBX_Env::GcInfo),
			InstanceMethod("readers", &MDBX_Env::Readers),
			InstanceMethod("sync", &MDBX_Env::Sync),
			InstanceMethod("copy", &MDBX_Env::Copy),
			InstanceMethod("close", &MDBX_Env::Close),
		});

	exports.Set("Env", func);
}

MDBX_Env::MDBX_Env(const Napi::CallbackInfo &info) : Napi::ObjectWrap<MDBX_Env>(info) {
	Napi::Object options = info[0].ToObject();

	if (options.Get("path").IsString()) {
		this->path = std::filesystem::canonical(options.Get("path").ToString().Utf8Value()).string();
	} else {
		Napi::Error::New(info.Env(), "Env path is not defined").ThrowAsJavaScriptException();
		return;
	}

	this->env = MDBX_Env::createSharedEnv(this->path, [&](MDBX_env *env) {
		int rc = MDBX_SUCCESS;
		unsigned int flags = MDBX_ENV_DEFAULTS;
		mdbx_mode_t mode = 644;

		Napi::Object options = info[0].ToObject();

		if (options.Get("maxDbs").IsNumber()) {
			MDBX_dbi maxDbs = options.Get("maxDbs").ToNumber().Uint32Value();

			rc = mdbx_env_set_maxdbs(env, maxDbs);
			if (rc) {
				Utils::throwMdbxError(info.Env(), rc);
				return;
			}
		}

		if (options.Get("maxReaders").IsNumber()) {
			unsigned int maxReaders = options.Get("maxReaders").ToNumber().Uint32Value();

			rc = mdbx_env_set_maxreaders(env, maxReaders);
			if (rc) {
				Utils::throwMdbxError(info.Env(), rc);
				return;
			}
		}

		if (options.Get("geometry").IsObject()) {
			Napi::Object geometry = options.Get("geometry").ToObject();

			intptr_t size_lower, size_now, size_upper, growth_step, shrink_threshold, pagesize;

			Utils::setFromObject(&size_lower, geometry, "sizeLower", -1);
			Utils::setFromObject(&size_now, geometry, "sizeNow", -1);
			Utils::setFromObject(&size_upper, geometry, "sizeUpper", -1);
			Utils::setFromObject(&growth_step, geometry, "growthStep", -1);
			Utils::setFromObject(&shrink_threshold, geometry, "shrinkThreshold", -1);
			Utils::setFromObject(&pagesize, geometry, "pageSize", -1);

			rc = mdbx_env_set_geometry(env, size_lower, size_now, size_upper, growth_step, shrink_threshold, pagesize);
			if (rc) {
				Utils::throwMdbxError(info.Env(), rc);
				return;
			}
		}

		if (options.Get("mode").IsNumber()) {
			Napi::Number numMode = options.Get("mode").ToNumber();

			mode = numMode.Uint32Value();
		}

		if (options.Get("flags").IsObject()) {
			Napi::Object flagsObj = options.Get("flags").ToObject();

			Utils::setFromObject(&flags, MDBX_VALIDATION, flagsObj, "validation");
			Utils::setFromObject(&flags, MDBX_NOSUBDIR, flagsObj, "noSubdir");
			Utils::setFromObject(&flags, MDBX_RDONLY, flagsObj, "readOnly");
			Utils::setFromObject(&flags, MDBX_EXCLUSIVE, flagsObj, "exclusive");
			Utils::setFromObject(&flags, MDBX_ACCEDE, flagsObj, "accede");
			Utils::setFromObject(&flags, MDBX_WRITEMAP, flagsObj, "writemap");
			Utils::setFromObject(&flags, MDBX_NOSTICKYTHREADS, flagsObj, "noStickyThreads");
			Utils::setFromObject(&flags, MDBX_NORDAHEAD, flagsObj, "noReadAhead");
			Utils::setFromObject(&flags, MDBX_NOMEMINIT, flagsObj, "noMemInit");
			Utils::setFromObject(&flags, MDBX_LIFORECLAIM, flagsObj, "lifoReclaim");
			Utils::setFromObject(&flags, MDBX_NOMETASYNC, flagsObj, "noMetaSync");

			if (flagsObj.Get("syncMode").IsString()) {
				std::string syncMode = flagsObj.Get("syncMode").ToString();

				if (syncMode == "durable") {
					flags |= MDBX_SYNC_DURABLE;
				}

				if (syncMode == "safe") {
					flags |= MDBX_SAFE_NOSYNC;
				}

				if (syncMode == "utterly") {
					flags |= MDBX_UTTERLY_NOSYNC;
				}
			}
		}

		rc = mdbx_env_open(env, path.c_str(), static_cast<MDBX_env_flags>(flags), mode);
		if (rc) {
			Utils::throwMdbxError(info.Env(), rc);
			return;
		}

		// rc = mdbx_env_open_for_recovery(env, path.c_str(), 1, true);
		// if (rc) {
		// 	Utils::throwMdbxError(info.Env(), rc);
		// 	return;
		// }

		// rc = mdbx_env_turn_for_recovery(env, 1);
		// if (rc) {
		// 	Utils::throwMdbxError(info.Env(), rc);
		// 	return;
		// }
	});
}

Napi::Value MDBX_Env::Info(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	MDBX_envinfo envinfo;

	int rc = mdbx_env_info_ex(this->env.get(), nullptr, &envinfo, sizeof(envinfo));
	if (rc) {
		Utils::throwMdbxError(env, rc);

		return env.Undefined();
	}

	Napi::Object obj = Napi::Object::New(env);

	Napi::Object geoObj = Napi::Object::New(env);
	geoObj.Set("lower", envinfo.mi_geo.lower);
	geoObj.Set("upper", envinfo.mi_geo.upper);
	geoObj.Set("current", envinfo.mi_geo.current);
	geoObj.Set("shrink", envinfo.mi_geo.shrink);
	geoObj.Set("grow", envinfo.mi_geo.grow);
	obj.Set("geo", geoObj);

	obj.Set("mapsize", envinfo.mi_mapsize);
	obj.Set("last_pgno", envinfo.mi_last_pgno);
	obj.Set("recent_txnid", envinfo.mi_recent_txnid);
	obj.Set("latter_reader_txnid", envinfo.mi_latter_reader_txnid);
	obj.Set("self_latter_reader_txnid", envinfo.mi_self_latter_reader_txnid);

	Napi::Array metaTxnidArray = Napi::Array::New(env, 3);
	Napi::Array metaSignArray = Napi::Array::New(env, 3);
	for (uint32_t i = 0; i < 3; i++) {
		metaTxnidArray.Set(i, Napi::BigInt::New(env, envinfo.mi_meta_txnid[i]));
		metaSignArray.Set(i, Napi::BigInt::New(env, envinfo.mi_meta_sign[i]));
	}
	obj.Set("meta_txnid", metaTxnidArray);
	obj.Set("meta_sign", metaSignArray);

	obj.Set("maxreaders", envinfo.mi_maxreaders);
	obj.Set("numreaders", envinfo.mi_numreaders);
	obj.Set("dxb_pagesize", envinfo.mi_dxb_pagesize);
	obj.Set("sys_pagesize", envinfo.mi_sys_pagesize);

	Napi::Object bootidObj = Napi::Object::New(env);

	Napi::Object bootidCurrent = Napi::Object::New(env);
	bootidCurrent.Set("x", Napi::BigInt::New(env, envinfo.mi_bootid.current.x));
	bootidCurrent.Set("y", Napi::BigInt::New(env, envinfo.mi_bootid.current.y));
	bootidObj.Set("current", bootidCurrent);

	Napi::Array bootidMetaArray = Napi::Array::New(env, 3);
	for (uint32_t i = 0; i < 3; i++) {
		Napi::Object meta = Napi::Object::New(env);
		meta.Set("x", Napi::BigInt::New(env, envinfo.mi_bootid.meta[i].x));
		meta.Set("y", Napi::BigInt::New(env, envinfo.mi_bootid.meta[i].y));
		bootidMetaArray.Set(i, meta);
	}
	bootidObj.Set("meta", bootidMetaArray);

	obj.Set("bootid", bootidObj);

	obj.Set("unsync_volume", envinfo.mi_unsync_volume);
	obj.Set("autosync_threshold", envinfo.mi_autosync_threshold);
	obj.Set("since_sync_seconds16dot16", envinfo.mi_since_sync_seconds16dot16);
	obj.Set("autosync_period_seconds16dot16", envinfo.mi_autosync_period_seconds16dot16);
	obj.Set("since_reader_check_seconds16dot16", envinfo.mi_since_reader_check_seconds16dot16);
	obj.Set("mode", envinfo.mi_mode);

	Napi::Object pgopStatObj = Napi::Object::New(env);
	pgopStatObj.Set("newly", envinfo.mi_pgop_stat.newly);
	pgopStatObj.Set("cow", envinfo.mi_pgop_stat.cow);
	pgopStatObj.Set("clone", envinfo.mi_pgop_stat.clone);
	pgopStatObj.Set("split", envinfo.mi_pgop_stat.split);
	pgopStatObj.Set("merge", envinfo.mi_pgop_stat.merge);
	pgopStatObj.Set("spill", envinfo.mi_pgop_stat.spill);
	pgopStatObj.Set("unspill", envinfo.mi_pgop_stat.unspill);
	pgopStatObj.Set("wops", envinfo.mi_pgop_stat.wops);
	pgopStatObj.Set("prefault", envinfo.mi_pgop_stat.prefault);
	pgopStatObj.Set("mincore", envinfo.mi_pgop_stat.mincore);
	pgopStatObj.Set("msync", envinfo.mi_pgop_stat.msync);
	pgopStatObj.Set("fsync", envinfo.mi_pgop_stat.fsync);

	obj.Set("pgop_stat", pgopStatObj);

	Napi::Object dxbidObj = Napi::Object::New(env);
	dxbidObj.Set("x", Napi::BigInt::New(env, envinfo.mi_dxbid.x));
	dxbidObj.Set("y", Napi::BigInt::New(env, envinfo.mi_dxbid.y));
	obj.Set("dxbid", dxbidObj);

	return obj;
}

Napi::Value MDBX_Env::Stat(const Napi::CallbackInfo &info) {
	MDBX_stat stat;

	int rc = mdbx_env_stat_ex(this->env.get(), nullptr, &stat, sizeof(stat));
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);

		return info.Env().Undefined();
	}

	return Utils::mdbxStatToJsObject(info.Env(), stat);
}

Napi::Value MDBX_Env::GetDbi(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	EnvInstanceData *instanceData = Utils::envInstanceData(env);

	Napi::Value name = info[0];

	if (name.IsUndefined()) {
		Napi::TypeError::New(env, "Expected the first argument").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	Napi::External<MDBX_env> externalEnv = Napi::External<MDBX_env>::New(env, this->env.get());

	Napi::Object options = Napi::Object::New(env);
	if (info[1].IsObject()) {
		options = info[1].ToObject();
	}

	return instanceData->dbi->New({externalEnv, name, options});
}

Napi::Value MDBX_Env::GetTxn(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	EnvInstanceData *instanceData = Utils::envInstanceData(env);

	Napi::External<MDBX_env> externalEnv = Napi::External<MDBX_env>::New(env, this->env.get());

	Napi::Value options = info.Env().Undefined();
	if (info[0].IsObject()) {
		options = info[0].ToObject();
	}

	return instanceData->txn->New({externalEnv, options});
}

Napi::Value MDBX_Env::GcInfo(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	int rc;
	MDBX_txn *txn;
	MDBX_cursor *cursor;
	MDBX_envinfo mei;

	typedef uint32_t pgno_t;
	typedef uint64_t txnid_t;

	try {
		txn = Utils::argToMdbxTxn(env, info[0]);
		MDBX_txn_flags flags = mdbx_txn_flags(txn);

		if (!(flags & MDBX_TXN_RDONLY)) {
			throw Napi::Error::New(env, "Txn must be readOnly");
		}
	} catch (const Napi::Error &e) {
		e.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	rc = mdbx_cursor_open(txn, 0, &cursor);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
		return env.Undefined();
	}

	rc = mdbx_env_info_ex(this->env.get(), txn, &mei, sizeof(mei));
	if (rc) {
		mdbx_cursor_close(cursor);
		Utils::throwMdbxError(info.Env(), rc);
		return env.Undefined();
	}

	pgno_t pages = 0, *iptr;
	pgno_t reclaimable = 0;
	MDBX_val key, data;
	while (MDBX_SUCCESS == (rc = mdbx_cursor_get(cursor, &key, &data, MDBX_NEXT))) {
		iptr = (pgno_t *)data.iov_base;
		const pgno_t number = *iptr++;

		pages += number;
		if (mei.mi_latter_reader_txnid > *(txnid_t *)key.iov_base)
			reclaimable += number;
	}

	mdbx_cursor_close(cursor);

	Napi::Object obj = Napi::Object::New(env);

	obj.Set("pages", pages);
	obj.Set("reclaimable", reclaimable);

	return obj;
}

Napi::Value MDBX_Env::Readers(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	Napi::Array array = Napi::Array::New(env);

	struct CallbackContext {
		Napi::Env env;
		Napi::Array *array;
		uint32_t index;
	} ctx = {env, &array, 0};

	auto readerCallback = [](void *ctx, int num, int slot, mdbx_pid_t pid, mdbx_tid_t thread, uint64_t txnid, uint64_t lag, size_t bytes_used,
							  size_t bytes_retained) noexcept -> int {
		auto *callbackContext = static_cast<CallbackContext *>(ctx);
		Napi::Env env = callbackContext->env;

		Napi::Object obj = Napi::Object::New(env);
		obj.Set("num", num);
		obj.Set("slot", slot);
		obj.Set("pid", static_cast<uint32_t>(pid));
		obj.Set("thread", static_cast<uint64_t>(thread));
		obj.Set("txnid", txnid);
		obj.Set("lag", lag);
		obj.Set("bytes_used", static_cast<uint64_t>(bytes_used));
		obj.Set("bytes_retained", static_cast<uint64_t>(bytes_retained));

		callbackContext->array->Set(callbackContext->index++, obj);

		return MDBX_RESULT_TRUE;
	};

	int rc = mdbx_reader_list(this->env.get(), readerCallback, &ctx);
	if (rc != 0 && rc != MDBX_RESULT_TRUE) {
		Utils::throwMdbxError(env, rc);
		return env.Undefined();
	}

	return array;
}

void MDBX_Env::Sync(const Napi::CallbackInfo &info) {
	bool force = true;
	bool nonblock = false;

	if (info[0].IsObject()) {
		Napi::Object options = info[0].ToObject();

		if (options.Has("force")) {
			force = options.Get("force").ToBoolean().Value();
		}

		if (options.Has("nonblock")) {
			nonblock = options.Get("nonblock").ToBoolean().Value();
		}
	}

	int rc = mdbx_env_sync_ex(this->env.get(), force, nonblock);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
	}
}

void MDBX_Env::Copy(const Napi::CallbackInfo &info) {
	std::string dest = info[0].ToString();

	int flags = MDBX_CP_DEFAULTS;

	if (info[1].IsObject()) {
		Napi::Object options = info[1].ToObject();

		Utils::setFromObject(&flags, MDBX_CP_COMPACT, options, "compact");
		Utils::setFromObject(&flags, MDBX_CP_FORCE_DYNAMIC_SIZE, options, "forceDynamicSize");
		Utils::setFromObject(&flags, MDBX_CP_DONT_FLUSH, options, "dontFlush");
		Utils::setFromObject(&flags, MDBX_CP_THROTTLE_MVCC, options, "throttleMvcc");
	}

	int rc = mdbx_env_copy(this->env.get(), dest.c_str(), static_cast<MDBX_copy_flags>(flags));
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
	}
}

void MDBX_Env::Close(const Napi::CallbackInfo &info) {
	int rc = mdbx_env_close(this->env.get());
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
		return;
	}

	MDBX_Env::deleteSharedEnv(this->path);
}

MDBX_Env::~MDBX_Env() {
	// 1 + 1 = 2 [map ptr + this class instance ptr]
	if (this->env.use_count() <= 2) {
		mdbx_env_close(this->env.get());
		MDBX_Env::deleteSharedEnv(this->path);
	}
}
