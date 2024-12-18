#include "cursor.h"

Napi::FunctionReference MDBX_Cursor::constructor;
#include "txn.h"

ValueType strToKeyTypeNum(std::string str) {
	if (str == "buffer") {
		return ValueType_Buffer;
	}

	if (str == "string") {
		return ValueType_String;
	}

	if (str == "number") {
		return ValueType_Number;
	}

	if (str == "bigint") {
		return ValueType_BigInt;
	}

	return ValueType_Buffer;
	// throw std::runtime_error("Unknown data type");
}

void MDBX_Cursor::Init(Napi::Env env) {
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env, "MDBXCursor",
		{
			InstanceMethod("getKey", &MDBX_Cursor::GetKey),
			InstanceMethod("getValue", &MDBX_Cursor::GetValue),

			InstanceMethod("count", &MDBX_Cursor::Count),
			InstanceMethod("dupStat", &MDBX_Cursor::DupStat),

			InstanceMethod("first", &MDBX_Cursor::First),
			InstanceMethod("firstDup", &MDBX_Cursor::FirstDup),
			InstanceMethod("last", &MDBX_Cursor::Last),
			InstanceMethod("lastDup", &MDBX_Cursor::LastDup),
			InstanceMethod("next", &MDBX_Cursor::Next),
			InstanceMethod("nextDup", &MDBX_Cursor::NextDup),
			InstanceMethod("nextNoDup", &MDBX_Cursor::NextNoDup),
			InstanceMethod("prev", &MDBX_Cursor::Prev),
			InstanceMethod("prevDup", &MDBX_Cursor::PrevDup),
			InstanceMethod("prevNoDup", &MDBX_Cursor::PrevNoDup),

			InstanceMethod("set", &MDBX_Cursor::Set),
			InstanceMethod("range", &MDBX_Cursor::Range),

			InstanceMethod("put", &MDBX_Cursor::Put),
			InstanceMethod("del", &MDBX_Cursor::Del),

			InstanceMethod("reset", &MDBX_Cursor::Reset),
			InstanceMethod("bind", &MDBX_Cursor::Bind),
			InstanceMethod("close", &MDBX_Cursor::Close),
		});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();
}

MDBX_Cursor::MDBX_Cursor(const Napi::CallbackInfo &info) : Napi::ObjectWrap<MDBX_Cursor>(info) {
	Napi::Env env = info.Env();

	if (info.Length() < 3 || !info[0].IsExternal() || !info[1].IsExternal() || !info[2].IsObject()) {
		Napi::TypeError::New(info.Env(), "Expected MDBX_env pointer, dbi, and an options object").ThrowAsJavaScriptException();
		return;
	}

	int rc;
	MDBX_txn_flags_t txnFlags = MDBX_TXN_READWRITE;

	this->env = info[0].As<Napi::External<MDBX_env>>().Data();
	this->dbi = info[1].As<Napi::External<MDBX_dbi>>().Data();

	if (info[2].IsObject()) {
		Napi::Object options = info[2].ToObject();

		if (options.Get("keyType").IsString()) {
			std::string type = options.Get("keyType").ToString();

			Utils::stringToLower(type);

			this->keyType = strToKeyTypeNum(type);
		}

		if (options.Get("dataType").IsString()) {
			std::string type = options.Get("dataType").ToString();

			Utils::stringToLower(type);

			this->dataType = strToKeyTypeNum(type);
		}

		if (options.Get("stringEncoding").IsString()) {
			std::string encoding = options.Get("stringEncoding").ToString();

			Utils::stringToLower(encoding);

			this->isStrUtf16 = encoding == "utf16";
		}

		if (options.Get("integerByteOrder").IsString()) {
			std::string order = options.Get("integerByteOrder").ToString();

			Utils::stringToLower(order);

			this->isIntBE = order == "be";
		}

		if (options.Get("txn").IsObject()) {
			try {
				txn = Utils::argToMdbxTxn(env, options.Get("txn"));
			} catch (const Napi::Error &e) {
				e.ThrowAsJavaScriptException();
				return;
			}
		}
	}

	if (!txn) {
		this->closeTxn = true;

		unsigned int envFlags;
		rc = mdbx_env_get_flags(this->env, &envFlags);
		if (rc) {
			Utils::throwMdbxError(env, rc);
			return;
		}

		if (envFlags & MDBX_RDONLY) {
			txnFlags = MDBX_TXN_RDONLY;
		}

		rc = mdbx_txn_begin(this->env, nullptr, txnFlags, &this->txn);
		if (rc) {
			Utils::throwMdbxError(env, rc);
			return;
		}
	}

	rc = mdbx_cursor_open(this->txn, *this->dbi, &this->cursor);
	if (rc) {
		Utils::throwMdbxError(env, rc);
		return;
	}

	rc = mdbx_dbi_flags(this->txn, *this->dbi, &this->dbiFlags);
	if (rc) {
		Utils::throwMdbxError(env, rc);
		return;
	}
}

Napi::Value MDBX_Cursor::GetKey(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (!this->hasData) {
		return env.Null();
	}

	return Utils::vectorBufferToArg(env, this->keyType, this->isIntBE, this->_keyBuffer);
}

Napi::Value MDBX_Cursor::GetValue(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (!this->hasData) {
		return env.Null();
	}

	return Utils::vectorBufferToArg(env, this->dataType, this->isIntBE, this->_dataBuffer);
}

Napi::Value MDBX_Cursor::Count(const Napi::CallbackInfo &info) {
	size_t count;

	if (!(this->dbiFlags & MDBX_DUPSORT)) {
		return Utils::Error(info.Env(), "Dbi is not MDB_DUPSORT");
	}

	int rc = mdbx_cursor_count(this->cursor, &count);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
		return info.Env().Undefined();
	}

	return Napi::Number::New(info.Env(), count);
}

Napi::Value MDBX_Cursor::DupStat(const Napi::CallbackInfo &info) {
	MDBX_stat stat;

	if (!(this->dbiFlags & MDBX_DUPSORT)) {
		return Utils::Error(info.Env(), "Dbi is not MDB_DUPSORT");
	}

	int rc = mdbx_cursor_count_ex(this->cursor, nullptr, &stat, sizeof(stat));
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
		return info.Env().Undefined();
	}

	return Utils::mdbxStatToJsObject(info.Env(), stat);
}

Napi::Value MDBX_Cursor::_commonMove(const Napi::CallbackInfo &info, MDBX_cursor_op op, bool checkDupFlag) {
	Napi::Env env = info.Env();

	if (checkDupFlag && !(this->dbiFlags & MDBX_DUPSORT)) {
		return Utils::Error(env, "Dbi is not MDB_DUPSORT");
	}

	MDBX_val key, data;
	int rc = mdbx_cursor_get(this->cursor, &key, &data, op);
	if (rc) {
		if (rc == MDBX_NOTFOUND) {
			this->hasData = false;
			this->_keyBuffer.clear();
			this->_dataBuffer.clear();

			return env.Null();
		}

		Utils::throwMdbxError(env, rc);
		return env.Undefined();
	}

	this->hasData = true;
	Utils::mdbxValueToVectorBuffer(this->_keyBuffer, key);
	Utils::mdbxValueToVectorBuffer(this->_dataBuffer, data);

	return Utils::vectorBufferToArg(env, this->keyType, this->isIntBE, this->_keyBuffer);
}

Napi::Value MDBX_Cursor::_commonSet(const Napi::CallbackInfo &info, MDBX_cursor_op opKeyOnly, MDBX_cursor_op opKeyValue) {
	Napi::Env env = info.Env();

	if (info.Length() < 1) {
		return Utils::Error(env, "Key not provided");
	}

	MDBX_val key, data;
	MDBX_cursor_op op;

	try {
		key = Utils::argToMdbxValue(env, info[0], this->_keyBuffer);

		if (info.Length() >= 2) {
			if (!(this->dbiFlags & MDBX_DUPSORT)) {
				return Utils::Error(env, "Dbi is not MDB_DUPSORT");
			}

			op = opKeyValue;
			data = Utils::argToMdbxValue(env, info[1], this->_dataBuffer);
		} else {
			op = opKeyOnly;
		}
	} catch (const Napi::Error &e) {
		e.ThrowAsJavaScriptException();
		return env.Undefined();
	}

	int rc = mdbx_cursor_get(this->cursor, &key, &data, op);
	if (rc) {
		if (rc == MDBX_NOTFOUND) {
			this->hasData = false;
			this->_keyBuffer.clear();
			this->_dataBuffer.clear();

			return env.Null();
		}

		Utils::throwMdbxError(env, rc);
		return env.Undefined();
	}

	this->hasData = true;
	Utils::mdbxValueToVectorBuffer(this->_keyBuffer, key);
	Utils::mdbxValueToVectorBuffer(this->_dataBuffer, data);

	return Utils::vectorBufferToArg(env, this->keyType, this->isIntBE, this->_keyBuffer);
}

Napi::Value MDBX_Cursor::First(const Napi::CallbackInfo &info) { return _commonMove(info, MDBX_FIRST, false); }

Napi::Value MDBX_Cursor::FirstDup(const Napi::CallbackInfo &info) { return _commonMove(info, MDBX_FIRST_DUP, true); }

Napi::Value MDBX_Cursor::Last(const Napi::CallbackInfo &info) { return _commonMove(info, MDBX_LAST, false); }

Napi::Value MDBX_Cursor::LastDup(const Napi::CallbackInfo &info) { return _commonMove(info, MDBX_LAST_DUP, true); }

Napi::Value MDBX_Cursor::Next(const Napi::CallbackInfo &info) { return _commonMove(info, MDBX_NEXT, false); }

Napi::Value MDBX_Cursor::NextDup(const Napi::CallbackInfo &info) { return _commonMove(info, MDBX_NEXT_DUP, true); }

Napi::Value MDBX_Cursor::NextNoDup(const Napi::CallbackInfo &info) { return _commonMove(info, MDBX_NEXT_NODUP, false); }

Napi::Value MDBX_Cursor::Prev(const Napi::CallbackInfo &info) { return _commonMove(info, MDBX_PREV, false); }

Napi::Value MDBX_Cursor::PrevDup(const Napi::CallbackInfo &info) { return _commonMove(info, MDBX_PREV_DUP, true); }

Napi::Value MDBX_Cursor::PrevNoDup(const Napi::CallbackInfo &info) { return _commonMove(info, MDBX_PREV_NODUP, false); }

Napi::Value MDBX_Cursor::Set(const Napi::CallbackInfo &info) { return _commonSet(info, MDBX_SET_KEY, MDBX_GET_BOTH); }

Napi::Value MDBX_Cursor::Range(const Napi::CallbackInfo &info) { return _commonSet(info, MDBX_SET_RANGE, MDBX_GET_BOTH_RANGE); }

void MDBX_Cursor::Put(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	MDBX_val key, data;
	unsigned int flags = MDBX_put_flags::MDBX_UPSERT;

	key = Utils::argToMdbxValue(env, info[0], this->_keyBuffer);
	data = Utils::argToMdbxValue(env, info[1], this->_dataBuffer);

	if (info[2].IsObject()) {
		Napi::Object options = info[2].ToObject();

		Utils::setFromObject(&flags, MDBX_NOOVERWRITE, options, "noOverwrite");
		Utils::setFromObject(&flags, MDBX_NODUPDATA, options, "noDupData");
		Utils::setFromObject(&flags, MDBX_CURRENT, options, "current");
		Utils::setFromObject(&flags, MDBX_ALLDUPS, options, "allDups");
		Utils::setFromObject(&flags, MDBX_APPEND, options, "append");
		Utils::setFromObject(&flags, MDBX_APPENDDUP, options, "appendDup");
		Utils::setFromObject(&flags, MDBX_MULTIPLE, options, "multiple");
	}

	int rc = mdbx_cursor_put(this->cursor, &key, &data, static_cast<MDBX_put_flags>(flags));
	if (rc) {
		Utils::throwMdbxError(env, rc);
	}
}

void MDBX_Cursor::Del(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	unsigned int flags = MDBX_put_flags::MDBX_UPSERT;

	if (info[2].IsObject()) {
		Napi::Object options = info[2].ToObject();

		Utils::setFromObject(&flags, MDBX_CURRENT, options, "current");
		Utils::setFromObject(&flags, MDBX_ALLDUPS, options, "allDups");
	}

	int rc = mdbx_cursor_del(this->cursor, static_cast<MDBX_put_flags>(flags));
	if (rc) {
		Utils::throwMdbxError(env, rc);
	}
}

void MDBX_Cursor::Reset(const Napi::CallbackInfo &info) {
	int rc = mdbx_cursor_reset(this->cursor);
	if (rc) {
		Utils::throwMdbxError(info.Env(), rc);
	}
}

void MDBX_Cursor::Bind(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (this->closeTxn) {
		Napi::Error::New(env, "Cannot bind cursor without provided txn").ThrowAsJavaScriptException();
		return;
	}

	if (info[0].IsObject()) {
		try {
			this->txn = Utils::argToMdbxTxn(env, info[0]);
		} catch (const Napi::Error &e) {
			e.ThrowAsJavaScriptException();
			return;
		}
	}

	int rc;
	MDBX_cursor_op op;

	rc = mdbx_cursor_bind(this->txn, this->cursor, *this->dbi);
	if (rc) {
		Utils::throwMdbxError(env, rc);
	}

	MDBX_val key = Utils::vectorBufferToMdbxValue(this->_keyBuffer);
	MDBX_val data = Utils::vectorBufferToMdbxValue(this->_dataBuffer);

	if (this->dbiFlags & MDBX_DUPSORT) {
		op = MDBX_GET_BOTH;
	} else {
		op = MDBX_SET;
	}

	rc = mdbx_cursor_get(this->cursor, &key, &data, op);
	if (rc) {
		Utils::throwMdbxError(env, rc);
		return;
	}
}

void MDBX_Cursor::Close(const Napi::CallbackInfo &info) {
	if (this->cursor) {
		mdbx_cursor_close(this->cursor);
		this->cursor = nullptr;
	}

	if (this->closeTxn && this->txn) {
		int rc = mdbx_txn_abort(this->txn);
		if (rc) {
			Utils::throwMdbxError(info.Env(), rc);
			return;
		}

		this->txn = nullptr;
	}
}

MDBX_Cursor::~MDBX_Cursor() {
	if (this->cursor) {
		mdbx_cursor_close(this->cursor);
	}

	if (this->closeTxn && this->txn) {
		mdbx_txn_abort(this->txn);
	}
}