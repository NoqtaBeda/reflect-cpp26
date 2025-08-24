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
        --toolchain=llvm  \
        (optional parameters see above)

xmake build --group=tests/**
LD_LIBRARY_PATH=<path-to-libc++> xmake run --group=tests/**

# ASM check to test whether rudundant symbols exist in the build target
LD_LIBRARY_PATH=<path-to-libc++> xmake run run_asm_check
```

# TODO
* Validators: Complete redesign
* Type traits
  * Recursively flattenable
* Type operations
  * `member_named<fixed_string>`
* Improvements to `constant`:
  * `map`, `filter` with index
  * `index_of`, `last_index_of`, `includes` with start index
* utils:
  * `fixed_set`
  * Fixed string
  * `to_string` for string types other than `char`
  * String hash with different `p` for `char16_t` and `char32_t`
* Missing UT for string-key fixed map (in utils):
  * Case-insensitive UT with `string_key_map_by_hash_binary_search_slow`;
  * Hash collision cases with every hash-related scenario;
  * Wide character cases with every hash-related scenario.
    * the underlying hash function shall be redesigned for wide char types.
