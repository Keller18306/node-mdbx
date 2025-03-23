const { execSync } = require('child_process');
const { join } = require('path');
const { existsSync, mkdirSync, rmSync, cpSync } = require('fs');

const cwd = './.local';
const out = './libmdbx';

if (existsSync(cwd)) {
    rmSync(cwd, { recursive: true });
}

mkdirSync(cwd);

execSync("git clone --branch stable https://gitflic.ru/project/erthink/libmdbx.git .", { cwd, stdio: 'inherit' });
execSync("make dist", { cwd, stdio: 'inherit' });

if (existsSync(out)) {
    rmSync(out, { recursive: true });
    mkdirSync(out);
}

cpSync(join(cwd, 'dist'), out, { recursive: true });

rmSync(cwd, { recursive: true });