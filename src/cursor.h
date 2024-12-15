#include <mdbx.h>
#include <napi.h>
#include <vector>

#include "utils.h"

class MDBX_Cursor : public Napi::ObjectWrap<MDBX_Cursor> {
  private:
	MDBX_env *env = nullptr;
	MDBX_txn *txn = nullptr;
	bool closeTxn = false;
	MDBX_dbi *dbi = nullptr;
	MDBX_cursor *cursor = nullptr;
	unsigned int dbiFlags;

	bool hasData;
	buffer_t _keyBuffer;
	buffer_t _dataBuffer;

	ValueType keyType = ValueType_Buffer;
	ValueType dataType = ValueType_Buffer;
	bool isIntBE;
	bool isStrUtf16;

	Napi::Value GetKey(const Napi::CallbackInfo &info);
	Napi::Value GetValue(const Napi::CallbackInfo &info);

	Napi::Value Count(const Napi::CallbackInfo &info);

	Napi::Value First(const Napi::CallbackInfo &info);
	Napi::Value FirstDup(const Napi::CallbackInfo &info);
	Napi::Value Last(const Napi::CallbackInfo &info);
	Napi::Value LastDup(const Napi::CallbackInfo &info);
	Napi::Value Next(const Napi::CallbackInfo &info);
	Napi::Value NextDup(const Napi::CallbackInfo &info);
	Napi::Value NextNoDup(const Napi::CallbackInfo &info);
	Napi::Value Prev(const Napi::CallbackInfo &info);
	Napi::Value PrevDup(const Napi::CallbackInfo &info);
	Napi::Value PrevNoDup(const Napi::CallbackInfo &info);

	Napi::Value Set(const Napi::CallbackInfo &info);
	Napi::Value Range(const Napi::CallbackInfo &info);

	void Put(const Napi::CallbackInfo &info);
	void Del(const Napi::CallbackInfo &info);

	void Reset(const Napi::CallbackInfo &info);
	void Bind(const Napi::CallbackInfo &info);
	void Close(const Napi::CallbackInfo &info);

	Napi::Value _commonMove(const Napi::CallbackInfo &info, MDBX_cursor_op op, bool checkDupFlag);
	Napi::Value _commonSet(const Napi::CallbackInfo &info, MDBX_cursor_op opKeyOnly, MDBX_cursor_op opKeyValue);

  public:
	static Napi::FunctionReference constructor;

	static void Init(Napi::Env env);
	MDBX_Cursor(const Napi::CallbackInfo &info);
	~MDBX_Cursor();
};