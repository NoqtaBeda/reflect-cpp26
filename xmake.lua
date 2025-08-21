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

function make_asm_check_case(path)
  local group_name, target_name, cpp_path =
    parse_test_case_path(path, "asm_check", "tests/asm_check")

  target(target_name, function ()
    set_kind("shared")
    set_group(group_name)
    add_files(cpp_path)
    set_languages("c++26")
    add_includedirs("include", ".")
    add_cxxflags("-freflection-latest")

    set_strip("all")
    set_symbols("hidden")
    set_optimize("fastest")

    set_policy("build.intermediate_directory", false)
    set_targetdir("$(buildir)/asm_check")
  end)
end

meta_test_cases = {
  -- Utility
  "utils/test_constant",
  "utils/test_identifier_naming",
  "utils/test_meta_tuple",
  "utils/test_preprocessors",
  "utils/test_to_string",
  "utils/test_type_tuple",
  "utils/test_utils_misc",
  "utils/fixed_map/test_integral_key",
  "utils/fixed_map/string_key/test_by_character_1",
  "utils/fixed_map/string_key/test_by_character_2",
  "utils/fixed_map/string_key/test_by_hash_search",
  "utils/fixed_map/string_key/test_by_hash_search_with_collision",
  "utils/fixed_map/string_key/test_by_hash_table_1",
  "utils/fixed_map/string_key/test_by_hash_table_2",
  "utils/fixed_map/string_key/test_by_hash_table_3",
  "utils/fixed_map/string_key/test_by_length",
  "utils/fixed_map/string_key/test_empty",
  "utils/fixed_map/string_key/test_naive",
  -- Type Traits
  "type_traits/test_cvref",
  "type_traits/test_function_types",
  "type_traits/test_is_invocable",
  "type_traits/test_string_like_types",
  "type_traits/test_tuple_like_types_1",
  "type_traits/test_tuple_like_types_2",
  "type_traits/class_types/test_ambiguous_inheritance",
  "type_traits/class_types/test_flattenable_types",
  "type_traits/class_types/test_flattened_nsdm",
  "type_traits/class_types/test_member_access",
  "type_traits/class_types/test_member_pointers",
  "type_traits/class_types/test_member_reflections",
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
  -- Lookup
  "lookup/test_class_lookup_table_by_enum",
  "lookup/test_class_lookup_table_by_name",
  "lookup/test_namespace_lookup_table_by_enum",
  "lookup/test_namespace_lookup_table_by_name",
  -- Type Operations
  "type_operations/test_comparison",
  "type_operations/test_member_access",
  "type_operations/test_to_string",
  "type_operations/test_to_structured",
  -- Validators
  -- "validators/compound/test_enum",
  -- "validators/compound/test_for_each",
  -- "validators/compound/test_front_back",
  -- "validators/compound/test_min_max_element",
  -- "validators/compound/test_size",
  "validators/leaf/test_arithmetic",
  "validators/leaf/test_boundary_options_exclusion_1",
  "validators/leaf/test_boundary_options_exclusion_2",
  "validators/leaf/test_contains",
  "validators/leaf/test_custom_validator",
  "validators/leaf/test_enum",
  "validators/leaf/test_non_empty",
  "validators/leaf/test_non_null",
  "validators/leaf/test_prefix_suffix",
  "validators/leaf/test_sorted",
  "validators/test_member_access_mode",
}

for i, path in ipairs(meta_test_cases) do
  make_test_case(path)
end

meta_asm_check_cases = {
  -- Enum
  "enum/enum_cast_from_string_dense",
  "enum/enum_cast_from_string_sparse",
  "enum/enum_index_dense",
  "enum/enum_index_sparse",
  "enum/enum_json",
  "enum/enum_json_static",
  "enum/enum_name_dense",
  "enum/enum_name_sparse",
  -- Validators
  "validators/arithmetic_boundary_test",
  "validators/arithmetic_boundary_test_with_error",
}

for i, path in ipairs(meta_asm_check_cases) do
  make_asm_check_case(path)
end

target("run_asm_check", function ()
  set_kind("phony")
  for i, path in ipairs(meta_asm_check_cases) do
    local _, target_name, _ =
      parse_test_case_path(path, "asm_check", "tests/asm_check")
    add_deps(target_name)
  end
  on_run(function (target)
    os.exec("python3 tests/asm_check/run.py -R $(projectdir) -c")
  end)
  set_targetdir("$(buildir)/asm_check")
end)
