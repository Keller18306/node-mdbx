const { existsSync, rmSync } = require('fs');

const buildDir = './build/Debug';

if (existsSync(buildDir)) {
    rmSync(buildDir, { recursive: true });
}