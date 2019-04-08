// file      : libbutl/manifest.cxx
// copyright : Copyright (c) 2014-2019 Code Synthesis Ltd
// license   : MIT; see accompanying LICENSE file

#include <ios>       // ios::failure, ios::*bit
#include <string>
#include <cassert>
#include <cstddef>   // size_t
#include <iostream>
#include <stdexcept> // invalid_argument

#include <libbutl/utility.mxx>
#include <libbutl/fdstream.mxx>
#include <libbutl/manifest-parser.mxx>
#include <libbutl/manifest-serializer.mxx>

using namespace std;
using namespace butl;

// Set the binary translation mode for stdout. Throw ios::failure on error.
// Noop on POSIX.
//
// Note that it makes sense to set the binary mode for stdout not to litter
// the resulting manifest representation with the carriage return characters
// on Windows.
//
static void
stdout_binary ()
{
  try
  {
    stdout_fdmode (fdstream_mode::binary);
  }
  catch (const invalid_argument&)
  {
    assert (false); // No reason to happen.
  }
}

static int
cmd_parse ()
{
  using parser     = manifest_parser;
  using parsing    = manifest_parsing;
  using name_value = manifest_name_value;

  // Parse the manifest list and write its binary representation.
  //
  try
  {
    stdout_binary ();

    cin.exceptions  (ios::badbit | ios::failbit);
    cout.exceptions (ios::badbit | ios::failbit);

    parser p (cin, "stdin");

    // Iterate over manifests in the list.
    //
    for (name_value nv (p.next ()); !nv.empty (); nv = p.next ())
    {
      // Iterate over manifest name/values.
      //
      for (; !nv.empty (); nv = p.next ())
        cout << nv.name << ':' << nv.value << '\0' << flush;
    }
  }
  catch (const parsing& e)
  {
    cerr << e << endl;
    return 1;
  }
  catch (const ios::failure& e)
  {
    cerr << "error: unable to read from stdin or write to stdout: " << e
         << endl;
    return 1;
  }

  return 0;
}

static int
cmd_serialize (bool long_lines)
{
  using serializer    = manifest_serializer;
  using serialization = manifest_serialization;

  try
  {
    stdout_binary ();

    // Don't throw when failbit is set (getline() failed to extract any
    // characters).
    //
    cin.exceptions  (ios::badbit);

    cout.exceptions (ios::badbit | ios::failbit);

    serializer s (cout, "stdout", long_lines);

    for (string l; !eof (getline (cin, l, '\0')); )
    {
      size_t p (l.find (':'));

      if (p == string::npos)
        throw serialization (s.name (), "':' expected after name");

      string n (l, 0, p);
      string v (l, p + 1);

      // Validates the name. Expects the first pair to be the format version.
      // Ends current and starts next manifest for the start-of-manifest pair.
      //
      s.next (n, v);
    }

    s.next ("", ""); // End of manifest list.
  }
  catch (const serialization& e)
  {
    cerr << e << endl;
    return 1;
  }
  catch (const ios::failure& e)
  {
    cerr << "error: unable to read from stdin or write to stdout: " << e
         << endl;
    return 1;
  }

  return 0;
}

// Usages:
//
// argv[0] parse
// argv[0] [--long-lines] serialize
//
int
main (int argc, char* argv[])
{
  // We should switch to CLI if we need anything more elaborate.
  //
  int i (1);
  bool long_lines (false);
  for (; i != argc; ++i)
  {
    string op (argv[i]);

    if (op == "--long-lines")
      long_lines = true;
    else
      break;
  }

  if (i == argc)
  {
    cerr << "error: missing command" << endl;
    return 1;
  }

  string c (argv[i]);

  if (c == "parse")     return cmd_parse ();
  if (c == "serialize") return cmd_serialize (long_lines);

  cerr << "error: unknown command '" << c << "'" << endl;
  return 1;
}
