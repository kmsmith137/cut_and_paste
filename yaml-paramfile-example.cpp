#include <fstream>
#include <iostream>
#include "yaml_paramfile.hpp"

using namespace std;


int main(int argc, char **argv)
{
    int verbosity = 2;
    yaml_paramfile p("yaml_paramfile_example.yaml", verbosity);

    // Since the yaml_paramfile is constructed with verbosity=2, these values will all be printed to stdout.
    int mandatory_int_key = p.read_scalar<int> ("mandatory_int_key");
    float present_float_key = p.read_scalar<float> ("present_float_key", 100.1);
    float missing_double_key = p.read_scalar<double> ("missing_double_key", 100.1);

    vector<float> mandatory_vector_float_key = p.read_vector<float> ("mandatory_vector_float_key");
    vector<int> present_vector_int_key = p.read_vector<int> ("present_vector_int_key", { 2, 3 });
    vector<int> missing_vector_int_key = p.read_vector<int> ("missing_vector_int_key", { 2, 3 });

    bool boolean_key = p.read_scalar<bool> ("boolean_key");

    // Throw some data in /dev/null, to suppress 'unused variable...' compiler warning
    std::ofstream fnull("/dev/null");
    fnull << mandatory_int_key << present_float_key << missing_double_key << boolean_key
	  << mandatory_vector_float_key.size() << present_vector_int_key.size() << missing_vector_int_key.size();

    return 0;
}
