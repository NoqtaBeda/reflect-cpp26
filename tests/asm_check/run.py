# MIT License
# 
# Copyright (c) 2025 NoqtaBeda (noqtabeda@163.com)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import argparse
from colorama import Fore
import os
import re
import subprocess
import yaml


def parse_args():
  parser = argparse.ArgumentParser()
  parser.add_argument('--root-dir', '-R', type=str, default='.',
                      help='Root directory of reflect_cpp26 repository')
  parser.add_argument('--colored-output', '-c', action='store_true',
                      help='Colored terminal output')
  parser.add_argument('--stops-on-first-failure', '-s', action='store_true',
                      help='Stops testing after first failed test case')
  parser.add_argument('--returns-non-zero-on-failure', '-z',
                      action='store_true',
                      help='Exits with non-zero return code ' +
                           'if any test case fails')
  parser.add_argument('--verbose', '-v', action='store_true',
                      help='Displays full message')
  return parser.parse_args()


def check_symbols(args, symbols, common_options, case_options):
  error_msg = ''

  def append_error_msg(pattern, prompt, matched_symbols):
    nonlocal args, error_msg
    if error_msg != '':
      error_msg += '\n'
    n = len(matched_symbols)
    error_msg += f"Expects pattern '{pattern}' {prompt}, " + \
                 f"but it appears {n} time(s) actually"
    if args.verbose and len(matched_symbols) > 0:
      error_msg += ':'
      for s in matched_symbols:
        error_msg += f"\n* {s}"
    else:
      error_msg += '.'

  def check_body(options):
    nonlocal args, symbols

    for pattern_str in options:
      pattern = re.compile(pattern_str)
      matched_symbols = list(filter(lambda s: pattern.search(s), symbols))

      def check_expected_condition_string(expects):
        nonlocal pattern_str, matched_symbols
        if expects == 'unexpected':
          if len(matched_symbols) > 0:
            append_error_msg(pattern_str, 'not to appear', matched_symbols)
        else:
          raise Exception(f"Unexpected condition string '{expects}'.")

      def check_expected_condition_dict(expects):
        nonlocal pattern_str, matched_symbols
        prompt = None
        for key in expects:
          if key == 'max_count':
            if len(matched_symbols) > expects['max_count']:
              prompt = f"to appear at most {expects['max_count']} times"
              break
          elif key == 'min_count':
            if len(matched_symbols) < expects['min_count']:
              prompt = f"to appear at least {expects['min_count']} times"
              break
          elif key == 'exact_count':
            if len(matched_symbols) != expects['exact_count']:
              prompt = f"to appear exactly {expects['exact_count']} times"
              break
          else:
            raise Exception(f"Unexpected key '{key}'.")

        if prompt is not None:
          append_error_msg(pattern_str, prompt, matched_symbols)

      expects = options[pattern_str]
      if type(expects) == str:
        check_expected_condition_string(expects)
      elif type(expects) == dict:
        check_expected_condition_dict(expects)

  check_body(case_options)
  if error_msg == '':
    check_body(common_options)

  return error_msg == '', error_msg


def make_pass_header(args):
  str = '[PASS]'
  if args.colored_output:
    return Fore.GREEN + str + Fore.RESET
  else:
    return str


def make_ignored_header(args, passed):
  str = '[PASS]' if passed else '[FAIL]'
  if args.colored_output:
    return Fore.YELLOW + str + Fore.RESET
  else:
    return str


def make_fail_header(args):
  str = '[FAIL]'
  if args.colored_output:
    return Fore.RED + str + Fore.RESET
  else:
    return str


def main():
  args = parse_args()
  asm_check_dir = os.path.join(args.root_dir, 'tests/asm_check')
  yaml_path = os.path.join(asm_check_dir, 'test_cases.yaml')
  with open(yaml_path, 'r', encoding='utf-8') as file:
    test_cases = yaml.safe_load(file)

  build_dir = os.path.join(args.root_dir, 'build/asm_check')
  pass_count = 0
  ignored_yet_passed_cases = []
  ignored_and_failed_cases = []
  failed_cases = []

  print("Starts ASM check:")
  for cpp_path in test_cases:
    if cpp_path == 'common':
      continue

    case_name = 'asm_check-' + cpp_path.replace('/', '-')
    build_file_path = os.path.join(build_dir, 'lib' + case_name + '.so')

    ignored = 'ignored' in test_cases[cpp_path] and \
      test_cases[cpp_path]['ignored']

    strings_result = subprocess.run(['strings', build_file_path],
      capture_output=True, text=True, check=True)
    symbols = strings_result.stdout.split('\n')

    check_res, error_msg = check_symbols(
      args, symbols, test_cases['common'], test_cases[cpp_path]['patterns'])
    if check_res:
      if ignored:
        print(f"{make_ignored_header(args, True)} {cpp_path}")
        ignored_yet_passed_cases.append(cpp_path)
      else:
        print(f"{make_pass_header(args)} {cpp_path}")
        pass_count += 1
    else:
      if ignored:
        print(f"{make_ignored_header(args, False)} {cpp_path}")
        ignored_and_failed_cases.append(cpp_path)
      else:
        print(f"{make_fail_header(args)} {cpp_path}\n{error_msg}")
        if args.stops_on_first_failure:
          return 1 if args.returns_non_zero_on_failure else 0
        failed_cases.append(cpp_path)

  n_failed = len(failed_cases)
  n_ignored_passed = len(ignored_yet_passed_cases)
  n_ignored_failed = len(ignored_and_failed_cases)
  # 1 : Excludes 'common'
  n_not_ignored = len(test_cases) - n_ignored_passed - n_ignored_failed - 1

  print(f"Summary: {pass_count} / {n_not_ignored} test cases passed.")
  if n_failed > 0:
    print(f"{n_failed} test cases failed:\n* " +
          "\n* ".join(failed_cases))
  if n_ignored_passed > 0:
    print(f"{n_ignored_passed} test cases ignored yet passed:\n* " +
          "\n* ".join(ignored_yet_passed_cases))
  if n_ignored_failed > 0:
    print(f"{n_ignored_failed} test cases ignored and failed:\n* " +
          "\n* ".join(ignored_and_failed_cases))

  return 1 if args.returns_non_zero_on_failure and n_failed > 0 else 0


if __name__ == '__main__':
  retcode = main()
  exit(retcode)
