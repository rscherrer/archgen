#ifndef ARCHGEN_CHECKER_HPP
#define ARCHGEN_CHECKER_HPP

// This is the header for the chk (checker) namespace.

#include <vector>
#include <string>

namespace chk {

    // Function to check that a value is a proportion
    template <typename T> 
    std::string proportion(const T &x) { 
        
        return x < 0.0 || x > 1.0 ? "must be between 0 and 1" : "";

    }

    // Function to check that a value is positive
    template <typename T>
    std::string positive(const T &x) {

        return x < 0.0 ? "must be positive" : "";

    }

    // Function to check that a value is strictly positive
    template <typename T>
    std::string strictpos(const T &x) {

        return x <= 0.0 ? "must be strictly positive" : "";

    }

    // Function to check that a value is between 0 and 4
    template <typename T>
    std::string zerotothree(const T &x) {

        return x < 0.0 || x > 3.0 ? "must be between 0 and 3" : "";

    }
}

#endif