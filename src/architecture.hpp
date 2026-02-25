#ifndef ARCHGEN_ARCHITECTURE_HPP
#define ARCHGEN_ARCHITECTURE_HPP

// This is the header for the Architecture class. It is a container
// for parameters related to the genetic architecture of traits.
// Note that it is separate from the regular parameters, as it
// can contain many values, making it handier to place it in
// its own class.

#pragma once

#include <string>
#include <vector>

struct Parameters;

struct Architecture {

    // Constructor
    Architecture(const std::string& = "");

    // Functions
    void read(const std::string&);
    void generate(const Parameters&);
    void check() const;
    void save(const std::string&) const;

    // Internal functions
    void checkinternal() const;

    // Hyperparameters
    size_t nloci;
    size_t nedges;
    size_t ntraits;

    // Locus-specific parameters
    std::vector<size_t> traitids;
    std::vector<double> effects;
    std::vector<double> dominances;

    // Edge-specific parameters
    std::vector<size_t> from;
    std::vector<size_t> to;
    std::vector<double> weights;

    // Internals
    std::vector<size_t> nlocipertrait;
    std::vector<size_t> nedgespertrait;

};

#endif