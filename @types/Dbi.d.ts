import { Cursor, CursorOptions, CursorValueType, ResolveCursorDataType } from './Cursor';
import { Stat } from './Stat';
import { Txn } from './Txn';

export interface DbiOptions {
    txn?: Txn;

    reverseKey?: boolean;
    dupSort?: boolean;
    integerKey?: boolean;
    dupFixed?: boolean;
    integerDup?: boolean;
    reverseDup?: boolean;
    create?: boolean;
    accede?: boolean;
}

export interface DbiDropOptions {
    txn?: Txn;
    del?: boolean;
}

export declare interface Dbi {
    stat(txn?: Txn): Stat;

    getCursor<TKey extends CursorValueType, TValue extends CursorValueType>(
        options: { keyType?: TKey; dataType?: TValue }
    ): Cursor<ResolveCursorDataType<TKey>, ResolveCursorDataType<TValue>>;
    getCursor(options?: CursorOptions): Cursor;

    drop(options?: DbiDropOptions): void;
    rename(name: string, txn?: Txn): void;
    close(): void;
}
