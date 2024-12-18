#include "txn.h"

#include "utils.h"

void MDBX_Txn::Init(Napi::Env env) {
	Napi::Function func = DefineClass(env, "MDBXTxn",
		{
			InstanceMethod("info", &MDBX_Txn::Info),
			InstanceMethod("commit", &MDBX_Txn::Commit),
			InstanceMethod("abort", &MDBX_Txn::Abort),
			InstanceMethod("park", &MDBX_Txn::Park),
			InstanceMethod("unpark", &MDBX_Txn::Unpark),
			InstanceMethod("reset", &MDBX_Txn::Reset),
			InstanceMethod("renew", &MDBX_Txn::Renew),
		});

	Napi::FunctionReference *constructor = new Napi::FunctionReference();
	*constructor = Napi::Persistent(func);
	// constructor->SuppressDestruct();

	EnvInstanceData *instanceData = Utils::envInstanceData(env);
	instanceData->txn = constructor;
}

MDBX_Txn::MDBX_Txn(const Napi::CallbackInfo &info) : Napi::ObjectWrap<MDBX_Txn>(info) {
	unsigned int envFlags;
	unsigned int txnFlags = MDBX_TXN_READWRITE;
	int rc;
	MDBX_txn *parentTxn = nullptr;

	MDBX_env *env = info[0].As<Napi::External<MDBX_env>>().Data();
	// MDBX_Txn parentTxn = info[1].As<Napi::External<MDBX_Txn>>().Data();

	rc = mdbx_env_get_flags(env, &envFlags);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
		return;
	}

	if (envFlags & MDBX_RDONLY) {
		txnFlags = MDBX_TXN_RDONLY;
	}

	if (info[1].IsObject()) {
		Napi::Object options = info[1].ToObject();

		Utils::setFromObject(&txnFlags, MDBX_TXN_RDONLY, options, "readOnly");
		Utils::setFromObject(&txnFlags, MDBX_TXN_RDONLY_PREPARE, options, "prepare");
		Utils::setFromObject(&txnFlags, MDBX_TXN_TRY, options, "try");
		Utils::setFromObject(&txnFlags, MDBX_TXN_NOMETASYNC, options, "noMetaSync");
		Utils::setFromObject(&txnFlags, MDBX_TXN_NOSYNC, options, "noSync");

		Napi::Value parentTxnValue = options.Get("parentTxn");
		if (parentTxnValue.IsObject()) {
			try {
				parentTxn = Utils::argToMdbxTxn(info.Env(), parentTxnValue);
			} catch (const Napi::Error &e) {
				e.ThrowAsJavaScriptException();
				return;
			}
		}
	}

	rc = mdbx_txn_begin(env, parentTxn, static_cast<MDBX_txn_flags>(txnFlags), &this->txn);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
		return;
	}
}

Napi::Value MDBX_Txn::Info(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	MDBX_txn_info txninfo;

	int rc = mdbx_txn_info(this->txn, &txninfo, false);
	if (rc) {
		Utils::throwMdbxError(env, rc);

		return env.Undefined();
	}

	Napi::Object obj = Napi::Object::New(env);

	obj.Set("id", txninfo.txn_id);
	obj.Set("reader_lag", txninfo.txn_reader_lag);
	obj.Set("space_used", txninfo.txn_space_used);
	obj.Set("space_limit_soft", txninfo.txn_space_limit_soft);
	obj.Set("space_limit_hard", txninfo.txn_space_limit_hard);
	obj.Set("space_retired", txninfo.txn_space_retired);
	obj.Set("space_leftover", txninfo.txn_space_leftover);
	obj.Set("space_dirty", txninfo.txn_space_dirty);

	return obj;
}

void MDBX_Txn::Commit(const Napi::CallbackInfo &info) {
	int rc = mdbx_txn_commit(this->txn);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
	}
}

void MDBX_Txn::Abort(const Napi::CallbackInfo &info) {
	if (this->txn) {
		int rc = mdbx_txn_abort(this->txn);
		if (rc) {
			Utils::throwMdbxError(info.Env(), rc);
			return;
		}

		this->txn = nullptr;
	}
}

void MDBX_Txn::Park(const Napi::CallbackInfo &info) {
	bool autounpark = info[0].ToBoolean().Value();

	int rc = mdbx_txn_park(this->txn, autounpark);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
	}
}

void MDBX_Txn::Unpark(const Napi::CallbackInfo &info) {
	bool restart_if_ousted = info[0].ToBoolean().Value();

	int rc = mdbx_txn_unpark(this->txn, restart_if_ousted);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
	}
}

void MDBX_Txn::Reset(const Napi::CallbackInfo &info) {
	int rc = mdbx_txn_reset(this->txn);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
	}
}

void MDBX_Txn::Renew(const Napi::CallbackInfo &info) {
	int rc = mdbx_txn_renew(this->txn);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
	}
}

MDBX_Txn::~MDBX_Txn() {
	if (this->txn) {
		mdbx_txn_abort(this->txn);
	}
}