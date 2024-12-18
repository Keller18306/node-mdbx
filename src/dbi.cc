#include "dbi.h"

#include "cursor.h"
#include "txn.h"

void MDBX_Dbi::Init(Napi::Env env) {
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env, "MDBXDbi",
		{
			InstanceMethod("stat", &MDBX_Dbi::Stat),
			InstanceMethod("getCursor", &MDBX_Dbi::GetCursor),
			InstanceMethod("rename", &MDBX_Dbi::Rename),
			InstanceMethod("drop", &MDBX_Dbi::Drop),
			InstanceMethod("close", &MDBX_Dbi::Close),
		});

	Napi::FunctionReference *constructor = new Napi::FunctionReference();
	*constructor = Napi::Persistent(func);
	// constructor->SuppressDestruct();

	EnvInstanceData *instanceData = Utils::envInstanceData(env);
	instanceData->dbi = constructor;
}

MDBX_Dbi::MDBX_Dbi(const Napi::CallbackInfo &info) : Napi::ObjectWrap<MDBX_Dbi>(info) {
	Napi::Env env = info.Env();

	if (!info[0].IsExternal()) {
		Napi::TypeError::New(env, "Invalid constructor data").ThrowAsJavaScriptException();
		return;
	}

	int rc;
	MDBX_txn *txn = nullptr;
	bool closeTxn = false;
	unsigned int dbiFlags = MDBX_DB_DEFAULTS;
	MDBX_txn_flags_t txnFlags = MDBX_TXN_READWRITE;

	this->env = info[0].As<Napi::External<MDBX_env>>().Data();
	Napi::Value nameValue = info[1];
	// const char *name = info[1].IsNull() ? nullptr : info[1].ToString().Utf8Value().c_str();

	if (info[2].IsObject()) {
		Napi::Object options = info[2].ToObject();

		Utils::setFromObject(&dbiFlags, MDBX_REVERSEKEY, options, "reverseKey");
		Utils::setFromObject(&dbiFlags, MDBX_DUPSORT, options, "dupSort");
		Utils::setFromObject(&dbiFlags, MDBX_INTEGERKEY, options, "integerKey");
		Utils::setFromObject(&dbiFlags, MDBX_DUPFIXED, options, "dupFixed");
		Utils::setFromObject(&dbiFlags, MDBX_INTEGERDUP, options, "integerDup");
		Utils::setFromObject(&dbiFlags, MDBX_REVERSEDUP, options, "reverseDup");
		Utils::setFromObject(&dbiFlags, MDBX_CREATE, options, "create");
		Utils::setFromObject(&dbiFlags, MDBX_DB_ACCEDE, options, "accede");

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

	if (!txn) {
		closeTxn = true;

		unsigned int envFlags;
		rc = mdbx_env_get_flags(this->env, &envFlags);
		if (rc) {
			Utils::throwMdbxError(env, rc);
			return;
		}

		if (envFlags & MDBX_RDONLY) {
			txnFlags = MDBX_TXN_RDONLY;
		}

		rc = mdbx_txn_begin(this->env, nullptr, txnFlags, &txn);
		if (rc) {
			Utils::throwMdbxError(env, rc);
			return;
		}
	}

	try {
		if (nameValue.IsNull()) {
			rc = mdbx_dbi_open2(txn, nullptr, static_cast<MDBX_db_flags>(dbiFlags), &this->dbi);
		} else {
			buffer_t buffer;
			MDBX_val value;

			try {
				value = Utils::argToMdbxValue(env, nameValue, buffer);
			} catch (const Napi::Error &e) {
				e.ThrowAsJavaScriptException();
				return;
			}

			rc = mdbx_dbi_open2(txn, &value, static_cast<MDBX_db_flags>(dbiFlags), &this->dbi);
		}

		if (rc) {
			Utils::throwMdbxError(env, rc);
			return;
		}

		if (closeTxn) {
			rc = mdbx_txn_commit(txn);
			if (rc) {
				Utils::throwMdbxError(env, rc);
				return;
			}
		}
	} catch (...) {
		if (closeTxn) {
			mdbx_txn_abort(txn);
		}

		throw;
	}
}

Napi::Value MDBX_Dbi::Stat(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	int rc;
	MDBX_stat stat;
	MDBX_txn *txn = nullptr;
	bool closeTxn = false;

	if (info[0].IsObject()) {
		try {
			txn = Utils::argToMdbxTxn(env, info[0]);
		} catch (const Napi::Error &e) {
			e.ThrowAsJavaScriptException();
			return env.Undefined();
		}
	}

	if (!txn) {
		closeTxn = true;

		rc = mdbx_txn_begin(this->env, nullptr, MDBX_TXN_RDONLY, &txn);
		if (rc) {
			Utils::throwMdbxError(env, rc);
			return env.Undefined();
		}
	}

	try {
		rc = mdbx_dbi_stat(txn, this->dbi, &stat, sizeof(stat));
		if (rc) {
			Utils::throwMdbxError(env, rc);
			return env.Undefined();
		}

		if (closeTxn) {
			rc = mdbx_txn_commit(txn);
			if (rc) {
				Utils::throwMdbxError(env, rc);
				return env.Undefined();
			}
		}
	} catch (...) {
		if (closeTxn) {
			mdbx_txn_abort(txn);
		}

		throw;
	}

	return Utils::mdbxStatToJsObject(env, stat);
}

Napi::Value MDBX_Dbi::GetCursor(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	EnvInstanceData *instanceData = Utils::envInstanceData(env);

	Napi::External<MDBX_env> externalEnv = Napi::External<MDBX_env>::New(env, this->env);
	Napi::External<MDBX_dbi> externalDbi = Napi::External<MDBX_dbi>::New(env, &this->dbi);

	Napi::Value options = env.Undefined();
	if (info[0].IsObject()) {
		options = info[0].ToObject();
	}

	return instanceData->cursor->New({externalEnv, externalDbi, options});
}

void MDBX_Dbi::Rename(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	int rc;
	MDBX_txn *txn = nullptr;
	bool closeTxn = false;

	std::string name = info[0].ToString().Utf8Value();

	if (info[1].IsObject()) {
		try {
			txn = Utils::argToMdbxTxn(env, info[1]);
		} catch (const Napi::Error &e) {
			e.ThrowAsJavaScriptException();
			return;
		}
	}

	if (!txn) {
		closeTxn = true;

		rc = mdbx_txn_begin(this->env, nullptr, MDBX_TXN_READWRITE, &txn);
		if (rc) {
			Utils::throwMdbxError(env, rc);
			return;
		}
	}

	try {
		rc = mdbx_dbi_rename(txn, this->dbi, name.c_str());
		if (rc) {
			Utils::throwMdbxError(env, rc);
			return;
		}

		if (closeTxn) {
			rc = mdbx_txn_commit(txn);
			if (rc) {
				Utils::throwMdbxError(env, rc);
				return;
			}
		}
	} catch (...) {
		if (closeTxn) {
			mdbx_txn_abort(txn);
		}

		throw;
	}
}

void MDBX_Dbi::Drop(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	int rc;
	MDBX_txn *txn = nullptr;
	bool closeTxn = false;
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

	if (!txn) {
		closeTxn = true;

		rc = mdbx_txn_begin(this->env, nullptr, MDBX_TXN_READWRITE, &txn);
		if (rc) {
			Utils::throwMdbxError(env, rc);
			return;
		}
	}

	try {
		rc = mdbx_drop(txn, this->dbi, del);
		if (rc) {
			Utils::throwMdbxError(env, rc);
			return;
		}

		if (closeTxn) {
			rc = mdbx_txn_commit(txn);
			if (rc) {
				Utils::throwMdbxError(env, rc);
				return;
			}
		}
	} catch (...) {
		if (closeTxn) {
			mdbx_txn_abort(txn);
		}

		throw;
	}
}

void MDBX_Dbi::Close(const Napi::CallbackInfo &info) {
	if (this->env && this->dbi) {
		mdbx_dbi_close(this->env, this->dbi);
	}
}

MDBX_Dbi::~MDBX_Dbi() {
	if (this->env && this->dbi) {
		mdbx_dbi_close(this->env, this->dbi);
	}
}