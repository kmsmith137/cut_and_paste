#include <fstream>
#include "yaml_paramfile.hpp"

int main(int argc, char **argv)
{
    int verbosity = 2;
    yaml_paramfile p("yaml_paramfile_example.yaml", verbosity);

    // Since the yaml_paramfile is constructed with verbosity=2, these values will all be printed to stdout.
    int mandatory_int_key = p.read_scalar<int> ("mandatory_int_key");
    float present_float_key = p.read_scalar<float> ("present_float_key", 100.1);
    float missing_double_key = p.read_scalar<double> ("missing_double_key", 100.1);

    // Throw 
    std::ofstream fnull("/dev/null");
    fnull << mandatory_int_key << present_float_key << missing_double_key;

    return 0;
}
