import '../index';

declare module '../index' {
    interface Build {
        datetime: string | null;
        target: string | null;
        options: string | null;
        compiler: string | null;
        metadata: string | null;
    }
}