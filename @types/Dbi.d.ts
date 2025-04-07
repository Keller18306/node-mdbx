import '../index';

declare module '../index' {
    interface DbiOptions {
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

    interface DbiDropOptions {
        txn?: Txn;
        del?: boolean;
    }

    interface Dbi {
        stat(txn?: Txn): Stat;

        getCursor<TKey extends CursorValueType | undefined = undefined, TValue extends CursorValueType | undefined = undefined>(
            options?: CursorOptions & { keyType?: TKey; dataType?: TValue }
        ): Cursor<
            ResolveCursorDataType<TKey, Buffer>,
            ResolveCursorDataType<TValue, Buffer>
        >;

        minKeySize(): number;
        maxKeySize(): number;

        minValSize(): number;
        maxValSize(): number;

        drop(options?: DbiDropOptions): void;
        rename(name: string, txn?: Txn): void;
        close(): void;
    }
}