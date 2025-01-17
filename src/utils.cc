#include "utils.h"

#include <iterator>

#include "txn.h"

uint32_t Utils::toBigEndian(uint32_t value) {
	return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000) | ((value << 24) & 0xFF000000);
}

uint64_t Utils::toBigEndian(uint64_t value) {
	return ((value >> 56) & 0x00000000000000FFULL) | ((value >> 40) & 0x000000000000FF00ULL) | ((value >> 24) & 0x0000000000FF0000ULL) |
		   ((value >> 8) & 0x00000000FF000000ULL) | ((value << 8) & 0x000000FF00000000ULL) | ((value << 24) & 0x0000FF0000000000ULL) |
		   ((value << 40) & 0x00FF000000000000ULL) | ((value << 56) & 0xFF00000000000000ULL);
}

void Utils::throwMdbxError(Napi::Env env, int rc) {
	Napi::Error err = Napi::Error::New(env, mdbx_strerror(rc));

	err.Set("name", "ErrorMDBX");
	err.Set("code", Napi::Number::New(env, rc));

	err.ThrowAsJavaScriptException();
}

Napi::Object Utils::mdbxStatToJsObject(Napi::Env env, const MDBX_stat stat) {
	Napi::Object obj = Napi::Object::New(env);

	obj.Set("psize", stat.ms_psize);
	obj.Set("depth", stat.ms_depth);
	obj.Set("branch_pages", stat.ms_branch_pages);
	obj.Set("leaf_pages", stat.ms_leaf_pages);
	obj.Set("overflow_pages", stat.ms_overflow_pages);
	obj.Set("entries", stat.ms_entries);
	obj.Set("mod_txnid", stat.ms_mod_txnid);

	return obj;
}

Napi::Value Utils::mdbxCommitLatencyToJSObject(const Napi::Env &env, const MDBX_commit_latency latency) {
	Napi::Object obj = Napi::Object::New(env);

	obj.Set("preparation", latency.preparation);
	obj.Set("gc_wallclock", latency.gc_wallclock);
	obj.Set("audit", latency.audit);
	obj.Set("write", latency.write);
	obj.Set("sync", latency.sync);
	obj.Set("ending", latency.ending);
	obj.Set("whole", latency.whole);
	obj.Set("gc_cputime", latency.gc_cputime);

	Napi::Object gcProf = Napi::Object::New(env);

	gcProf.Set("wloops", latency.gc_prof.wloops);
	gcProf.Set("coalescences", latency.gc_prof.coalescences);
	gcProf.Set("wipes", latency.gc_prof.wipes);
	gcProf.Set("flushes", latency.gc_prof.flushes);
	gcProf.Set("kicks", latency.gc_prof.kicks);

	Napi::Object work = Napi::Object::New(env);
	work.Set("counter", latency.gc_prof.work_counter);
	work.Set("rtime_monotonic", latency.gc_prof.work_rtime_monotonic);
	work.Set("xtime_cpu", latency.gc_prof.work_xtime_cpu);
	work.Set("rsteps", latency.gc_prof.work_rsteps);
	work.Set("xpages", latency.gc_prof.work_xpages);
	work.Set("majflt", latency.gc_prof.work_majflt);
	gcProf.Set("work", work);

	Napi::Object self = Napi::Object::New(env);
	self.Set("counter", latency.gc_prof.self_counter);
	self.Set("rtime_monotonic", latency.gc_prof.self_rtime_monotonic);
	self.Set("xtime_cpu", latency.gc_prof.self_xtime_cpu);
	self.Set("rsteps", latency.gc_prof.self_rsteps);
	self.Set("xpages", latency.gc_prof.self_xpages);
	self.Set("majflt", latency.gc_prof.self_majflt);
	gcProf.Set("self", self);

	Napi::Object pnlMergeWork = Napi::Object::New(env);
	pnlMergeWork.Set("time", latency.gc_prof.pnl_merge_work.time);
	pnlMergeWork.Set("volume", latency.gc_prof.pnl_merge_work.volume);
	pnlMergeWork.Set("calls", latency.gc_prof.pnl_merge_work.calls);
	gcProf.Set("pnl_merge_work", pnlMergeWork);

	Napi::Object pnlMergeSelf = Napi::Object::New(env);
	pnlMergeSelf.Set("time", latency.gc_prof.pnl_merge_self.time);
	pnlMergeSelf.Set("volume", latency.gc_prof.pnl_merge_self.volume);
	pnlMergeSelf.Set("calls", latency.gc_prof.pnl_merge_self.calls);
	gcProf.Set("pnl_merge_self", pnlMergeSelf);

	obj.Set("gc_prof", gcProf);

	return obj;
}

Napi::Value Utils::Error(Napi::Env env, const char *message) {
	Napi::TypeError::New(env, message).ThrowAsJavaScriptException();
	return env.Undefined();
}

MDBX_val Utils::vectorBufferToMdbxValue(const buffer_t &buffer) {
	MDBX_val value;

	value.iov_len = buffer.size();
	value.iov_base = (void *)buffer.data();

	return value;
}

void Utils::mdbxValueToVectorBuffer(buffer_t &buffer, MDBX_val value) {
	const size_t size = value.iov_len;
	const char *data = static_cast<const char *>(value.iov_base);

	buffer.clear();
	buffer.reserve(size);

	std::copy(data, data + size, std::back_inserter(buffer));
}

MDBX_val Utils::argToMdbxValue(Napi::Value arg, buffer_t &_buffer) {
	if (arg.IsBuffer()) {
		Napi::Buffer<char> buffer = arg.As<Napi::Buffer<char>>();

		const size_t size = buffer.Length();
		const char *data = buffer.Data();

		_buffer.clear();
		_buffer.reserve(size);

		std::copy(data, data + size, std::back_inserter(_buffer));
	} else if (arg.IsString()) {
		// TODO CHECK FOR UTF8
		Napi::String string = arg.ToString();

		std::string data = string.Utf8Value();

		_buffer.clear();
		_buffer.reserve(data.size());

		std::copy(data.begin(), data.end(), std::back_inserter(_buffer));

		// std::u16string data = string.Utf16Value();
		// Napi::Buffer<char16_t> buffer = Napi::Buffer<char16_t>::Copy(env, data.data(), data.size());

		// value.iov_len = buffer.Length() * sizeof(char16_t);
		// value.iov_base = buffer.Data();
	} else if (arg.IsNumber()) {
		// TODO CHECK FOR BE/LE
		Napi::Number number = arg.As<Napi::Number>();
		uint32_t data = toBigEndian(number.Uint32Value());

		_buffer.clear();
		_buffer.reserve(sizeof(data));

		char *dataPtr = reinterpret_cast<char *>(&data);
		std::copy(dataPtr, dataPtr + sizeof(data), std::back_inserter(_buffer));
	} else if (arg.IsBigInt()) {
		Napi::BigInt bigint = arg.As<Napi::BigInt>();

		bool lossless;
		uint64_t data = bigint.Uint64Value(&lossless);
		if (!lossless) {
			throw Napi::RangeError::New(arg.Env(), "BigInt is too large for int64");
		}

		char *dataPtr = reinterpret_cast<char *>(&data);
		std::copy(dataPtr, dataPtr + sizeof(data), std::back_inserter(_buffer));
	} else {
		throw Napi::TypeError::New(arg.Env(), "Unknown data type");
	}

	return vectorBufferToMdbxValue(_buffer);
}

Napi::Value Utils::vectorBufferToArg(Napi::Env env, ValueType type, bool intToBE, const buffer_t &buffer) {
	const size_t size = buffer.size();
	const char *data = buffer.data();

	switch (type) {

	case ValueType_Buffer:
		return Napi::Buffer<char>::Copy(env, data, size);

	case ValueType_String: {
		std::string str(data, size);

		return Napi::String::New(env, str);
	}

	case ValueType_Number: {
		if (size != sizeof(uint32_t)) {
			throw Napi::Error::New(env, "Data size invalid for uint32_t");
		}

		uint32_t value = *reinterpret_cast<const uint32_t *>(data);
		if (intToBE) {
			value = Utils::toBigEndian(value);
		}

		return Napi::Number::New(env, value);
	}

	case ValueType_BigInt: {
		if (size != sizeof(uint64_t)) {
			throw Napi::Error::New(env, "Data size invalid for uint64_t");
		}

		uint64_t value = *reinterpret_cast<const uint64_t *>(data);
		if (intToBE) {
			value = Utils::toBigEndian(value);
		}

		return Napi::BigInt::New(env, value);
	}
	}

	throw std::runtime_error("Unsupported data type");
}

void Utils::setFromObject(unsigned int *flagsSetter, int flag, Napi::Object flagsObj, const char *objValue) {
	Napi::Value value = flagsObj.Get(objValue);

	if (value.IsBoolean() && value.ToBoolean().Value()) {
		*flagsSetter |= flag;
	}
}

void Utils::setFromObject(int *flagsSetter, int flag, Napi::Object flagsObj, const char *objValue) {
	Napi::Value value = flagsObj.Get(objValue);

	if (value.IsBoolean() && value.ToBoolean().Value()) {
		*flagsSetter |= flag;
	}
}

void Utils::setFromObject(intptr_t *longSetter, Napi::Object flagsObj, const char *objValue, long defaultValue) {
	Napi::Value value = flagsObj.Get(objValue);

	if (value.IsNumber()) {
		*longSetter = value.As<Napi::Number>().Int64Value();
	} else {
		*longSetter = defaultValue;
	}
}

void Utils::stringToLower(std::string &str) {
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
}

MDBX_txn *Utils::argToMdbxTxn(Napi::Env env, Napi::Value arg) {
	Napi::Object txnObj = arg.ToObject();
	EnvInstanceData *instanceData = Utils::envInstanceData(env);

	if (txnObj.InstanceOf(instanceData->txn->Value())) {
		MDBX_Txn *txnClass = Napi::ObjectWrap<MDBX_Txn>::Unwrap(txnObj);

		return txnClass->txn;
	} else {
		throw Napi::TypeError::New(env, "Invalid Txn: not an instance of Txn");
		// .ThrowAsJavaScriptException();
	}
}

EnvInstanceData *Utils::envInstanceData(Napi::Env env) {
	EnvInstanceData *instanceData = env.GetInstanceData<EnvInstanceData>();

	if (!instanceData) {
		instanceData = new EnvInstanceData();
		env.SetInstanceData<EnvInstanceData>(instanceData);
	}

	return instanceData;
}