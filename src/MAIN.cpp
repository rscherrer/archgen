// This is the implementation of the main function. It
// is separate from main.cpp so that it can be tested.

#include "MAIN.hpp"
#include "parameters.hpp"
#include "random.hpp"
#include "architecture.hpp"
#include <iostream>
#include <bitset>
#include <cassert>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <numeric>

// Function to import matrix of alleles from file
void gen::import(std::vector<std::bitset<64u> > &alleles, const std::string &filename, const size_t &N) {

    // alleles: vector of bitsets representing matrix of alleles
    // filename: name of the file to import
    // N: total number of alleles in the population

    // Create input file stream
    std::ifstream file(filename);

    // Check that it is open
    if (!file.is_open())
        throw std::runtime_error("Unable to open file " + filename);

    // Skip header
    std::string line;
    std::getline(file, line);

    // Number of bits per bitset
    const size_t n = 64u;

    // Number of diploid genotype entries expected in the file
    const size_t ngenotypes = N / 2u;

    // For each line in the file...
    size_t i = 0u;
    while (std::getline(file, line)) {

        // Parse line as CSV
        std::stringstream stream(line);
        std::string token;

        // Skip the first token (identifier column)
        if (!std::getline(stream, token, ','))
            throw std::runtime_error("Invalid format in file " + filename);

        // Parse only genotype columns
        while (std::getline(stream, token, ',')) {

            // Check that file does not contain too many genotype entries
            if (i >= ngenotypes)
                throw std::runtime_error("Incorrect number of genotypes read from file " + filename);

            // Convert token to integer
            int value = 0;
            try {
                value = std::stoi(token);
            }
            catch (const std::invalid_argument&) {
                throw std::runtime_error("Invalid format in file " + filename);
            }
            catch (const std::out_of_range&) {
                throw std::runtime_error("Invalid format in file " + filename);
            }

            // Check that it is a valid genotype
            if (value < 0 || value > 2)
                throw std::runtime_error("Invalid genotype value in file " + filename);

            // Set the corresponding bits in the bitsets
            if (value == 1) alleles[i / n].set(i % n);
            else if (value == 2) {
                alleles[i / n].set(i % n);
                alleles[(i + ngenotypes) / n].set((i + ngenotypes) % n);
            }

            // Move to the next allele
            ++i;

        }
    }

    // Check that we have read the expected number of genotype entries
    if (i != ngenotypes)
        throw std::runtime_error("Incorrect number of genotypes read from file " + filename);

    // Close the file
    file.close();

    // Check
    assert(!file.is_open());

}

// Function to throw mutations into the matrix of alleles
void gen::mutate(std::vector<std::bitset<64u> > &alleles, const double &mu, const size_t &N, const size_t &imode, const double &ratio) {

    // alleles: vector of bitsets representing matrix of alleles
    // mu: mutation rate
    // N: total number of alleles in the population
    // imode: sampling mode (1: "bernoulli", 2: "binomial", 3: "geometric", or 0: "given")
    // ratio: density of mutations below which to shuffle only partially

    // Convert sampling mode to string
    std::string mode;
    if (imode == 1u) mode = "bernoulli";
    else if (imode == 2u) mode = "binomial";
    else if (imode == 3u) mode = "geometric";
    else {

        // Default
        assert(imode == 0u);
        mode = "given";

    }

    // Note: We only convert for readability.

    // Number of bits per bitset
    const size_t n = 64u;

    // Exit if no mutations
    if (mu == 0.0) return;

    // If needed...
    if (mu == 1.0) {

        // Flip every allele
        for (size_t j = 0u; j < alleles.size(); ++j) alleles[j].flip();

        // Exit
        return;

    }

    // Depending on the sampling mode...
    if (mode == "bernoulli") {

        // For each allele...
        for (size_t i = 0u; i < N; ++i) {

            // Sample whether it mutates
            if (rnd::bernoulli(mu)(rnd::rng))
                alleles[i / n].flip(i % n);

        }

    } else if (mode == "geometric") {

        // If mutation rate is high...
        if (mu > 0.5) {

            // Flip all alleles first
            for (size_t j = 0u; j < alleles.size(); ++j)
                alleles[j].flip();
            
            // Note: In this case it is more efficient to sample
            // which alleles to flip back into a non-mutated state.

        }

        // Prepare a next mutation sampler
        rnd::geometric getnext(mu);

        // Sample first mutation
        size_t i = getnext(rnd::rng);

        // For as long as it takes...
        while (i < N) {

            // Flip the sampled position
            alleles[i / n].flip(i % n);

            // Sample the next one (avoid self)
            i += getnext(rnd::rng) + 1u;

        }

        // Check
        assert(i >= N);

    } else {

        // Number of mutations
        size_t nmut = floor(mu * N);

        // Randomly pick floor or ceiling
        nmut += rnd::bernoulli(0.5)(rnd::rng);

        // If needed...
        if (mode == "binomial") {

            // Override with binomial sampling
            nmut = rnd::binomial(N, mu)(rnd::rng);

        } else {

            // Note: We could also supply any mode other than "binomial",
            // "bernoulli" or "geometric" to get to here, but we want
            // to make sure that the number of options is fixed to be
            // able to catch typos.

            // Otherwise one possible choice left
            assert(mode == "given");

        }

        // Check
        assert(nmut <= N);

        // If the number of mutations is high...
        if (nmut > N / 2) {

            // Flip all alleles first
            for (size_t j = 0u; j < alleles.size(); ++j)
                alleles[j].flip();
            
            // Note: We will flip some back later.

            // Take the complement of the number of mutations to sample
            nmut = N - nmut;

        }

        // Create consecutive indices
        std::vector<size_t> indices(N);
        std::iota(indices.begin(), indices.end(), 0u);

        // If density is high enough...
        if (1.0 * nmut / N > ratio) {

            // Note: Make sure not to perform integer division when
            // checking this condition.

            // Full shuffle
            shuffle(indices.begin(), indices.end(), rnd::rng);

        } else {

            // Otherwise, loop through the first positions...
            for (int i = 0; i < nmut; ++i) {

                // Sample a random position to swap with
                const size_t j = rnd::random(i, N - 1u)(rnd::rng);

                // Swap the two positions
                std::swap(indices[i], indices[j]);

            }

            // Note: This partial shuffle is more efficient when the number
            // of mutations is small relative to the total number of alleles.

        }

        // Keep the sampled indices
        indices.resize(nmut);

        // Mutate the sampled positions
        for (size_t i : indices)
            alleles[i / n].flip(i % n);

    }
}

// Function to convert the matrix of alleles into a vector of trait values
std::vector<double> gen::develop(const std::vector<std::bitset<64u> > &alleles, const Parameters &pars, const Architecture &arch, const size_t &N) {

    // alleles: vector of bitsets representing matrix of alleles
    // pars: general hyperparameters
    // arch: genetic architecture
    // N: total number of alleles in the population

    // Number of bits per bitset
    const size_t n = 64u;

    // Get population size
    const size_t popsize = N / (2u * arch.nloci);

    // Total number of genotypes
    const size_t tloci = popsize * arch.nloci;

    // Total number of edges
    const size_t tedges = popsize * arch.nedges;

    // Total number of trait values
    const size_t ttraits = popsize * arch.ntraits;

    // Prepare to store gene expression values
    std::vector<double> expressions(tloci);

    // Prepare to store individual trait values
    std::vector<double> traits(ttraits);

    // For each locus in each individual...
    for (size_t i = 0u; i < tloci; ++i) {

        // Locus index
        const size_t locus = i % arch.nloci;

        // Which trait is affected?
        const size_t traitid = arch.traitids[locus];

        // Get the two alleles (stored next to each other)
        const size_t i1 = 2u * i;
        const size_t i2 = i1 + 1u;
        const bool allele1 = alleles[i1 / n].test(i1 % n);
        const bool allele2 = alleles[i2 / n].test(i2 % n);

        // Note: This assumes that each diploid locus is encoded as adjacent
        // alleles in the bit stream.

        // Get genotype from alleles
        const size_t genotype = allele1 + allele2;

        // Translate genotype into expression level (-1, 0, or +1)
        expressions[i] = genotype - 1.0;

        // Add dominance deviation for heterozygotes if needed
        expressions[i] += (genotype == 1u) * arch.dominances[locus] * pars.dominance[traitid];

        // Compute additive contribution to the phenotype
        const double value = expressions[i] * arch.effects[locus] * (1.0 - pars.epistasis[traitid]);
        
        // Individual index
        const size_t individual = i / arch.nloci;

        // Add to trait value
        traits[individual * arch.ntraits + traitid] += value;

        // Note: This assumes that the trait vector groups values by individual,
        // such that values encoding different traits for the same individual
        // are contiguous.

    }

    // For each edge in each individual...
    for (size_t i = 0u; i < tedges; ++i) {

        // Edge index
        const size_t edge = i % arch.nedges;

        // Which trait is affected?
        const size_t traitid = arch.traitids[arch.from[edge]];

        // Get indices of interacting loci
        const size_t from = arch.from[edge];
        const size_t to = arch.to[edge];

        // Individual index
        const size_t individual = i / arch.nedges;

        // Convert into indices for the current individual
        const size_t ifrom = individual * arch.nloci + from;
        const size_t ito = individual * arch.nloci + to;

        // Compute interaction contribution to the phenotype
        const double value = expressions[ifrom] * expressions[ito] * arch.weights[edge] * pars.epistasis[traitid];

        // Add to trait value
        traits[individual * arch.ntraits + traitid] += value;

    }

    // Prepare an environmental noise generator
    rnd::normal getnormal(0.0, 1.0);

    // For each trait value in each individual...
    for (size_t i = 0u; i < ttraits; ++i) {

        // Trait index
        const size_t traitid = i % arch.ntraits;

        // Add environmental noise
        traits[i] += getnormal(rnd::rng) * pars.envnoise[traitid];

    }

    // Exit
    return traits;

}

// Function to save trait values to file
void stf::saveTraits(const std::vector<double> &traits, const size_t &ntraits, const std::string &filename) {

    // traits: vector of trait values
    // ntraits: number of traits per individual
    // filename: name of the file to save

    // Check
    assert(traits.size() % ntraits == 0u);

    // Create output file stream
    std::ofstream file(filename);

    // Check that it is open
    if (!file.is_open())
        throw std::runtime_error("Unable to open file " + filename);

    // Header
    file << "id,";

    // For each trait...
    for (size_t j = 0u; j < ntraits; ++j) {

        // Write trait name to header
        file << "trait" << j + 1u;
        if (j < ntraits - 1u) file << ',';
        else file << '\n';

    }

    // Get number of individuals
    const size_t popsize = traits.size() / ntraits;

    // For each trait in each individual...
    for (size_t i = 0u; i < ntraits * popsize; ++i) {

        // Write individual identifier to file
        if (i % ntraits == 0u) file << i / ntraits + 1u << ',';

        // Write trait value to file
        file << traits[i];

        // Right separator
        if (i % ntraits == ntraits - 1u) file << '\n';
        else file << ',';
            
    }

    // Note: Each row is an individual, each column a trait.

    // Close the file
    file.close();

    // Check
    assert(!file.is_open());
}

// Function to save matrix of alleles to file
void stf::saveAlleles(std::vector<std::bitset<64u> > &alleles, const size_t &popsize, const size_t &nloci, const std::string &filename, const bool &binary) {

    // alleles: vector of bitsets representing matrix of alleles
    // popsize: total number of individuals in the population
    // nloci: number of loci in the genome
    // filename: name of the file to save
    // binary: whether to save in binary (if not, CSV)

    // Number of bits per bitset
    const size_t n = 64u;

    // Total number of alleles in the population
    const size_t N = popsize * nloci * 2u;

    // Make sure that the trailing bits are zeros
    for (size_t i = 0u; i < N % n; ++i)
        alleles.back().reset(i);

    // Create output file stream
    std::ofstream file;

    // Open the file in the right mode
    if (binary) file.open(filename, std::ios::binary);
    else file.open(filename);

    // Check that it is open    
    if (!file.is_open())
        throw std::runtime_error("Unable to open file " + filename);

    // If saving in binary...
    if (binary) {

        // For each bitset...
        for (size_t j = 0u; j < alleles.size(); ++j) {

            // Convert it to a binary number
            const size_t value = alleles[j].to_ulong();

            // Note: Each 64-bit bitset will be fully converted to
            // a 64-bit number on a 64-bit system, but will overflow
            // on a 32-bit system.

            // Save that number to the file
            file << value;

        }

        // Note: Each bitset (chunk of 64 alleles) is saved as a binary number.
        // The file is in written in binary. Make sure to read it back on a
        // bit-by-bit basis, to be able to analyze the outcome.

    } else {

        // First write a header
        file << "id,";

        // For each locus...
        for (size_t i = 0u; i < nloci; ++i) {

            // Add to the header
            file << "locus" << i + 1u;
            if (i < nloci - 1u) file << ',';
            else file << '\n';

        }

        // Note: If saving as CSV, we save the combined genotype (0, 1 or 2),
        // not the alleles separately.

        // For each locus in each individual...
        for (size_t i = 0u; i < nloci * popsize; ++i) {

            // Write individual identifier to file
            if (i % nloci == 0u) file << i / nloci + 1u << ',';

            // Find the two haplotypes in the bitsets
            const size_t j = i / 2u;
            const size_t k = j + 1u;

            // Extract alleles
            const bool a = alleles[j / n].test(j % n);
            const bool b = alleles[k / n].test(k % n);

            // Check
            assert(a + b <= 2u);

            // Write combined genotype to file
            file << a + b;

            // Right separator
            if (i % nloci == nloci - 1u) file << '\n';
            else file << ',';
            
        }
    }

    // Close the file
    file.close();

    // Check
    assert(!file.is_open());
    
}

// Function to add replicate to file name
std::string addrepl(std::string filename, const std::string &ext, const size_t &k, const bool &ison, const char &sep = '_') {

    // filename: base name of the file
    // ext: file extension
    // k: replicate number
    // ison: whether to include the replicate number
    // sep: separator between filename and replicate number

    // Add replicate if needd
    if (ison) filename += sep + std::to_string(k + 1u);

    // Add extension
    filename += '.' + ext;

    // Exit
    return filename;

}

// Main function
void doMain(const std::vector<std::string> &args) {

    // args: program arguments

    // Error if two many arguments
    if (args.size() > 2u) 
        throw std::runtime_error("Too many arguments provided");

    // Parameter file
    const std::string parfile = args.size() == 2u ? args[1u] : "";

    // Create parameters (from file if needed)
    Parameters pars(parfile);

    // Check
    pars.check();

    // Save the parameters if needed
    if (pars.savepars) pars.save("paramlog.txt");

    // Verbose if needed
    if (args.size() == 2u) std::cout << "Parameters read in succesfully\n";

    // Seed the random number generator
    rnd::rng.seed(pars.seed);

    // Create a simple genetic architecture or read from file if needed
    Architecture arch(pars.loadarch ? "architecture.txt" : "");

    // Check that the architecture is compatible with the parameters
    if (pars.loadarch) arch.test(pars);

    // For each replicate...
    for (size_t k = 0u; k < pars.nrepl; ++k) {

        // Simulate a (complicated) genetic architecture if needed
        if (!pars.loadarch) arch.generate(pars);

        // If needed...
        if (pars.verbose) {
            
            // Verbose
            std::cout << "Replicate " << k + 1u << " of " << pars.nrepl << '\n';
            std::cout << "Genetic architecture ";
            std::cout << (pars.loadarch ? "read in" : "generated");
            std::cout << " successfully\n";

        }

        // Current parameters
        Parameters parsk = pars;

        // Override general parameters if needed
        parsk.override(arch);

        // Check
        arch.check();
        parsk.check();

        // Output file name
        const std::string archfile = addrepl("architecture", "txt", k, pars.nrepl > 1u);

        // Save the architecture if needed
        if (pars.savearch) arch.save(archfile);

        // Total number of bits needed
        const size_t N = pars.popsize * pars.nloci * 2u;

        // Number of bitsets needed
        const size_t n = N / 64u;

        // Create a vector of bitsets
        std::vector<std::bitset<64u> > alleles(n + 1u);

        // Note: The size of a bitset must be hard-coded in C++.
        
        // Import matrix of alleles if needed
        if (pars.import) gen::import(alleles, "genotypes.csv", N);

        // Throw mutations
        gen::mutate(alleles, pars.mutation, N, pars.sampling, pars.ratio);

        // Develop genotypes into phenotypes
        const std::vector<double> traits = gen::develop(alleles, pars, arch, N);

        // Output file name
        const std::string traitfile = addrepl("traits", "csv", k, pars.nrepl > 1u);

        // Save trait values to file
        stf::saveTraits(traits, pars.ntraits, traitfile);
        
        // Output file name
        const std::string genfile = addrepl("genotypes", "csv", k, pars.nrepl > 1u);
        const std::string allfile = addrepl("alleles", "dat", k, pars.nrepl > 1u);

        // Save matrix of alleles if needed
        stf::saveAlleles(alleles, pars.popsize, pars.nloci, pars.binary ? allfile : genfile, pars.binary);
        
        // Verbose if needed
        if (pars.verbose) std::cout << "Population generated successfully\n";

    }
}