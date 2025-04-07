import '../index';

declare module '../index' {
    interface Info {
        geo: {
            lower: number,
            upper: number,
            current: number,
            shrink: number,
            grow: number
        },
        mapsize: number,
        last_pgno: number,
        recent_txnid: number,
        latter_reader_txnid: number,
        self_latter_reader_txnid: number,
        meta_txnid: [number, number, number],
        meta_sign: [bigint, bigint, bigint],
        maxreaders: number,
        numreaders: number,
        dxb_pagesize: number,
        sys_pagesize: number,
        bootid: {
            current: { x: bigint, y: bigint },
            meta: [
                { x: bigint, y: bigint },
                { x: bigint, y: bigint },
                { x: bigint, y: bigint }
            ]
        },
        unsync_volume: number,
        autosync_threshold: number,
        since_sync_seconds16dot16: number,
        autosync_period_seconds16dot16: number,
        since_reader_check_seconds16dot16: number,
        mode: number,
        pgop_stat: {
            newly: number,
            cow: number,
            clone: number,
            split: number,
            merge: number,
            spill: number,
            unspill: number,
            wops: number,
            prefault: number,
            mincore: number,
            msync: number,
            fsync: number
        },
        dxbid: { x: bigint, y: bigint }
    }
}