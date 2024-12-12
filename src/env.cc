#include "env.h"

#include "dbi.h"
#include "txn.h"
#include "utils.h"

void MDBX_Env::Init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(env, "MDBXEnv",
		{
			InstanceMethod("info", &MDBX_Env::Info),
			InstanceMethod("stat", &MDBX_Env::Stat),
			InstanceMethod("getDbi", &MDBX_Env::GetDbi),
			InstanceMethod("getTxn", &MDBX_Env::GetTxn),
			InstanceMethod("close", &MDBX_Env::Close),
		});

	// Napi::FunctionReference *constructor = new Napi::FunctionReference();
	// *constructor = Napi::Persistent(func);
	// env.SetInstanceData(constructor);

	exports.Set("Env", func);
}

MDBX_Env::MDBX_Env(const Napi::CallbackInfo &info) : Napi::ObjectWrap<MDBX_Env>(info) {
	int rc = MDBX_SUCCESS;
	unsigned int flags = MDBX_ENV_DEFAULTS;
	mdbx_mode_t mode = 644;

	std::string path = info[0].ToString().Utf8Value();

	if (info[1].IsObject()) {
		Napi::Object options = info[1].ToObject();

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
	}

	rc = mdbx_env_create(&this->env);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
		return;
	}

	if (info[1].IsObject()) {
		Napi::Object options = info[1].ToObject();

		if (options.Get("maxdbs").IsNumber()) {
			MDBX_dbi maxdbs = options.Get("maxdbs").ToNumber().Uint32Value();

			rc = mdbx_env_set_maxdbs(this->env, maxdbs);
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

			rc = mdbx_env_set_geometry(this->env, size_lower, size_now, size_upper, growth_step, shrink_threshold, pagesize);
			if (rc) {
				Utils::throwMdbxError(info.Env(), rc);
				return;
			}
		}
	}

	rc = mdbx_env_open(this->env, path.c_str(), static_cast<MDBX_env_flags>(flags), mode);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
		return;
	}
}

Napi::Value MDBX_Env::Info(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	MDBX_envinfo envinfo;

	int rc = mdbx_env_info_ex(this->env, nullptr, &envinfo, sizeof(envinfo));
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

	int rc = mdbx_env_stat_ex(this->env, nullptr, &stat, sizeof(stat));
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);

		return info.Env().Undefined();
	}

	return Utils::mdbxStatToJsObject(info.Env(), stat);
}

Napi::Value MDBX_Env::GetDbi(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	Napi::Value name = info[0];

	if (!name.IsString() && !name.IsNull()) {
		Napi::TypeError::New(env, "Expected a string/null as the first argument").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	Napi::External<MDBX_env> externalEnv = Napi::External<MDBX_env>::New(env, this->env);

	Napi::Object options = Napi::Object::New(env);
	if (info[1].IsObject()) {
		options = info[1].ToObject();
	}

	return MDBX_Dbi::constructor.New({externalEnv, name, options});
}

Napi::Value MDBX_Env::GetTxn(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	Napi::External<MDBX_env> externalEnv = Napi::External<MDBX_env>::New(env, this->env);

	Napi::Value options = info.Env().Undefined();
	if (info[0].IsObject()) {
		options = info[0].ToObject();
	}

	return MDBX_Txn::constructor.New({externalEnv, options});
}

void MDBX_Env::Close(const Napi::CallbackInfo &info) {
	int rc = mdbx_env_close_ex(this->env, false);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
	}
}

MDBX_Env::~MDBX_Env() {
	if (this->env) {
		mdbx_env_close_ex(this->env, true);
	}
}
