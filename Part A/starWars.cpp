// AD48FB4835AF347EB0CA8009E24C3B13F8519882

/*

    A utility file containing the member function definitions of the class starWars.

*/

#include "starWars.h"
#include <getopt.h>

// Read and process command line options.
void starWars::getOptions(int argc, char** argv) {

    int option_index = 0, option = 0;
    
    // Don't display getopt error messages about options.
    opterr = false;

    // Use getopt to find command line options.
    struct option longOpts[] = {{"verbose",      no_argument, nullptr, 'v' },
                                {"median",       no_argument, nullptr, 'm' },
                                {"general-eval", no_argument, nullptr, 'g' },
                                {"watcher",      no_argument, nullptr, 'w' },
                                {nullptr,        0,           nullptr, '\0'}};
    

    
    while ((option = getopt_long(argc, argv, "sqho:", longOpts, &option_index)) != -1) {
        switch (option) {
            case 's':
                // Set routing scheme to 'stack' mode.
                isStack = 1;
                ++sqOnce;
                break;

            case 'h':
                // Prints a help message describing what this program does.
                cout << "Usage: " << argv[0] << " [-s -o M|L]|-h\n"
                     << "This program helps you rescue the countess in Super Marco\n";
                exit(0);
                
            case 'q':
                // Set routing scheme to 'queue' mode.
                isStack = 0;
                ++sqOnce;
                break;
                
            case 'o':
                // Sets the output format to either 'map' or 'list'.
                outputFormat = *optarg;
                break;
        }
    }
        
    // If routing scheme is not specifed, then we exit 1
    // while printing an error message.
    if (sqOnce == 0)  {
        cerr << "Stack or queue must be specified\n";
        exit(1);
    }
    
    // If more than one routing scheme is specifed, then we exit 1
    // while printing an error message.
    if (sqOnce > 1) {
        cerr << "Stack or queue can only be specified once\n";
        exit(1);
    }
    
    // If  and output mode is not specified, default to 'map' output.
    if (outputFormat == ' ') {
        outputFormat = 'M';
    }
}
