export interface SetupDebugOption {
    dump?: boolean;
    legacyMultiopen?: boolean;
    legacyOverlap?: boolean;
    dontUpgrade?: boolean;
}

export function setup_debug(options: SetupDebugOption): void;