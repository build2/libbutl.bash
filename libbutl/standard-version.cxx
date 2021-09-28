// file      : libbutl/standard-version.cxx
// license   : MIT; see accompanying LICENSE file

#include <string>
#include <cstddef>   // size_t
#include <iostream>
#include <stdexcept> // invalid_argument

#include <libbutl/utility.hxx>          // operator<<(ostream,exception)
#include <libbutl/optional.hxx>
#include <libbutl/standard-version.hxx>

using namespace std;
using namespace butl;

// Usage: argv[0] [<options>] <standard-version>
//
// Check that the specified predicates are true for a standard version.
//
// See standard-version.bash.in for the functionality and options description.
//
int
main (int argc, char* argv[])
{
  using butl::optional;

  // We could probably try to parse the last argument as a version first and
  // then evaluate the predicates while parsing the options. This, however
  // would worsen the diagnostics (think about the `standard-version --beta`
  // command) and would complicate things a bit, since at the time of the
  // version parsing we wouldn't know if to print the diagnostics for an
  // invalid version. Thus, we will parse the arguments in the direct order.
  //
  optional<bool> snapshot;
  optional<bool> latest_sn;
  optional<bool> alpha;
  optional<bool> beta;
  optional<bool> pre_release;
  optional<bool> release;
  optional<bool> final;
  optional<bool> stub;
  optional<bool> earliest;

  bool valid    (false); // --is-version is specified.
  bool opposite (false); // Opposite predicates are specified simultaneously
                         // (i.e. --is-beta and --is-not-beta).

  int i (1);
  for (; i != argc; ++i)
  {
    string op (argv[i]);

    bool is;
    size_t n;

         if (op.compare (0, (n = 9), "--is-not-") == 0) is = false;
    else if (op.compare (0, (n = 5), "--is-")     == 0) is = true;
    else break;

    string p (op, n);

    // Set the predicate expected value. If the opposite value have already
    // been specified, then save this information to exit with code 1 later
    // (note that we still need to validate the rest of the arguments).
    //
    auto set = [is, &opposite] (optional<bool>& v)
    {
      if (!v)
        v = is;
      else if (*v != is)
        opposite = true;
    };

         if (p == "snapshot")        set (snapshot);
    else if (p == "latest-snapshot") set (latest_sn);
    else if (p == "alpha")           set (alpha);
    else if (p == "beta")            set (beta);
    else if (p == "pre-release")     set (pre_release);
    else if (p == "release")         set (release);
    else if (p == "final")           set (final);
    else if (p == "stub")            set (stub);
    else if (p == "earliest")        set (earliest);
    else if (p == "version" && is)   valid = true;
    else break;
  }

  if (i == argc)
  {
    cerr << "error: missing version" << endl;
    return 3;
  }

  string a (argv[i++]);

  if (i != argc)
  {
    cerr << "error: unexpected argument '" << argv[i] << "'" << endl;
    return 3;
  }

  try
  {
    standard_version v (a,
                        (standard_version::allow_earliest |
                         standard_version::allow_stub));

    return !opposite                                                       &&
           (!snapshot    || *snapshot    == v.snapshot())                  &&
           (!latest_sn   || *latest_sn   == v.latest_snapshot ())          &&
           (!alpha       || *alpha       == v.alpha ().has_value ())       &&
           (!beta        || *beta        == v.beta ().has_value ())        &&
           (!pre_release || *pre_release == v.pre_release ().has_value ()) &&
           (!release     || *release     == v.release ())                  &&
           (!final       || *final       == v.final ())                    &&
           (!stub        || *stub        == v.stub ())                     &&
           (!earliest    || *earliest    == v.earliest ())
           ? 0
           : 1;
  }
  catch (const invalid_argument& e)
  {
    if (!valid)
      cerr << "error: " << e << endl;

    return 2;
  }
}
