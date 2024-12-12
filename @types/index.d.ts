import { Build } from './Build';
import { Version } from './Version';

export const VERSION_STRING: string;
export const version: Version;
export const build: Build;

export * from './Build';
export * from './Cursor';
export * from './Dbi';
export * from './Env';
export * from './Info';
export * from './Stat';
export * from './Txn';
export * from './Version';

export as namespace MDBX;