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
    bool check_for_unused_params(bool fatal=true) const;

    // For debugging and message-printing
    virtual void _die(const std::string &txt) const;    // by default, throws an exception
    virtual void _print(const std::string &txt) const;  // by default, prints to cout
    
    template<typename T> static inline std::string type_name();
    template<typename T> static inline std::string stringify(const T &x);
    template<typename T> static inline std::string stringify(const std::vector<T> &x);


    // _read_scalar1(): helper for read_scalar(), assumes key exists
    template<typename T>
    T _read_scalar1(const std::string &k) const
    {
	try {
	    return yaml[k].as<T> ();
	}
	catch (...) { }

	_die(std::string("expected '") + k + std::string("' to have type ") + type_name<T>());
	throw std::runtime_error("yaml_paramfile::_die() returned?!");
    }

    // _read_scalar2(): helper for read_scalar(), assumes key exists
    template<typename T>
    T _read_scalar2(const std::string &k) const
    {
	T ret = _read_scalar1<T> (k);
	requested_keys.insert(k);

	if (verbosity >= 2)
	    _print(k + " = " + stringify(ret) + "\n");

	return ret;
    }

    // "Vanilla" version of read_scalar()
    template<typename T>
    T read_scalar(const std::string &k) const
    {
	if (!has_param(k))
	    _die("parameter '" + k + "' not found");

	return _read_scalar2<T> (k);
    }

    // This version of read_scalar() has a default value, which is returned if the key is not found.
    template<typename T>
    T read_scalar(const std::string &k, T default_val) const
    {
	if (!has_param(k)) {
	    if (verbosity >= 1)
		_print("parameter '" + k + "' not found, using default value " + stringify(default_val) + "\n");
	    return default_val;
	}

	return _read_scalar2<T> (k);
    }


    // _read_vector1(): helper for read_vector(), assumes key exists
    // Automatically converts a scalar to length-1 vector.
    template<typename T>
    std::vector<T> _read_vector1(const std::string &k) const
    {
	try {
	    return yaml[k].as<std::vector<T>> ();
	}
	catch (...) { }

	try {
	    return { yaml[k].as<T>() };
	}
	catch (...) { }

	_die("expected '" + k + "' to have type " + type_name<T>() + ", or be a list of " + type_name<T>() + "s");
	throw std::runtime_error("yaml_paramfile::_die() returned?!");	
    }

    // _read_vector2(): helper for read_vector(), assumes key exists
    template<typename T>
    std::vector<T> _read_vector2(const std::string &k) const
    {
	std::vector<T> ret = _read_vector1<T> (k);
	requested_keys.insert(k);

	if (verbosity >= 2)
	    _print(k + " = " + stringify(ret) + "\n");

	return ret;
    }

    // "Vanilla" version of read_vector().
    // Automatically converts a scalar to length-1 vector.
    template<typename T>
    std::vector<T> read_vector(const std::string &k) const
    {
	if (!has_param(k))
	    _die("parameter '" + k + "' not found");

	return _read_vector2<T> (k);
    }

    // This version of read_vector() has a default value, which is returned if the key is not found.
    template<typename T>
    std::vector<T> read_vector(const std::string &k, const std::vector<T> default_val) const
    {
	if (!has_param(k)) {
	    if (verbosity >= 1)
		_print("parameter '" + k + "' not found, using default value " + stringify(default_val) + "\n");
	    return default_val;
	}

	return _read_vector2<T> (k);
    }
};


template<> inline std::string yaml_paramfile::type_name<int> () { return "int"; }
template<> inline std::string yaml_paramfile::type_name<bool> () { return "bool"; }
template<> inline std::string yaml_paramfile::type_name<float> () { return "float"; }
template<> inline std::string yaml_paramfile::type_name<double> () { return "double"; }
template<> inline std::string yaml_paramfile::type_name<std::string> () { return "string"; }


template<typename T> inline std::string yaml_paramfile::stringify(const T &x)
{
    std::stringstream ss;
    ss << x;
    return ss.str();
}

template<typename T> inline std::string yaml_paramfile::stringify(const std::vector<T> &x)
{
    std::stringstream ss;
    ss << "[ ";
    for (size_t i = 0; i < x.size(); i++)
	ss << (i ? ", " : "") << x[i];
    ss << " ]";
    return ss.str();
}


#endif  // _YAML_PARAMFILE_HPP
