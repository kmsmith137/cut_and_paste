#include "argument_parser.hpp"
#include <iostream>

using namespace std;


int main(int argc, char **argv)
{
    bool aflag;
    bool bflag;
    bool cflag;
    int c = -1;
    int d = -1;

    argument_parser parser;
    parser.add_boolean_flag("-a", aflag);
    parser.add_boolean_flag("-b", bflag);
    parser.add_flag_with_parameter("-c", c, cflag);
    parser.add_flag_with_parameter("-d", d);

    if (!parser.parse_args(argc, argv)) {
	cerr << "parse_args() failed!\n";
	exit(1);
    }

    cout << "aflag: " << (aflag ? "true" : "false") << endl;
    cout << "bflag: " << (bflag ? "true" : "false") << endl;
    cout << "cflag: " << (cflag ? "true" : "false") << endl;
    cout << "c: " << c << endl;
    cout << "d: " << d << endl;
    cout << "remaining args:";
    
    for (int i = 0; i < parser.nargs; i++)
	cout << " " << parser.args[i];
    
    cout << endl;
    return 0;
}

