// This is the implementation of the Parameters class.

#include "parameters.hpp"
#include "readpars.hpp"
#include "architecture.hpp"
#include "checker.hpp"
#include <chrono>

// Create a default seed based on clock
size_t clockseed() {

    // Use the current time to generate a seed
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    // Convert the seed to integer and return
    return static_cast<size_t>(seed);
}

// Constructor
Parameters::Parameters(const std::string& filename) :
    popsize(10u),
    allfreq(0.0),
    effect(0.0),
    weight(0.0),
    ntraits(1u),
    nlocipertrait(ntraits, nloci),
    nedgespertrait(ntraits, nedges),
    skews(ntraits, 1.0),
    epistasis(ntraits, 0.0),
    dominance(ntraits, 0.0),
    envnoise(ntraits, 0.0),
    sampling(0u),
    ratio(0.25),
    seed(clockseed()),
    loadarch(false),
    savearch(true),
    savepars(true),
    binary(false),
    verbose(true),
    nloci(0u),
    nedges(0u)
{

    // filename: optional parameter input file

    // Update internals
    update();

    // Read from file if needed
    if (filename != "") read(filename);

    // Check
    check();

}

// Function to update internal parameters
void Parameters::update() {

    // Reset
    nloci = 0u;
    nedges = 0u;

    // Count
    for (size_t n : nlocipertrait) nloci += n;
    for (size_t n : nedgespertrait) nedges += n;

}

// Function to read parameters from a file
void Parameters::read(const std::string &filename) {

    // filename: name of the input file

    // Create a reader
    ReadPars reader(filename);

    // Open the file
    reader.open();

    // For each line in the file...
    while (!reader.iseof()) {

        // Read a line
        reader.readline();

        // Skip empty line
        if (reader.isempty()) continue;

        // Skip if comment line
        if (reader.iscomment()) continue;

        // Check
        assert(!reader.isempty());
        assert(!reader.iscomment());

        // Current parameter name
        std::string name = reader.getname();

        // Read the parameter value(s)
        if (name == "popsize") reader.readvalue<size_t>(popsize, chk::strictpos<size_t>);
        else if (name == "allfreq") reader.readvalue<double>(allfreq, chk::proportion<double>);
        else if (name == "effect") reader.readvalue<double>(effect, chk::positive<double>);
        else if (name == "weight") reader.readvalue<double>(weight, chk::positive<double>);
        else if (name == "ntraits") reader. readvalue<size_t>(ntraits, chk::strictpos<size_t>);
        else if (name == "nlocipertrait") reader.readvalues<size_t>(nlocipertrait, ntraits, chk::strictpos<size_t>);
        else if (name == "nedgespertrait") reader.readvalues<size_t>(nedgespertrait, ntraits);
        else if (name == "skews") reader.readvalues<double>(skews, ntraits);
        else if (name == "epistasis") reader.readvalues<double>(epistasis, ntraits, chk::proportion<double>);
        else if (name == "dominance") reader.readvalues<double>(dominance, ntraits, chk::positive<double>);
        else if (name == "envnoise") reader.readvalues<double>(envnoise, ntraits, chk::positive<double>);
        else if (name == "sampling") reader.readvalue<size_t>(sampling, chk::zerotothree<size_t>);
        else if (name == "ratio") reader.readvalue<double>(ratio, chk::proportion<double>);
        else if (name == "seed") reader.readvalue<size_t>(seed);
        else if (name == "loadarch") reader.readvalue<bool>(loadarch);
        else if (name == "savearch") reader.readvalue<bool>(savearch);
        else if (name == "savepars") reader.readvalue<bool>(savepars);
        else if (name == "binary") reader.readvalue<bool>(binary);
        else if (name == "verbose") reader.readvalue<bool>(verbose);
        else
            reader.readerror();

        // Check that we have reached the end of the line
        assert(reader.iseol());

    }

    // Check that we have reached the end of the file
    assert(reader.iseof());

    // Close the file
    reader.close();

    // Update internals
    update();

    // Note: Here the number of traits cannot be greater than the number
    // of loci, so no need to check for that.

    // For each trait...
    for (size_t i = 0u; i < ntraits; ++i) {

         // Check that fewer edges than a full graph allows
        if (nedgespertrait[i] > (nlocipertrait[i] * (nlocipertrait[i] - 1u)) / 2u) 
            throw std::runtime_error("Too many edges for the number of loci for trait " + std::to_string(i + 1u));

        // Note: This does not guarantee that all edges will be made. See the implementation
        // the genetic architecture for more details.

    }

    // Check
    check();
    
}

// Function to override parameters with those from the architecture
void Parameters::override(const Architecture &arch) {

    // arch: architecture to override parameters with

    // Override key hyperparameters
    nloci = arch.nloci;
    nedges = arch.nedges;
    ntraits = arch.ntraits;
    nlocipertrait = arch.nlocipertrait;
    nedgespertrait = arch.nedgespertrait;

    // Check
    check();

}

// Function to check the consistency of internal parameters
void Parameters::checkinternal() const {

    // Prepare
    size_t nl = 0u;
    size_t ne = 0u;

    // For each trait...
    for (size_t i = 0u; i < ntraits; ++i) {

        // Count
        nl += nlocipertrait[i];
        ne += nedgespertrait[i];
        
    }

    // Check
    assert(nl == nloci);
    assert(ne == nedges);

}

// Function to check that parameters are valid
void Parameters::check() const {

    // Check internal consistency
    checkinternal();

    // Check validity
    assert(popsize > 0u);
    assert(ntraits > 0u);
    assert(ntraits <= nloci);
    assert(allfreq >= 0.0 && allfreq <= 1.0);
    assert(effect >= 0.0);
    assert(weight >= 0.0);
    assert(nlocipertrait.size() == ntraits);
    assert(nedgespertrait.size() == ntraits);
    assert(skews.size() == ntraits);
    assert(epistasis.size() == ntraits);
    assert(dominance.size() == ntraits);
    assert(envnoise.size() == ntraits);
    assert(sampling < 4u);
    assert(ratio >= 0.0 && ratio <= 1.0);

    // Vectors
    for (size_t i : nlocipertrait) assert(i > 0u);
    for (double x : epistasis) assert(x >= 0.0 && x <= 1.0);
    for (double x : dominance) assert(x >= 0.0);
    for (double x : envnoise) assert(x >= 0.0);

    // For each trait...
    for (size_t i = 0u; i < ntraits; ++i) {

        // Check
        assert(nedgespertrait[i] <= nlocipertrait[i] * (nlocipertrait[i] - 1u) / 2u);

    }
}

// Function to save parameters to a file
void Parameters::save(const std::string &filename) const {

    // filename: name of the output file

    // Create output file stream
    std::ofstream file(filename);

    // Check that it is open
    if (!file.is_open()) 
        throw std::runtime_error("Unable to open file " + filename);

    // Write parameters to the file
    file << "popsize " << popsize << '\n';    
    file << "allfreq " << allfreq << '\n';
    file << "effect " << effect << '\n';
    file << "ntraits " << ntraits << '\n';
    file << "nlocipertrait";
    for (size_t i : nlocipertrait) file << ' ' << i;
    file << '\n';
    file << "nedgespertrait";
    for (size_t i : nedgespertrait) file << ' ' << i;
    file << '\n';
    file << "skews";
    for (double x : skews) file << ' ' << x;
    file << '\n';
    file << "epistasis";
    for (double x : epistasis) file << ' ' << x;
    file << '\n';
    file << "dominance";
    for (double x : dominance) file << ' ' << x;
    file << '\n';
    file << "envnoise";
    for (double x : envnoise) file << ' ' << x;
    file << '\n';    
    file << "sampling " << sampling << '\n';
    file << "ratio " << ratio << '\n';
    file << "seed " << seed << '\n';
    file << "loadarch " << loadarch << '\n';
    file << "savearch " << savearch << '\n';
    file << "savepars " << savepars << '\n';
    file << "binary " << binary << '\n';
    file << "verbose " << verbose << '\n';

    // Close the file
    file.close();

    // Check
    assert(!file.is_open());

}