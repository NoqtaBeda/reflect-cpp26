-- Copyright (c) 2026 NoqtaBeda (noqtabeda@163.com)
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

function parse_test_case_path(path, group_prefix, path_prefix)
  local _, last_slash_index = string.find(path, ".*/")
  local group = group_prefix .. "/" .. string.sub(path, 1, last_slash_index - 1)
  local target = group_prefix .. "-" .. string.gsub(path, "/", "-")
  local cpp_path = path_prefix .. "/" .. path .. ".cpp"
  return group, target, cpp_path
end

function make_test_case(path)
  local group_name, target_name, cpp_path =
    parse_test_case_path(path, "tests", "tests")

  target(target_name, function ()
    set_kind("binary")
    set_group(group_name)
    add_files(cpp_path)
    set_languages("c++26")
    add_includedirs("include", ".")
    add_cxxflags("-freflection-latest")
    add_packages("gtest")
    add_options("full-header-test")
    add_options("static-test")
  end)
end

function make_example(path)
  local group_name, target_name, cpp_path =
    parse_test_case_path(path, "examples", "examples")

  target(target_name, function ()
    set_kind("binary")
    set_group(group_name)
    add_files(cpp_path)
    set_languages("c++26")
    add_includedirs("include")
    add_cxxflags("-freflection-latest")
  end)
end

meta_test_cases = {
  -- Utility
  "utils/test_addressable_member",
  "utils/test_ctype",
  "utils/test_identifier_naming",
  "utils/test_meta_tuple",
  "utils/test_to_string",
  "utils/test_type_tuple",
  "utils/test_utils_misc",
  -- Type Traits
  "type_traits/test_cvref",
  "type_traits/test_is_invocable",
  "type_traits/test_string_like_types",
  "type_traits/test_tuple_like_types_1",
  "type_traits/test_tuple_like_types_2",
  "type_traits/class_types/test_ambiguous_inheritance",
  "type_traits/class_types/test_flattenable_types",
  "type_traits/class_types/test_flattened_nsdm",
  "type_traits/class_types/test_member_access",
  "type_traits/class_types/test_structured_types",
  -- Enum
  "enum/impl/test_enum_flags_category",
  "enum/test_enum_bitwise_operators",
  "enum/test_enum_cast_from_integer",
  "enum/test_enum_cast_from_string",
  "enum/test_enum_cast_from_string_ci",
  "enum/test_enum_comparison_operators",
  "enum/test_enum_contains_integer",
  "enum/test_enum_contains_string",
  "enum/test_enum_contains_string_ci",
  "enum/test_enum_count",
  "enum/test_enum_entries",
  "enum/test_enum_flags_cast_from_integer",
  "enum/test_enum_flags_cast_from_string",
  "enum/test_enum_flags_cast_from_string_ci",
  "enum/test_enum_flags_contains_integer",
  "enum/test_enum_flags_contains_string",
  "enum/test_enum_flags_contains_string_ci",
  "enum/test_enum_flags_name",
  "enum/test_enum_format",
  "enum/test_enum_hash",
  "enum/test_enum_index",
  "enum/test_enum_json_static",
  "enum/test_enum_json",
  "enum/test_enum_meta_entries",
  "enum/test_enum_name",
  "enum/test_enum_names",
  "enum/test_enum_switch",
  "enum/test_enum_type_name",
  "enum/test_enum_unique_count",
  "enum/test_enum_unique_index",
  "enum/test_enum_values",
  -- Fixed map
  "fixed_map/integral_key/test_custom_kv_pair",
  "fixed_map/integral_key/test_dense",
  "fixed_map/integral_key/test_empty",
  "fixed_map/integral_key/test_fully_dense",
  "fixed_map/integral_key/test_fully_dense_int8",
  "fixed_map/integral_key/test_general",
  "fixed_map/integral_key/test_scoped_enum",
  "fixed_map/integral_key/test_sparse",
  "fixed_map/integral_key/test_unscoped_enum",
  "fixed_map/string_key/test_by_hash_search_1",
  "fixed_map/string_key/test_by_hash_search_2",
  "fixed_map/string_key/test_by_hash_search_with_collision",
  "fixed_map/string_key/test_by_hash_table_1",
  "fixed_map/string_key/test_by_hash_table_2",
  "fixed_map/string_key/test_by_hash_table_3",
  "fixed_map/string_key/test_by_length",
  "fixed_map/string_key/test_empty",
  "fixed_map/string_key/test_naive",
  -- Lookup
  "lookup/class_member/test_overloads",
  "lookup/class_member/enum_key/test_basic",
  "lookup/class_member/enum_key/test_categories",
  "lookup/class_member/enum_key/test_custom_filter",
  "lookup/class_member/enum_key/test_inheritance",
  "lookup/class_member/string_key/test_access_context",
  "lookup/class_member/string_key/test_basic",
  "lookup/class_member/string_key/test_case_insensitive",
  "lookup/class_member/string_key/test_categories",
  "lookup/class_member/string_key/test_custom_filter",
  "lookup/class_member/string_key/test_custom_filter_with_options",
  "lookup/class_member/string_key/test_deleted",
  "lookup/class_member/string_key/test_inheritance_1",
  "lookup/class_member/string_key/test_inheritance_2",
  "lookup/class_member/string_key/test_pointers",
  "lookup/class_member/string_key/test_references_and_bitfields",
  "lookup/class_member/string_key/test_special_members",
  "lookup/class_member/string_key/test_templates",
  "lookup/namespace_member/test_overloads",
  "lookup/namespace_member/enum_key/test_basic",
  "lookup/namespace_member/enum_key/test_categories",
  "lookup/namespace_member/enum_key/test_custom_filter",
  "lookup/namespace_member/enum_key/test_custom_transform",
  "lookup/namespace_member/string_key/test_basic",
  "lookup/namespace_member/string_key/test_categories",
  "lookup/namespace_member/string_key/test_custom_filter",
  "lookup/namespace_member/string_key/test_deleted",
  "lookup/namespace_member/string_key/test_pointers",
  "lookup/namespace_member/string_key/test_references",
  "lookup/namespace_member/string_key/test_templates",
  -- Type Operations
  "type_operations/test_comparison",
  "type_operations/test_member_access",
  "type_operations/test_member_name_access",
  "type_operations/test_to_string",
  "type_operations/test_to_structured",
}

for i, path in ipairs(meta_test_cases) do
  make_test_case(path)
end

meta_examples = {
  "lookup/basic",
  "lookup/using_filter_function",
  "lookup/using_transformation_to_enum",
  "lookup/using_transformation_to_string"
}

for i, path in ipairs(meta_examples) do
  make_example(path)
end
