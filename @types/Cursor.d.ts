import '../index';

declare module '../index' {
    type CursorValue = Buffer | string | number | bigint;
    type CursorValueType = 'buffer' | 'string' | 'number' | 'bigint';
    type StringEncoding = 'utf8' | 'utf16';
    type IntegerByteOrder = 'BE' | 'LE';

    interface CursorOptions {
        txn?: Txn;
        /**
         * Key data type
         * @default buffer
         */
        keyType?: CursorValueType;

        /**
         * Value data type
         * @default buffer
         */
        dataType?: CursorValueType;

        /**
         * String encoding
         * @default utf8
         */
        stringEncoding?: StringEncoding; //default utf8

        /**
         * Default to system settings (often: LE)
         * 
         * LE:
         *  01 00 00 00,
         *  02 00 00 00,
         *  03 00 00 00
         * 
         * BE:
         *  00 00 00 01,
         *  00 00 00 02,
         *  00 00 00 03
         */
        integerByteOrder?: IntegerByteOrder;
    }

    interface CursorPutOptions {
        noOverwrite?: boolean;
        noDupData?: boolean;
        current?: boolean;
        allDups?: boolean;
        append?: boolean;
        appendDup?: boolean;
        multiple?: boolean;
    }

    interface CursorDelOptions {
        current?: boolean;
        allDups?: boolean;
    }

    type DataTypeMap = {
        buffer: Buffer;
        string: string;
        number: number;
        bigint: bigint;
    };

    type ResolveCursorDataType<T extends CursorValueType | undefined, TDefault extends CursorValue> =
        T extends keyof DataTypeMap ? DataTypeMap[T] : TDefault;

    interface Cursor<TKey extends CursorValue = Buffer, TValue extends CursorValue = Buffer> {
        getKey(): TKey | null;
        getValue(): TValue | null;

        count(): number;
        dupStat(): Stat;

        first(): TKey | null;
        firstDup(): TKey | null;
        last(): TKey | null;
        lastDup(): TKey | null;
        next(): TKey | null;
        nextDup(): TKey | null;
        nextNoDup(): TKey | null;
        prev(): TKey | null;
        prevDup(): TKey | null;
        prevNoDup(): TKey | null;

        set(key: CursorValue, value?: CursorValue): TKey | null;
        range(key: CursorValue, value?: CursorValue): TKey | null;
        pos(): number;

        put(key: CursorValue, value: CursorValue, options?: CursorPutOptions): void;
        del(options?: CursorDelOptions): void;

        reset(): void;
        bind(txn: Txn, restorePos?: boolean): void;
        close(): void;
    }
}
