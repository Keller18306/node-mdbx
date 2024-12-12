import { Txn } from "./Txn";

export declare type CursorValue = Buffer | string | number | bigint;
export declare type CursorValueType = 'buffer' | 'string' | 'number' | 'bigint';
export declare type StringEncoding = 'utf8' | 'utf16';
export declare type IntegerByteOrder = 'BE' | 'LE';

export declare interface CursorOptions {
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

export interface CursorPutOptions {
    noOverwrite?: boolean;
    noDupData?: boolean;
    current?: boolean;
    allDups?: boolean;
    append?: boolean;
    appendDup?: boolean;
    multiple?: boolean;
}

export interface CursorDelOptions {
    current?: boolean;
    allDups?: boolean;
}

export type DataTypeMap = {
    buffer: Buffer;
    string: string;
    number: number;
    bigint: bigint;
};

export type ResolveCursorDataType<T extends CursorValueType | undefined> =
    T extends keyof DataTypeMap ? DataTypeMap[T] : CursorValue;

export declare interface Cursor<TKey extends CursorValue = Buffer, TValue extends CursorValue = Buffer> {
    getKey(): TKey;
    getValue(): TValue;

    // info(): any;
    count(): number;
    first(): TKey;
    firstDup(): TKey;
    last(): TKey;
    lastDup(): TKey;
    next(): TKey;
    nextDup(): TKey;
    nextNoDup(): TKey;
    prev(): TKey;
    prevDup(): TKey;
    prevNoDup(): TKey;

    range(key: CursorValue, value?: CursorValue): TKey | null;

    put(key: CursorValue, value: CursorValue, options?: CursorPutOptions): void;
    del(options?: CursorDelOptions): void;

    reset(): void;
    close(): void;
}
