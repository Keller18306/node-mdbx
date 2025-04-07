import '../index';

declare module '../index' {
    function setup_debug(options: SetupDebugOption): void;

    interface SetupDebugOption {
        dump?: boolean;
        legacyMultiopen?: boolean;
        legacyOverlap?: boolean;
        dontUpgrade?: boolean;
    }
}