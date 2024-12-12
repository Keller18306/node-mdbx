export interface Version {
    major: number,
    minor: number,
    release: number,
    revision: number,
    git: {
        datetime: string | null,
        tree: string | null,
        commit: string | null,
        describe: string | null
    },
    sourcery: string | null
}