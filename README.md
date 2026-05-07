# Prerequisites

- GCC 16.1 or later

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

# Build and run
xmake run --group=tests/**
```

# TODO

- Fix `structural_type` implementation
- `fixed_set`
- lookup:
  - Complete redesign
- type_operations
  - Serialize to YAML

`structural_type` can be replaced by the standard [`std::is_structural_type_v`](https://www.open-std.org/JTC1/SC22/WG21/docs/papers/2025/p3856r8.pdf) yet it's not supported by GCC 16.1. Our own `structural_type` implementation shall be preserved:

```cpp
#if __cpp_lib_is_structural >= 2026XXL
template <class T>
concept structural_type = std::is_structural_type_v<T>;
#else
// Our own implementation
#endif
```
