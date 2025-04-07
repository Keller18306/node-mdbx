import '../index';

declare module '../index' {
    interface TxnOptions {
        parentTxn?: Txn;
        readOnly?: boolean;
        prepare?: boolean;
        try?: boolean;
        noMetaSync?: boolean;
        noSync?: boolean;
    }

    interface TxnInfo {
        id: number;
        reader_lag: number;
        space_used: number;
        space_limit_soft: number;
        space_limit_hard: number;
        space_retired: number;
        space_leftover: number;
        space_dirty: number;
    }

    interface GcProfilingInfo {
        wloops: number;
        coalescences: number;
        wipes: number;
        flushes: number;
        kicks: number;

        work: {
            counter: number;
            rtime_monotonic: number;
            xtime_cpu: number;
            rsteps: number;
            xpages: number;
            majflt: number;
        };

        self: {
            counter: number;
            rtime_monotonic: number;
            xtime_cpu: number;
            rsteps: number;
            xpages: number;
            majflt: number;
        };

        pnl_merge_work: {
            time: number;
            volume: number;
            calls: number;
        };

        pnl_merge_self: {
            time: number;
            volume: number;
            calls: number;
        };
    }

    interface CommitLatency {
        preparation: number;
        gc_wallclock: number;
        audit: number;
        write: number;
        sync: number;
        ending: number;
        whole: number;
        gc_cputime: number;

        gc_prof: GcProfilingInfo;
    }

    interface Txn {
        info(): TxnInfo;
        commit(): void;
        commitWithLatency(): CommitLatency;
        abort(): void;
        park(autounpark: boolean): void;
        unpark(restartIfOusted: boolean): void;
        reset(): void;
        renew(): void;
    }
}