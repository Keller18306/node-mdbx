const { execSync } = require('child_process');
const { join } = require('path');
const { existsSync, mkdirSync, rmSync, cpSync } = require('fs');

const baseDir = './.local';
let cwd = baseDir;
const out = './libmdbx';

if (existsSync(cwd)) {
    rmSync(cwd, { recursive: true });
}

mkdirSync(cwd);

execSync("git clone --depth 1 --branch stable https://gitflic.ru/project/erthink/libmdbx.git .", { cwd, stdio: 'inherit' });

if (!existsSync(join(cwd, 'VERSION.json'))) {
    execSync("make dist", { cwd, stdio: 'inherit' });
    cwd = join(cwd, 'dist');
}

const patches = [];

if (patches.length) {
    console.log('Applying patches...')
    for (const patch of patches) {
        console.log('PATCH:', patch);
        execSync(`git apply ../patches/${patch}`, { cwd, stdio: 'inherit' });
    }
}

if (existsSync(out)) {
    rmSync(out, { recursive: true });
    mkdirSync(out);
}

cpSync(cwd, out, { recursive: true });

rmSync(baseDir, { recursive: true });