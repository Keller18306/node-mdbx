{
  "targets": [
    {
      "target_name": "node-mdbx",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "libmdbx/mdbx.c",
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
        'NDEBUG', # disable assertions
        'MDBX_ENABLE_PROFGC',
        # 'MDBX_DEBUG=1'
      ],
      "conditions": [
        ["OS=='win'", {
          "libraries": [
            "ntdll.lib"
          ]
        }]
      ],
    }
  ]
}