-- Copyright (c) 2025 NoqtaBeda (noqtabeda@163.com)
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
-- SOFTWARE.

add_rules("mode.debug", "mode.release")

add_requires("gtest", {
    config = { main = true }
})

-- Uses static assertion for supported unit test cases
option("static-test")
  set_default(false)
  set_showmenu(true)
  add_defines("ENABLE_STATIC_TEST")

option("full-header-test")
  set_default(false)
  set_showmenu(true)
  add_defines("ENABLE_FULL_HEADER_TEST")

function make_test_case(path, suffix, callback)
  local _, last_slash_index = string.find(path, ".*/")
  local case_group = string.sub(path, 1, last_slash_index - 1)
  local case_target_name = string.gsub(path, "/", "-") .. suffix
  local case_src_file_path = path .. ".cpp"

  target(case_target_name, function ()
    set_kind("binary")
    set_group(case_group)
    add_files(case_src_file_path)
    set_languages("c++26")
    add_includedirs("include", ".")
    add_cxxflags("-freflection-latest")
    add_packages("gtest")
    add_options("static-test")

    if (callback ~= nil) then
      callback()
    end
  end)
end

-- Each group contains a variant with no suffix or defs implicitly.
meta_test_cases_with_custom_variants = {
  {
    path = "tests/enum/test_enum_name",
    variants = {
      { suffix = "_binary_search", defs = { "ENABLE_BINARY_SEARCH_CHECK" } }
    }
  },
  {
    path = "tests/enum/test_enum_cast_from_string",
    variants = {
      { suffix = "_binary_search", defs = { "ENABLE_BINARY_SEARCH_CHECK" } },
      { suffix = "_linear_search", defs = { "ENABLE_LINEAR_SEARCH_CHECK" } }
    }
  }
}

meta_test_cases_without_variant = {
  -- Utility
  "tests/utils/test_constant",
  "tests/utils/test_identifier_naming",
  "tests/utils/test_meta_tuple",
  "tests/utils/test_preprocessors",
  "tests/utils/test_to_string",
  "tests/utils/test_utils_misc",
  -- Type Traits
  "tests/type_traits/test_function_types",
  "tests/type_traits/test_is_invocable",
  "tests/type_traits/test_tuple_like_types",
  "tests/type_traits/class_types/test_member_pointers",
  "tests/type_traits/class_types/test_member_reflections",
  "tests/type_traits/class_types/test_flattened_accessible_nsdm_1",
  "tests/type_traits/class_types/test_flattened_accessible_nsdm_2",
  "tests/type_traits/class_types/test_flattenable_types",
  "tests/type_traits/class_types/test_structured_types",
  "tests/type_traits/class_types/test_class_memberwise_predicate",
  -- Enum
  "tests/enum/impl/test_hash_collision_check",
  "tests/enum/test_enum_cast_from_integer",
  "tests/enum/test_enum_contains_integer",
  "tests/enum/test_enum_contains_string",
  "tests/enum/test_enum_count",
  "tests/enum/test_enum_entries",
  "tests/enum/test_enum_for_each",
  "tests/enum/test_enum_hash",
  "tests/enum/test_enum_index",
  "tests/enum/test_enum_json_static",
  "tests/enum/test_enum_json",
  "tests/enum/test_enum_meta_entries",
  "tests/enum/test_enum_names",
  "tests/enum/test_enum_switch",
  "tests/enum/test_enum_type_name",
  "tests/enum/test_enum_unique_count",
  "tests/enum/test_enum_unique_index",
  "tests/enum/test_enum_values",
  -- Lookup
  "tests/lookup/test_class_lookup_table_by_enum",
  "tests/lookup/test_class_lookup_table_by_name",
  "tests/lookup/test_namespace_lookup_table_by_enum",
  "tests/lookup/test_namespace_lookup_table_by_name",
  -- Type Operations
  "tests/type_operations/test_comparison",
  "tests/type_operations/test_define_aggregate",
  "tests/type_operations/test_to_structured",
  -- Annotations
  "tests/annotations/test_properties",
  -- TODO: Debugging
  "tests/annotations/validators/test_leaf_validators_1",
  "tests/annotations/validators/test_leaf_validators_2",
  "tests/annotations/validators/test_compound_validators",
}

for _, item in ipairs(meta_test_cases_with_custom_variants) do
  make_test_case(item.path, "")
  for _, variant in ipairs(item.variants) do
    make_test_case(item.path, variant.suffix, function ()
      add_defines(table.unpack(variant.defs))
    end)
  end
end
for i, path in ipairs(meta_test_cases_without_variant) do
  make_test_case(path, "")
end
