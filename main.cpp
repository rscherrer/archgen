#include "src/MAIN.hpp"

#include <iostream>

// Main function
int main(int argc, char * argv[]) {

    // argc: number of arguments
    // argv: vector of arguments

    // Convert arguments into a vector of strings
    const std::vector<std::string> args(argv, argv + argc);

    // Try to...
    try {
        
        // Run the program
        doMain(args);

        // Exit
        return 0;

    }
    catch (const std::exception& err) {

        // Catch exceptions
        std::cerr << "Exception: " << err.what() << '\n';

    }
    catch (const char* err) {

        // Catch error messages
        std::cerr << "Exception: " << err << '\n';

    }
    catch (...) {

        // Or unknown errors
        std::cerr << "Unknown Exception\n";

    }

    // Return failure flag if got here
    return 1;

}