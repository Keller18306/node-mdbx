#include "options.h"

void MDBX_Options::Init(Napi::Env env, Napi::Object exports) {
	Napi::Object opt = Napi::Object::New(env);

	opt.Set("max_db", static_cast<int>(MDBX_opt_max_db));
	opt.Set("max_readers", static_cast<int>(MDBX_opt_max_readers));
	opt.Set("sync_bytes", static_cast<int>(MDBX_opt_sync_bytes));
	opt.Set("sync_period", static_cast<int>(MDBX_opt_sync_period));
	opt.Set("rp_augment_limit", static_cast<int>(MDBX_opt_rp_augment_limit));
	opt.Set("loose_limit", static_cast<int>(MDBX_opt_loose_limit));
	opt.Set("dp_reserve_limit", static_cast<int>(MDBX_opt_dp_reserve_limit));
	opt.Set("txn_dp_limit", static_cast<int>(MDBX_opt_txn_dp_limit));
	opt.Set("txn_dp_initial", static_cast<int>(MDBX_opt_txn_dp_initial));
	opt.Set("spill_max_denominator", static_cast<int>(MDBX_opt_spill_max_denominator));
	opt.Set("spill_min_denominator", static_cast<int>(MDBX_opt_spill_min_denominator));
	opt.Set("spill_parent4child_denominator", static_cast<int>(MDBX_opt_spill_parent4child_denominator));
	opt.Set("merge_threshold_16dot16_percent", static_cast<int>(MDBX_opt_merge_threshold_16dot16_percent));
	opt.Set("writethrough_threshold", static_cast<int>(MDBX_opt_writethrough_threshold));
	opt.Set("prefault_write_enable", static_cast<int>(MDBX_opt_prefault_write_enable));
	opt.Set("gc_time_limit", static_cast<int>(MDBX_opt_gc_time_limit));
	opt.Set("prefer_waf_insteadof_balance", static_cast<int>(MDBX_opt_prefer_waf_insteadof_balance));
	opt.Set("subpage_limit", static_cast<int>(MDBX_opt_subpage_limit));
	opt.Set("subpage_room_threshold", static_cast<int>(MDBX_opt_subpage_room_threshold));
	opt.Set("subpage_reserve_prereq", static_cast<int>(MDBX_opt_subpage_reserve_prereq));
	opt.Set("subpage_reserve_limit", static_cast<int>(MDBX_opt_subpage_reserve_limit));

	exports.Set("Options", opt);
}