#ifndef ARCHGEN_PARAMETERS_HPP
#define ARCHGEN_PARAMETERS_HPP

// This is the header for the Parameters class. It is a structure
// containing all necessary parameters for the simulation, with
// an option for reading them from a text file, thereby overriding
// the default values.

#pragma once

#include <string>
#include <vector>

struct Architecture;

struct Parameters {

    // Constructor
    Parameters(const std::string& = "");

    // Functions
    void read(const std::string&);
    void override(const Architecture&);
    void check() const;
    void save(const std::string&) const;

    // Internal functions
    void update();
    void checkinternal() const;

    // Parameters
    size_t popsize;                         // population size
    double allfreq;                         // initial frequency of allele 1
    double effect;                          // standard deviation of additive effect sizes of loci on traits
    double weight;                          // standard deviation of interaction weights of edges on traits
    size_t ntraits;                         // number of traits
    std::vector<size_t> nlocipertrait;      // number of loci affecting each trait
    std::vector<size_t> nedgespertrait;     // number of edges affecting each trait
    std::vector<double> skews;              // skewness of the degree distributions of each trait network
    std::vector<double> epistasis;          // scaling parameters for the importance of interactions in trait development
    std::vector<double> dominance;          // scaling parameters for the importance of dominance effects in trait development
    std::vector<double> envnoise;           // scaling parameters for the importance of environmental effects in trait development
    size_t sampling;                        // sampling mode for mutations
    double ratio;                           // density of mutations below which to shuffle only partially
    size_t seed;                            // random seed
    bool loadarch;                          // whether to read the genetic architecture from file
    bool savearch;                          // whether to save the genetic architecture to file
    bool savepars;                          // whether to save the parameters to file
    bool binary;                            // whether to save the matrix of alleles in binary (if not, CSV)
    bool verbose;                           // print progress to screen

    // Internal
    size_t nloci;                           // number of loci
    size_t nedges;                          // number of edges across all trait networks

};

#endif