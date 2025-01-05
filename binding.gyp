{
  "targets": [
    {
      "target_name": "libmdbx",
      "product_name": "libmdbx",
      "type": "shared_library",
      "sources": [ "libmdbx/mdbx.c" ],
      "include_dirs": [ "libmdbx" ],
      "defines": [
        'MDBX_BUILD_FLAGS_CONFIG="Removed by GYP"', # Disable MDBX warning: Build flags undefined. Please use correct build script
        'NDEBUG', # disable assertions
        'MDBX_ENABLE_PROFGC',
        # 'MDBX_DEBUG=1'
      ]
    },
    {
      "target_name": "node-mdbx",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "dependencies": [ "libmdbx" ],
      "sources": [
        "src/main.cc",
        "src/env.cc",
        "src/txn.cc",
        "src/dbi.cc",
        "src/cursor.cc",
        "src/version.cc",
        "src/errors.cc",
        "src/debug.cc",
        "src/utils.cc"
      ],
      "include_dirs": [
        "src",
        "libmdbx",
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "defines": [ 
        'NAPI_DISABLE_CPP_EXCEPTIONS',
      ],
      "conditions": [
        ["OS=='win'", {
          "libraries": [
            "ntdll.lib"
          ]
        }]
      ],
    },
    {
      "target_name": "mdbx_chk",
      "type": "executable",
      "dependencies": [ "libmdbx" ],
      "sources": [
        "libmdbx/mdbx_chk.c",
      ],
      "include_dirs": [ "libmdbx" ],
    },
    {
      "target_name": "mdbx_copy",
      "type": "executable",
      "dependencies": [ "libmdbx" ],
      "sources": [
        "libmdbx/mdbx_copy.c",
      ],
      "include_dirs": [ "libmdbx" ],
    },
    {
      "target_name": "mdbx_drop",
      "type": "executable",
      "dependencies": [ "libmdbx" ],
      "sources": [
        "libmdbx/mdbx_drop.c",
      ],
      "include_dirs": [ "libmdbx" ],
    },
    {
      "target_name": "mdbx_dump",
      "type": "executable",
      "dependencies": [ "libmdbx" ],
      "sources": [
        "libmdbx/mdbx_dump.c",
      ],
      "include_dirs": [ "libmdbx" ],
    },
    {
      "target_name": "mdbx_load",
      "type": "executable",
      "dependencies": [ "libmdbx" ],
      "sources": [
        "libmdbx/mdbx_load.c",
      ],
      "include_dirs": [ "libmdbx" ],
    },
    {
      "target_name": "mdbx_stat",
      "type": "executable",
      "dependencies": [ "libmdbx" ],
      "sources": [
        "libmdbx/mdbx_stat.c",
      ],
      "include_dirs": [ "libmdbx" ],
    },
  ]
}