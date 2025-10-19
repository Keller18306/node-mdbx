#include "dbi.h"

#include "cursor.h"
#include "txn.h"

void MDBX_Native_Dbi::Init(Napi::Env env) {
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env, "MDBXDbi",
		{
			InstanceMethod("stat", &MDBX_Native_Dbi::Stat),
			InstanceMethod("getCursor", &MDBX_Native_Dbi::GetCursor),
			InstanceMethod("minKeySize", &MDBX_Native_Dbi::MinKeySize),
			InstanceMethod("maxKeySize", &MDBX_Native_Dbi::MaxKeySize),
			InstanceMethod("minValSize", &MDBX_Native_Dbi::MinValSize),
			InstanceMethod("maxValSize", &MDBX_Native_Dbi::MaxValSize),
			InstanceMethod("rename", &MDBX_Native_Dbi::Rename),
			InstanceMethod("drop", &MDBX_Native_Dbi::Drop),
			InstanceMethod("close", &MDBX_Native_Dbi::Close),
		});

	Napi::FunctionReference *constructor = new Napi::FunctionReference();
	*constructor = Napi::Persistent(func);
	// constructor->SuppressDestruct();

	EnvInstanceData *instanceData = Utils::envInstanceData(env);
	instanceData->dbi = constructor;

	env.AddCleanupHook([](Napi::FunctionReference *ctor) { delete ctor; }, constructor);
}

MDBX_Native_Dbi::MDBX_Native_Dbi(const Napi::CallbackInfo &info) : Napi::ObjectWrap<MDBX_Native_Dbi>(info) {
	Napi::Env env = info.Env();

	try {
		Utils::_assertMdbxNativeTxn(env, info[0]);
	} catch (const Napi::Error &e) {
		e.ThrowAsJavaScriptException();
		return;
	}

	if (!info[1].IsExternal()) {
		Napi::TypeError::New(env, "Invalid constructor data").ThrowAsJavaScriptException();
		return;
	}

	int rc;
	unsigned int dbiFlags = MDBX_DB_DEFAULTS;

	this->txn = Utils::argToMdbxTxn(env, info[0]);
	this->nativeTxnRef_ = Napi::Persistent(info[0].ToObject());
	this->nativeTxnRef_.Ref();

	this->env = info[1].As<Napi::External<MDBX_env>>().Data();
	Napi::Value nameValue = info[2];
	// const char *name = info[1].IsNull() ? nullptr : info[1].ToString().Utf8Value().c_str();

	if (info[3].IsObject()) {
		Napi::Object options = info[3].ToObject();

		Utils::setFromObject(&dbiFlags, MDBX_REVERSEKEY, options, "reverseKey");
		Utils::setFromObject(&dbiFlags, MDBX_DUPSORT, options, "dupSort");
		Utils::setFromObject(&dbiFlags, MDBX_INTEGERKEY, options, "integerKey");
		Utils::setFromObject(&dbiFlags, MDBX_DUPFIXED, options, "dupFixed");
		Utils::setFromObject(&dbiFlags, MDBX_INTEGERDUP, options, "integerDup");
		Utils::setFromObject(&dbiFlags, MDBX_REVERSEDUP, options, "reverseDup");
		Utils::setFromObject(&dbiFlags, MDBX_CREATE, options, "create");
		Utils::setFromObject(&dbiFlags, MDBX_DB_ACCEDE, options, "accede");
	}

	try {
		if (nameValue.IsNull()) {
			rc = mdbx_dbi_open2(this->txn, nullptr, static_cast<MDBX_db_flags>(dbiFlags), &this->dbi);
		} else {
			buffer_t buffer;
			MDBX_val value;

			try {
				value = Utils::argToMdbxValue(nameValue, buffer);
			} catch (const Napi::Error &e) {
				e.ThrowAsJavaScriptException();
				return;
			}

			rc = mdbx_dbi_open2(this->txn, &value, static_cast<MDBX_db_flags>(dbiFlags), &this->dbi);
		}

		if (rc) {
			Utils::throwMdbxError(env, rc);
			return;
		}

		rc = mdbx_dbi_flags(this->txn, this->dbi, &this->flags);
		if (rc) {
			Utils::throwMdbxError(env, rc);
			return;
		}
	} catch (...) {
		throw;
	}
}

Napi::Value MDBX_Native_Dbi::Stat(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	MDBX_stat stat;
	MDBX_txn *txn = this->txn;

	if (info[0].IsObject()) {
		try {
			txn = Utils::argToMdbxTxn(env, info[0]);
		} catch (const Napi::Error &e) {
			e.ThrowAsJavaScriptException();
			return env.Undefined();
		}
	}

	int rc = mdbx_dbi_stat(txn, this->dbi, &stat, sizeof(stat));
	if (rc) {
		Utils::throwMdbxError(env, rc);
		return env.Undefined();
	}

	return Utils::mdbxStatToJsObject(env, stat);
}

Napi::Value MDBX_Native_Dbi::GetCursor(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	EnvInstanceData *instanceData = Utils::envInstanceData(env);

	Napi::Object self = this->Value();

	Napi::Value options = env.Undefined();
	if (info[0].IsObject()) {
		options = info[0].ToObject();
	}

	return instanceData->cursor->New({self, options});
}

Napi::Value MDBX_Native_Dbi::MinKeySize(const Napi::CallbackInfo &info) {
	int size = mdbx_limits_keysize_min(static_cast<MDBX_db_flags_t>(this->flags));

	return Napi::Number::New(info.Env(), size);
}

Napi::Value MDBX_Native_Dbi::MaxKeySize(const Napi::CallbackInfo &info) {
	int size = mdbx_env_get_maxkeysize_ex(this->env, static_cast<MDBX_db_flags_t>(this->flags));

	return Napi::Number::New(info.Env(), size);
}

Napi::Value MDBX_Native_Dbi::MinValSize(const Napi::CallbackInfo &info) {
	int size = mdbx_limits_valsize_min(static_cast<MDBX_db_flags_t>(this->flags));

	return Napi::Number::New(info.Env(), size);
}

Napi::Value MDBX_Native_Dbi::MaxValSize(const Napi::CallbackInfo &info) {
	int size = mdbx_env_get_maxvalsize_ex(this->env, static_cast<MDBX_db_flags_t>(this->flags));

	return Napi::Number::New(info.Env(), size);
}

void MDBX_Native_Dbi::Rename(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	MDBX_txn *txn = this->txn;
	std::string name = info[0].ToString().Utf8Value();

	if (info[1].IsObject()) {
		try {
			txn = Utils::argToMdbxTxn(env, info[1]);
		} catch (const Napi::Error &e) {
			e.ThrowAsJavaScriptException();
			return;
		}
	}

	int rc = mdbx_dbi_rename(txn, this->dbi, name.c_str());
	if (rc) {
		Utils::throwMdbxError(env, rc);
		return;
	}
}

void MDBX_Native_Dbi::Drop(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	MDBX_txn *txn = this->txn;
	bool del = false;

	if (info[0].IsObject()) {
		Napi::Object options = info[0].ToObject();

		if (options.Get("del").IsBoolean()) {
			del = options.Get("del").ToBoolean().Value();
		}

		Napi::Value txnValue = options.Get("txn");
		if (txnValue.IsObject()) {
			try {
				txn = Utils::argToMdbxTxn(env, txnValue);
			} catch (const Napi::Error &e) {
				e.ThrowAsJavaScriptException();
				return;
			}
		}
	}

	int rc = mdbx_drop(txn, this->dbi, del);
	if (rc) {
		Utils::throwMdbxError(env, rc);
		return;
	}
}

void MDBX_Native_Dbi::Close(const Napi::CallbackInfo &info) {
	if (this->env && this->dbi) {
		mdbx_dbi_close(this->env, this->dbi);
	}
}

MDBX_Native_Dbi::~MDBX_Native_Dbi() {
	if (!this->nativeTxnRef_.IsEmpty()) {
		this->nativeTxnRef_.Unref();
	}

	if (this->env && this->dbi) {
		mdbx_dbi_close(this->env, this->dbi);
	}
}