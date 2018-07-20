# file      : libbutl/manifest-parser.bash
# copyright : Copyright (c) 2014-2018 Code Synthesis Ltd
# license   : MIT; see accompanying LICENSE file

function butl_parse_manifest ()
{
  printf ":1\0"
  printf "name:foo\0"
  printf "version:1.2.3\0"
  printf "description:foo\nexecutable\0"
  printf "depends:libfoo\0"
  printf "depends:libbar\0"
}
