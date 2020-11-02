#include "apara/Options.hpp"
#include "apara/Parallelize.hpp"

using namespace std;
using namespace ufo;
using namespace boost::filesystem;
using namespace apara;

int main (int argc, char ** argv) {
  Options o;
  path def_config("default.conf");
  if (exists(def_config)) o.parse_config(def_config);
  if (!o.parse_cmdline(argc, argv)) return 0;
  parallelizeCHCs(o);
  return 0;
}
