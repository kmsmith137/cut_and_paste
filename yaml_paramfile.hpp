#ifndef _YAML_PARAMFILE_HPP
#define _YAML_PARAMFILE_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_set>

#include <yaml-cpp/yaml.h>


struct yaml_paramfile {
    const std::string filename;
    YAML::Node yaml;

    std::unordered_set<std::string> all_keys;
    mutable std::unordered_set<std::string> requested_keys;

    yaml_paramfile(const std::string &filename);

    bool has_param(const std::string &k) const;
    void check_for_unused_params(bool fatal=true) const;

    // By default, throws an exception
    virtual void _die(const std::string &txt) const;

    // For debugging
    template<typename T> static inline std::string type_name();
    
    template<typename T>
    T read_scalar(const std::string &k) const
    {
	requested_keys.insert(k);

	if (!has_param(k))
	    _die("parameter '" + k + "' not found");

	try {
	    return yaml[k].as<T> ();
	}
	catch (...) { }

	_die(std::string("expected '") + k + std::string("' to have type ") + type_name<T>());
	return 0;   // compiler pacifier
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
template<> inline std::string yaml_paramfile::type_name<double> () { return "double"; }
template<> inline std::string yaml_paramfile::type_name<std::string> () { return "string"; }


#endif  // _YAML_PARAMFILE_HPP
