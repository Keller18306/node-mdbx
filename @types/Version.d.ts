import '../index';

declare module '../index' {
    interface Version {
        major: number;
        minor: number;
        patch: number;
        tweak: number;
        semver_prerelease: string;
        git: {
            datetime: string | null;
            tree: string | null;
            commit: string | null;
            describe: string | null;
        };
        sourcery: string | null;
    }
}