import '../index';

declare module '../index' {
    interface Stat {
        psize: number,
        depth: number,
        branch_pages: number,
        leaf_pages: number,
        overflow_pages: number,
        entries: number,
        mod_txnid: number
    }
}