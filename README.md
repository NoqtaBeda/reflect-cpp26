# Prerequisites

- Recommended: GCC Trunk
- Legacy: [Experimental LLVM fork with C++26 Reflection support](https://github.com/bloomberg/clang-p2996/tree/p2996)

# Build & Run Test Cases

This project uses [XMake](https://xmake.io) as build system.

```
# Optional parameters:
# --static-test=[yn]: Enables static assertion test

# Using GCC 16
xmake f -m <debug|release> \
        --sdk=<gcc-root> \
        --cxxflags="-freflection" \
        --toolchain=gcc \
        (optional parameters see above)

# Using experimental LLVM fork with C++26 Reflection support
xmake f -m <debug|release> \
        --sdk=<llvm-root> \
        --cxxflags="-stdlib=libc++" \
        --ldflags="-stdlib=libc++" \
        --toolchain=llvm  \
        (optional parameters see above)

# Build and run
xmake run --group=tests/**
# Using experimental LLVM: Build and run with libc++
LD_LIBRARY_PATH=<path-to-libc++> xmake run --group=tests/**
```

# TODO

- Replace `structural_type` to the standard [`std::is_structural_type_v`](https://www.open-std.org/JTC1/SC22/WG21/docs/papers/2025/p3856r3.pdf) once it is implemented by the compiler.
- `fixed_set`
- utils:
  - `append_*_unsafe` and `reserve_and_append` for string builder
- enum:
  - `enum_flags_name_to` with string builder
- lookup:
  - Complete redesign
- type_operations
  - Serialize to YAML
  - Deserialize from JSON or YAML
  - Performance fix to JSON serializer & dumper
