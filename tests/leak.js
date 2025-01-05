const mdbx = require('../index');

mdbx.setup_debug({
    logger: false
})

console.log('start')
const env = new mdbx.Env({
    path: 'db/',
    geometry: {
        sizeNow: 4 * 1024 * 1024 * 1024,
        sizeLower: 4 * 1024 * 1024 * 1024,
        sizeUpper: 4 * 1024 * 1024 * 1024
    }
});

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



const dbi = env.getDbi(null, {
    create: true,
    dupSort: true
});

function testCursorWrite1(ic) {
    const txn = env.getTxn({});
    const cursor = dbi.getCursor({ txn });

    logMemoryUsage("Before test - write1");

    const iterations = 30_000;

    for (let i = 0; i < iterations; i++) {
        const randomKeyA = getRandomKey();
        const randomKeyB = getRandomKey();

        cursor.put(Buffer.from(randomKeyA), Buffer.from(randomKeyB));

        if (i % 1_000 === 0) {
            // console.clear();
            console.log(`Iteration ${ic}-${i}:`, randomKeyA, randomKeyB);
            logMemoryUsage("While iteration - write1");
        }
    }

    logMemoryUsage("After test - write1");

    cursor.close();
    // console.log(txn.commitWithLatency());
    txn.commit();

    if (global.gc) {
        global.gc();
        logMemoryUsage("After GC");
    } else {
        console.warn("Сборщик мусора недоступен. Запустите с флагом '--expose-gc'.");
    }

    // debugger;
}

function testCursorWrite2(ic) {
    const cursor = dbi.getCursor();

    logMemoryUsage("Before test - write2");

    const iterations = 30_000;

    for (let i = 0; i < iterations; i++) {
        const randomKeyA = getRandomKey();
        const randomKeyB = getRandomKey();

        cursor.put(Buffer.from(randomKeyA), Buffer.from(randomKeyB));

        if (i % 1_000 === 0) {
            // console.clear();
            console.log(`Iteration ${ic}-${i}:`, randomKeyA, randomKeyB);
            logMemoryUsage("While iteration - write2");
        }
    }

    logMemoryUsage("After test - write2");

    cursor.close();

    if (global.gc) {
        global.gc();
        logMemoryUsage("After GC");
    } else {
        console.warn("Сборщик мусора недоступен. Запустите с флагом '--expose-gc'.");
    }

    // debugger;
}

function testCursorRead(ic) {
    const cursor = dbi.getCursor();

    logMemoryUsage("Before test - read");

    const iterations = 10_000;

    for (let i = 0; i < iterations; i++) {
        const randomKey = getRandomKey();

        cursor.range(Buffer.from(randomKey));
        // const [a, b] = cursor.next();

        for (let j = 0; j <= 10; j++) {
            cursor.next();

            // if (j % 100 === 0) {
            //     console.clear();
            //     console.log(`Iteration ${ic}-${i}-${j}:`, value?.[0]?.toString(), value?.[1]?.toString());
            //     logMemoryUsage("While iteration (read)");
            // }
        }

        if (i % 1_000 === 0) {
            // console.clear();
            console.log(`Iteration ${ic}-${i}:`, cursor.getKey()?.toString(), cursor.getValue()?.toString());
            logMemoryUsage("While iteration - read");
        }
    }

    logMemoryUsage("After test - read");

    cursor.close();

    if (global.gc) {
        global.gc();
        logMemoryUsage("After GC - read");
    } else {
        console.warn("Сборщик мусора недоступен. Запустите с флагом '--expose-gc'.");
    }

    // debugger;
}

function testTxnWriteAbort(ic) {
    const txn = env.getTxn({});
    const cursor = dbi.getCursor({ txn });

    logMemoryUsage("Before test - testTxnWriteAbort");

    for (let i = 0; i < 10; i++) {
        const randomKeyA = getRandomKey();
        const randomKeyB = getRandomKey();

        for (let j = 0; j < 1000; j++) {
            cursor.put(Buffer.from(randomKeyA), Buffer.from(randomKeyB));
        }

        console.log(`Iteration ${ic}-${i}:`, randomKeyA, randomKeyB);
        logMemoryUsage("While iteration - testTxnWriteAbort");
    }

    logMemoryUsage("After test - write1");

    cursor.close();
    txn.abort()

    if (global.gc) {
        global.gc();
        logMemoryUsage("After GC");
    } else {
        console.warn("Сборщик мусора недоступен. Запустите с флагом '--expose-gc'.");
    }
}

function testTxnRead(ic) {
    const txn = env.getTxn({ readOnly: true });
    const cursor = dbi.getCursor({ txn });

    logMemoryUsage("Before test - testTxnWriteAbort");

    for (let i = 0; i < 5; i++) {
        const randomKeyA = getRandomKey();

        cursor.range(Buffer.from(randomKeyA));

        for (let j = 0; j < 1000; j++) {
            cursor.next();
        }

        console.log(`Iteration ${ic}-${i}:`, randomKeyA);
        logMemoryUsage("While iteration - testTxnWriteAbort");
    }

    for (let i = 0; i < 5; i++) {
        const randomKeyA = getRandomKey();
        const randomKeyB = getRandomKey();

        const val = cursor.range(Buffer.from(randomKeyA), Buffer.from(randomKeyB));
        if (!val) {
            continue;
        }

        for (let j = 0; j < 1000; j++) {
            cursor.next();
        }

        console.log(`Iteration ${ic}-${i}:`, randomKeyA, randomKeyB);
        logMemoryUsage("While iteration - testTxnWriteAbort");
    }

    logMemoryUsage("After test - write1");

    cursor.close();
    txn.commit();

    if (global.gc) {
        global.gc();
        logMemoryUsage("After GC");
    } else {
        console.warn("Сборщик мусора недоступен. Запустите с флагом '--expose-gc'.");
    }
}

for (let i = 0; i < 10; i++) {
    testCursorWrite1(i);
    testCursorWrite2(i);
    testCursorRead(i);
    testTxnWriteAbort(i)
    testTxnRead(i);
}