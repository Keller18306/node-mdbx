export interface TxnOptions {
    parentTxn?: Txn;
    readOnly?: boolean;
    prepare?: boolean;
    try?: boolean;
    noMetaSync?: boolean;
    noSync?: boolean;
}

export interface TxnInfo {
    id: number;
    reader_lag: number;
    space_used: number;
    space_limit_soft: number;
    space_limit_hard: number;
    space_retired: number;
    space_leftover: number;
    space_dirty: number;
}

export declare interface Txn {
    info(): TxnInfo;
    commit(): void;
    abort(): void;
    park(autounpark: boolean): void;
    unpark(restartIfOusted: boolean): void;
    reset(): void;
    renew(): void;
}
