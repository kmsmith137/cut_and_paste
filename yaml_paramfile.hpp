#ifndef _YAML_PARAMFILE_HPP
#define _YAML_PARAMFILE_HPP

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

#include <yaml-cpp/yaml.h>


struct yaml_paramfile {
    const std::string filename;
    YAML::Node yaml;
    int verbosity;

    std::unordered_set<std::string> all_keys;
    mutable std::unordered_set<std::string> requested_keys;

    // The 'verbosity' constructor argument has the following meaning:
    //   0 = quiet
    //   1 = announce default values for all unspecified parameters
    //   2 = announce all parameters
    yaml_paramfile(const std::string &filename, int verbosity=0);

    bool has_param(const std::string &k) const;
    void check_for_unused_params(bool fatal=true) const;

    virtual void _die(const std::string &txt) const;    // by default, throws an exception
    virtual void _print(const std::string &txt) const;  // by default, prints to cout
    
    // For debugging and message-printing
    template<typename T> static inline std::string type_name();
    template<typename T> static inline std::string stringify(const T &x) { std::stringstream ss; ss << x; return ss.str(); }

    // _read_scalar(): assumes key exists
    template<typename T>
    T _read_scalar(const std::string &k) const
    {
	T ret;

	try {
	    ret = yaml[k].as<T> ();
	}
	catch (...) {
	    _die(std::string("expected '") + k + std::string("' to have type ") + type_name<T>());
	    return 0;  // compiler pacifier
	}

	if (verbosity >= 2)
	    _print(k + " = " + stringify(ret) + "\n");

	return ret;
    }

    // "Vanilla" version of read_scalar()
    template<typename T>
    T read_scalar(const std::string &k) const
    {
	requested_keys.insert(k);

	if (!has_param(k))
	    _die("parameter '" + k + "' not found");

	return _read_scalar<T> (k);
    }

    // This version of read_scalar() has a default value, which is returned if the key is not found.
    template<typename T>
    T read_scalar(const std::string &k, T default_val) const
    {
	requested_keys.insert(k);

	if (!has_param(k)) {
	    if (verbosity >= 1)
		_print("parameter '" + k + "' not found, using default value " + stringify(default_val) + "\n");
	    return default_val;
	}

	return _read_scalar<T> (k);
    }

    // Automatically converts a scalar to length-1 vector.
    template<typename T>
    std::vector<T> read_vector(const std::string &k) const
    {
	requested_keys.insert(k);

	if (!has_param(k))
	    _die("parameter '" + k + "' not found");

	try {
	    return yaml[k].as<std::vector<T>> ();
	}
	catch (...) { }

	try {
	    return { yaml[k].as<T>() };
	}
	catch (...) { }

	_die("expected '" + k + "' to have type " + type_name<T>() + ", or be a list of " + type_name<T>() + "s");
	return { };   // compiler pacifier
    }
};


template<> inline std::string yaml_paramfile::type_name<int> () { return "int"; }
template<> inline std::string yaml_paramfile::type_name<float> () { return "float"; }
template<> inline std::string yaml_paramfile::type_name<double> () { return "double"; }
template<> inline std::string yaml_paramfile::type_name<std::string> () { return "string"; }


#endif  // _YAML_PARAMFILE_HPP
