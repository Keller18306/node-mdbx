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

    getCursor<TKey extends CursorValueType | undefined = undefined, TValue extends CursorValueType | undefined = undefined>(
        options: CursorOptions & { keyType?: TKey; dataType?: TValue }
    ): Cursor<
        ResolveCursorDataType<TKey, Buffer>,
        ResolveCursorDataType<TValue, Buffer>
    >;

    drop(options?: DbiDropOptions): void;
    rename(name: string, txn?: Txn): void;
    close(): void;
}
