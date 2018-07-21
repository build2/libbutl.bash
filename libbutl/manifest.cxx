// file      : libbutl/manifest.cxx
// copyright : Copyright (c) 2014-2018 Code Synthesis Ltd
// license   : MIT; see accompanying LICENSE file

#include <string>
#include <iostream>

#include <libbutl/utility.mxx>
#include <libbutl/manifest-parser.mxx>
#include <libbutl/manifest-serializer.mxx>

using namespace std;
using namespace butl;

static int
cmd_parse ()
{
  //@@ TODO

  const char m[] =
    ":1\0"
    "name:foo\0"
    "version:1.2.3\0"
    "description:foo\nexecutable\0"
    "depends:libfoo\0"
    "depends:libbar"; // Last \0 will be added.

  cout.write (m, sizeof (m));

  return 0;
}

static int
cmd_serialize ()
{
  //@@ TODO

  return 0;
}

int
main (int argc, char* argv[])
{
  // We should switch to CLI if we need anything more elaborate.
  //
  if (argc < 2)
  {
    cerr << "error: missing command" << endl;
    return 1;
  }

  string c (argv[1]);

  if (c == "parse")     return cmd_parse ();
  if (c == "serialize") return cmd_serialize ();

  cerr << "error: unknown command '" << c << "'" << endl;
  return 1;
}
