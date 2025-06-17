# Prerequisites
[Experimental LLVM compiler with C++26 Reflection support](https://github.com/bloomberg/clang-p2996/tree/p2996) should be installed.

# Build & Run Test Cases
This project uses [XMake](https://xmake.io) as build system.
```
# Optional parameters:
# --full-header-test=[yn]: Enables full header test
#   (e.g. <reflect_cpp26/module.hpp> is included instead of <reflect_cpp26/module/component.hpp>)
# --static-test=[yn]: Enables static assertion test

xmake f -m <debug|release> \
        --sdk=<llvm-root> \
        --cxxflags="-stdlib=libc++" \
        --ldflags="-stdlib=libc++" \
        --toolchain=llvm

xmake build --group=tests/**
LD_LIBRARY_PATH=<path-to-libc++> xmake run --group=tests/**
```

# TODO
* Enum functions or types not implemented (compared to [magic_enum](https://github.com/Neargye/magic_enum)):
  * `enum_fusion`
  * Functions for enum flags
  * IOStream operators
  * Bitwise operators
  * containers
  * Case-insensitive enum name comparison
* Validators
  * (see the table below)
  * Recursive validation
  * String validators with case-insensitive comparison
  * Validation on invariant relation between multiple members
  * Non-intrusive validator annotation
* Improvements to `constant`:
  * `map`, `filter` with index
  * `index_of`, `last_index_of`, `includes` with start index

Validators to be implemented:
| Name | Implemented | Tested |
| :--- | :---------: | :----: |
| `starts_with` | | |
| `ends_with` | | |
| `contains` | | |
| `matches_regex` | | |
| `substr_matches_regex` | | |
| `has_subsequence` | | |
| `is_subsequence_of` | | |
