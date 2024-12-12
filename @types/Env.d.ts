import { Dbi, DbiOptions } from './Dbi'
import { Info } from './Info';
import { Stat } from './Stat';
import { Txn, TxnOptions } from './Txn';

export declare interface EnvFlags {
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

export declare interface EnvGeometry {
    sizeLower?: number;
    sizeNow?: number;
    sizeUpper?: number;
    growthStep?: number;
    shrinkThreshold?: number;
    pageSize?: number;
}

export declare interface EnvOptions {
    mode?: number;
    maxdbs?: number;
    flags?: EnvFlags;
    geometry?: EnvGeometry;
}

export declare class Env {
    constructor(path: string, options: EnvOptions);

    public info(): Info;
    public stat(): Stat;
    public getDbi(name: string | null, options?: DbiOptions): Dbi;
    public getTxn(options?: TxnOptions): Txn;
    public close(): void;
}
