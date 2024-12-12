const mdbx = require('../build/Debug/node-mdbx.node');

console.log('Aaaa')
const env = new mdbx.Env('/newhdd/mdbx/0-main/');

function logMemoryUsage(label) {
    const mem = process.memoryUsage();
    console.log(`Memory Usage (${label}):`);
    console.log(`  RSS: ${(mem.rss / 1024 / 1024).toFixed(2)} MB`);
    console.log(`  Heap Total: ${(mem.heapTotal / 1024 / 1024).toFixed(2)} MB`);
    console.log(`  Heap Used: ${(mem.heapUsed / 1024 / 1024).toFixed(2)} MB`);
    console.log(`  External: ${(mem.external / 1024 / 1024).toFixed(2)} MB`);
    console.log(`  Array Buffers: ${(mem.arrayBuffers / 1024 / 1024).toFixed(2)} MB`);
}

function getRandomKey() {
    const length = Math.floor(Math.random() * 10) + 5;
    return Array.from({ length }, () =>
        String.fromCharCode(97 + Math.floor(Math.random() * 26))
    ).join('');
}

const dbi = env.getDbi('email');
const test = [];

function testCursor(ic) {
    const cursor = dbi.getCursor();

    const iterations = 1000000;
    logMemoryUsage("Before test");

    for (let i = 0; i < iterations; i++) {
        // const randomKey = getRandomKey();
        const randomKey = 'test'
        let value = cursor.range(Buffer.from(randomKey));
        test.push(value);
        // const [a, b] = cursor.next();

        for (let i = 0; i <= 1000; i++) {
            value = cursor.next();
            test.push(value);

            if (i % 100 === 0) {
                console.clear();
                console.log(`Iteration ${ic}-${i}:`, value?.[0]?.toString(), value?.[1]?.toString());
                logMemoryUsage("While iteration");
            }
        }

        if (i % 10000 === 0) {
            console.clear();
            console.log(`Iteration ${ic}-${i}:`, value?.[0]?.toString(), value?.[1]?.toString());
            logMemoryUsage("While iteration");
        }
    }

    logMemoryUsage("After test");

    cursor.close();

    if (global.gc) {
        global.gc();
        logMemoryUsage("After GC");
    } else {
        console.warn("Сборщик мусора недоступен. Запустите с флагом '--expose-gc'.");
    }

    // debugger;
}

for (let i = 0; i < 10; i++) {
    testCursor(i);
} 