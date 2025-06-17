/**
 * Copyright (c) 2025 NoqtaBeda (noqtabeda@163.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/annotations.hpp>
#else
#include <reflect_cpp26/annotations/macros.h>
#include <reflect_cpp26/annotations/properties.hpp>
#endif

#define RFL_PROPERTY(...) REFLECT_CPP26_PROPERTY(__VA_ARGS__)

namespace rfl = reflect_cpp26;
namespace annots = reflect_cpp26::annotations;
using namespace std::literals;

struct foo_t {
  RFL_PROPERTY(rename, "foo_name") // Literal
  std::string name;

  static constexpr const char* args_rename = "foo_args";

  RFL_PROPERTY(rename, args_rename) // const char*
  RFL_PROPERTY(description,
    "argv[1:] i.e. list of runtime arguments "
    "excluding argv[0] the program name"s) // std::string
  RFL_PROPERTY(author, "John Doe (johndoe@example.com)"sv) // std::string_view
  RFL_PROPERTY(version, "1.2.3")
  std::vector<std::string> args;
};

struct bar_t {
  RFL_PROPERTY(arg_notation, 'a')
  RFL_PROPERTY(alias, "bar_alpha")
  RFL_PROPERTY(version_since, "2.3.4")
  double alpha;

  RFL_PROPERTY(arg_notation, 'b')
  RFL_PROPERTY(aliases, {"bar_beta", "bar_β"})
  RFL_PROPERTY(version_since, "2.4.6")
  double beta;
};

struct baz_t : foo_t, bar_t {};

TEST(AnnotationProperties, Basic)
{
  EXPECT_EQ_STATIC("foo_name", annots::rename_of<&foo_t::name>());
  EXPECT_EQ_STATIC("foo_args", annots::rename_of<&foo_t::args>());
  EXPECT_EQ_STATIC("argv[1:] i.e. list of runtime arguments "
            "excluding argv[0] the program name",
            annots::description_of<&foo_t::args>());
  EXPECT_EQ_STATIC("John Doe (johndoe@example.com)",
            annots::author_of<&foo_t::args>());
  EXPECT_EQ_STATIC("1.2.3", annots::version_of<&foo_t::args>());

  EXPECT_EQ_STATIC('a', annots::arg_notation_of<&bar_t::alpha>());
  EXPECT_EQ_STATIC("2.3.4", annots::version_since_of<&bar_t::alpha>());
  EXPECT_EQ_STATIC('b', annots::arg_notation_of<&bar_t::beta>());
  EXPECT_EQ_STATIC("2.4.6", annots::version_since_of<&bar_t::beta>());
  EXPECT_THAT(annots::aliases_of<&bar_t::alpha>(),
    testing::ElementsAre("bar_alpha"));
  EXPECT_THAT(annots::aliases_of<&bar_t::beta>(),
    testing::ElementsAre("bar_beta", "bar_β"));
}

TEST(AnnotationProperties, Inheritance)
{
  EXPECT_EQ_STATIC("foo_name", annots::rename_of<&baz_t::name>());
  EXPECT_EQ_STATIC("foo_args", annots::rename_of<&baz_t::args>());
  EXPECT_EQ_STATIC("argv[1:] i.e. list of runtime arguments "
            "excluding argv[0] the program name",
            annots::description_of<&baz_t::args>());
  EXPECT_EQ_STATIC("John Doe (johndoe@example.com)",
            annots::author_of<&baz_t::args>());
  EXPECT_EQ_STATIC("1.2.3", annots::version_of<&baz_t::args>());

  EXPECT_EQ_STATIC('a', annots::arg_notation_of<&baz_t::alpha>());
  EXPECT_EQ_STATIC("2.3.4", annots::version_since_of<&baz_t::alpha>());
  EXPECT_EQ_STATIC('b', annots::arg_notation_of<&baz_t::beta>());
  EXPECT_EQ_STATIC("2.4.6", annots::version_since_of<&baz_t::beta>());
  EXPECT_THAT(annots::aliases_of<&baz_t::alpha>(),
    testing::ElementsAre("bar_alpha"));
  EXPECT_THAT(annots::aliases_of<&baz_t::beta>(),
    testing::ElementsAre("bar_beta", "bar_β"));
}
