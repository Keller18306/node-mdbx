#include <mdbx.h>
#include <napi.h>

typedef std::vector<char> buffer_t;

enum ValueType {
	ValueType_Buffer,
	ValueType_String,
	ValueType_Number,
	ValueType_BigInt,
};

struct EnvInstanceData {
	Napi::FunctionReference *dbi;
	Napi::FunctionReference *txn;
	Napi::FunctionReference *cursor;
};

class Utils {
  public:
	static uint32_t toBigEndian(uint32_t value);
	static uint64_t toBigEndian(uint64_t value);
	static void throwMdbxError(Napi::Env env, int rc);
	static Napi::Object mdbxStatToJsObject(Napi::Env env, MDBX_stat stat);
	static Napi::Value Error(Napi::Env env, const char *message);
	static MDBX_val vectorBufferToMdbxValue(const buffer_t &buffer);
	static Napi::Value vectorBufferToArg(Napi::Env env, ValueType type, bool intToBE, const buffer_t &buffer);
	static void mdbxValueToVectorBuffer(buffer_t &buffer, MDBX_val value);
	static MDBX_val argToMdbxValue(Napi::Env env, Napi::Value arg, buffer_t &_buffer);
	static void setFromObject(unsigned int *flagsSetter, int flag, Napi::Object flagsObj, const char *objValue);
	static void setFromObject(long *longSetter, Napi::Object flagsObj, const char *objValue, long defaultValue);
	static void stringToLower(std::string &str);
	static MDBX_txn *argToMdbxTxn(Napi::Env env, Napi::Value arg);
	static EnvInstanceData *envInstanceData(Napi::Env env);
};