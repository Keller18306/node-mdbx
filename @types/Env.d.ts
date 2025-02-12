import { CursorValue } from './Cursor';
import { Dbi, DbiOptions } from './Dbi';
import { Info } from './Info';
import { Options } from './Options';
import { Stat } from './Stat';
import { Txn, TxnOptions } from './Txn';

export interface EnvFlags {
    validation?: boolean;
    noSubdir?: boolean;
    readOnly?: boolean;
    exclusive?: boolean;
    accede?: boolean;
    writemap?: boolean;
    noStickyThreads?: boolean;
    noReadAhead?: boolean;
    noMemInit?: boolean;
    lifoReclaim?: boolean;
    noMetaSync?: boolean; // on utterly enabled by default
    syncMode?: 'durable' | 'safe' | 'utterly';
}

export interface EnvGeometry {
    sizeLower?: number;
    sizeNow?: number;
    sizeUpper?: number;
    growthStep?: number;
    shrinkThreshold?: number;
    pageSize?: number;
}

export interface EnvOptions {
    path: string;
    mode?: number;
    maxDbs?: number;
    maxReaders?: number;
    flags?: EnvFlags;
    geometry?: EnvGeometry;
}

export interface GcInfo {
    pages: number;
    reclaimable: number;
}

export interface ReaderInfo {
    num: number;
    slot: number;
    pid: number;
    thread: number;
    txnid: number;
    lag: number;
    bytes_used: number;
    bytes_retained: number;
}

export interface SyncOptions {
    force?: boolean;
    nonblock?: boolean;
}

export interface CopyOptions {
    compact?: boolean;
    forceDynamicSize?: boolean;
    dontFlush?: boolean;
    throttleMvcc?: boolean;
}

export declare class Env {
    constructor(options: EnvOptions);

    public info(): Info;
    public stat(): Stat;
    public getDbi(name: CursorValue | null, options?: DbiOptions): Dbi;
    public getTxn(options?: TxnOptions): Txn;
    public gcInfo(txn: Txn): GcInfo;
    public readers(): ReaderInfo[];
    public getOption(option: Options): bigint;
    public setOption(option: Options, value: number | bigint): void;
    /**
     * 
     * @returns boolean: true if data synced, false if not data to sync
     */
    public sync(options?: SyncOptions): boolean;
    public copy(dest: string, options?: CopyOptions): void;
    public close(): void;
}
