# Prerequisites
[Experimental LLVM compiler with C++26 Reflection support](https://github.com/bloomberg/clang-p2996/tree/p2996) should be installed.

# Build & Run Test Cases
This project uses [XMake](https://xmake.io) as build system.
```
# Optional parameters:
# --static-test=[yn]: Enables static assertion test

xmake f -m <debug|release> \
        --sdk=<llvm-root> \
        --cxxflags="-stdlib=libc++" \
        --ldflags="-stdlib=libc++" \
        --toolchain=llvm  \
        (optional parameters see above)

xmake build --group=tests/**
LD_LIBRARY_PATH=<path-to-libc++> xmake run --group=tests/**
```

# Possibly Redundant Components
* `structural_type` can be replaced by [`std::is_structural_type_v` proposed by P3856](https://www.open-std.org/JTC1/SC22/WG21/docs/papers/2025/p3856r3.pdf) once it is adopted by C++ standard.

# TODO
* `fixed_set`
* utils:
  * `to_string` for string types other than `char`
* Missing UT for string-key fixed map (in utils):
  * Case-insensitive UT with `string_key_map_by_hash_binary_search_slow`;
  * Hash collision cases with every hash-related scenario.
* type_operations
  * Serialize to JSON or YAML
  * Deserialize from JSON or YAML
