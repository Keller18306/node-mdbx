#include "version.h"

#include <sstream>

Napi::Object ConvertMDBXVersionToJS(const Napi::Env &env) {
	Napi::Object versionInfo = Napi::Object::New(env);

	versionInfo.Set("major", Napi::Number::New(env, mdbx_version.major));
	versionInfo.Set("minor", Napi::Number::New(env, mdbx_version.minor));
	versionInfo.Set("patch", Napi::Number::New(env, mdbx_version.patch));
	versionInfo.Set("tweak", Napi::Number::New(env, mdbx_version.tweak));
	versionInfo.Set("semver_prerelease", Napi::String::New(env, mdbx_version.semver_prerelease));

	Napi::Object gitInfo = Napi::Object::New(env);
	gitInfo.Set("datetime", mdbx_version.git.datetime ? Napi::String::New(env, mdbx_version.git.datetime) : env.Null());
	gitInfo.Set("tree", mdbx_version.git.tree ? Napi::String::New(env, mdbx_version.git.tree) : env.Null());
	gitInfo.Set("commit", mdbx_version.git.commit ? Napi::String::New(env, mdbx_version.git.commit) : env.Null());
	gitInfo.Set("describe", mdbx_version.git.describe ? Napi::String::New(env, mdbx_version.git.describe) : env.Null());

	versionInfo.Set("git", gitInfo);

	versionInfo.Set("sourcery", mdbx_version.sourcery ? Napi::String::New(env, mdbx_version.sourcery) : env.Null());

	return versionInfo;
}

Napi::Object ConvertMDBXBuildToJS(const Napi::Env &env) {
	Napi::Object buildInfo = Napi::Object::New(env);

	buildInfo.Set("datetime", mdbx_build.datetime ? Napi::String::New(env, mdbx_build.datetime) : env.Null());
	buildInfo.Set("target", mdbx_build.target ? Napi::String::New(env, mdbx_build.target) : env.Null());
	buildInfo.Set("options", mdbx_build.options ? Napi::String::New(env, mdbx_build.options) : env.Null());
	buildInfo.Set("compiler", mdbx_build.compiler ? Napi::String::New(env, mdbx_build.compiler) : env.Null());
	// buildInfo.Set("flags", mdbx_build.flags ? Napi::String::New(env, mdbx_build.flags) : env.Null());
	buildInfo.Set("metadata", mdbx_build.metadata ? Napi::String::New(env, mdbx_build.metadata) : env.Null());

	return buildInfo;
}

Napi::String GetVersionString(const Napi::Env &env) {
	std::ostringstream versionStream;
	versionStream << static_cast<int>(mdbx_version.major) << "." << static_cast<int>(mdbx_version.minor) << "."
				  << static_cast<int>(mdbx_version.patch) << "." << mdbx_version.tweak;

	return Napi::String::New(env, versionStream.str());
}

void MDBX_Version::Init(Napi::Env env, Napi::Object exports) {
	exports.Set("VERSION_STRING", GetVersionString(env));
	exports.Set("version", ConvertMDBXVersionToJS(env));
	exports.Set("build", ConvertMDBXBuildToJS(env));
}