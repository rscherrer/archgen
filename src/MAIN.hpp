#ifndef ARCHGEN_MAIN_HPP
#define ARCHGEN_MAIN_HPP

// This is the header for the main function.

#include "parameters.hpp"
#include "architecture.hpp"
#include <vector>
#include <string>
#include <bitset>

// Name space for saving functions ("save to file")
namespace stf {

    // Note: This is handy in testing.

    // Function to save trait values to file
    void saveTraits(const std::vector<double>&, const size_t&, const std::string&);

    // Function to save matrix of alleles to file
    void saveAlleles(std::vector<std::bitset<64u> >&, const size_t&, const std::string&, const bool& = false);

}

// Name space for genetic processing functions
namespace gen {

    // Function to throw mutations into the matrix of alleles
    void mutate(std::vector<std::bitset<64u> >&, const double&, const size_t&, const size_t&, const double& = 0.25);

    // Function to convert the matrix of alleles into a vector of trait values
    std::vector<double> develop(const std::vector<std::bitset<64u> >&, const Parameters&, const Architecture&, const size_t&);
    
}

// Main function
void doMain(const std::vector<std::string>&);

#endif