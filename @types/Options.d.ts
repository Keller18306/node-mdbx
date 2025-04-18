import '../index';

declare module '../index' {
    enum Options {
        max_db,
        max_readers,
        sync_bytes,
        sync_period,
        rp_augment_limit,
        loose_limit,
        dp_reserve_limit,
        txn_dp_limit,
        txn_dp_initial,
        spill_max_denominator,
        spill_min_denominator,
        spill_parent4child_denominator,
        merge_threshold_16dot16_percent,
        writethrough_threshold,
        prefault_write_enable,
        gc_time_limit,
        prefer_waf_insteadof_balance,
        subpage_limit,
        subpage_room_threshold,
        subpage_reserve_prereq,
        subpage_reserve_limit,
    }
}