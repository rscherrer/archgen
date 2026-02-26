// This is the implementation for the Architecture class.

#include "architecture.hpp"
#include "readpars.hpp"
#include "checker.hpp"
#include "parameters.hpp"
#include "random.hpp"

// Constructor
Architecture::Architecture(const std::string& archfile) :
    nloci(10u),
    nedges(0u),
    ntraits(1u),
    traitids(nloci, 0u),
    effects(nloci, 0.0),
    dominances(nloci, 0.0),
    from(nedges, 0u),
    to(nedges, 0u),
    weights(nedges, 0.0),
    nlocipertrait(ntraits, nloci),
    nedgespertrait(ntraits, nedges)
{

    // archfile: (optional) name of the file to read from

    // Note: Keep the default to no edges, so that the user can stick to
    // that by not supplying any in the architecture file.
    
    // Either read of generate an architecture
    if (archfile != "") read(archfile);

    // Check
    check();

}

// Function to read a genetic architecture from a file
void Architecture::read(const std::string& filename) {

    // filename: name of the file to read from

    // Create a reader
    ReadPars reader(filename);

    // Open it
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
        if (name == "nloci") reader.readvalue<size_t>(nloci, chk::strictpos<size_t>);
        else if (name == "nedges") reader.readvalue<size_t>(nedges);
        else if (name == "ntraits") reader.readvalue<size_t>(ntraits, chk::strictpos<size_t>);
        else if (name == "traitids") reader.readvalues<size_t>(traitids, nloci, chk::strictpos<size_t>);
        else if (name == "effects") reader.readvalues<double>(effects, nloci);
        else if (name == "dominances") reader.readvalues<double>(dominances, nloci);
        else if (name == "from") reader.readvalues<size_t>(from, nedges, chk::strictpos<size_t>);
        else if (name == "to") reader.readvalues<size_t>(to, nedges, chk::strictpos<size_t>);
        else if (name == "weights") reader.readvalues<double>(weights, nedges);
        else
            reader.readerror();

        // Note: Edge-specific parameters may be absent from the architecture file,
        // which will stand for no edges. All other parameters must be supplied,
        // and their dimensions must be consistent with the hyperparameters.

        // Check that we have reached the end of the line
        assert(reader.iseol());
 
    }

    // Check that we have reached the end of the file
    assert(reader.iseof());

    // Close the file
    reader.close();

    // Check more specific features of the parameters
    if (ntraits > nloci) throw std::runtime_error("Too many traits for the number of loci in file " + filename);

    // Check that all vectors have the right size
    if (traitids.size() != nloci) throw std::runtime_error("Number of encoded traits does not match number of loci in file " + filename);
    if (effects.size() != nloci) throw std::runtime_error("Number of effects does not match number of loci in file " + filename);
    if (dominances.size() != nloci) throw std::runtime_error("Number of dominance effects does not match number of loci in file " + filename);
    if (from.size() != nedges) throw std::runtime_error("Number of start loci does not match number of edges in file " + filename);
    if (to.size() != nedges) throw std::runtime_error("Number of end loci does not match number of edges in file " + filename);
    if (weights.size() != nedges) throw std::runtime_error("Number of interaction weights does not match number of edges in file " + filename);

    // Note: At this point the edge-specific parameters may have been omitted, in
    // which case they will remain empty. The above makes sure that if one is empty,
    // they all are.

    // Prepare to count numbers of loci and edges per trait
    nlocipertrait.assign(ntraits, 0u);
    nedgespertrait.assign(ntraits, 0u);

    // For each locus...
    for (size_t i = 0u; i < nloci; ++i) {

        // Make sure encoded trait is valid
        if (traitids[i] > ntraits) 
            throw std::runtime_error("Encoded trait " + std::to_string(traitids[i]) + " of locus " + std::to_string(i) + " is out of bounds in file " + filename);

        // Decrement
        --traitids[i];

        // Note: C++ is zero-based but users are expected to provide one-based indices.
        // We just decrement internally.

        // Count the number of loci per trait
        ++nlocipertrait[traitids[i]];

    }

    // For each edge...
    for (size_t i = 0u; i < nedges; ++i) {

        // Check start locus
        if (from[i] > nloci) 
            throw std::runtime_error("Start locus " + std::to_string(from[i]) + " of edge " + std::to_string(i + 1u) + " is out of bounds in file " + filename);
            
        // Check end locus
        if (to[i] > nloci) 
            throw std::runtime_error("End locus " + std::to_string(to[i]) + " of edge " + std::to_string(i + 1u) + " is out of bounds in file " + filename);
            
        // Check that they are different
        if (from[i] == to[i]) 
            throw std::runtime_error("Start and end loci of edge " + std::to_string(i + 1u) + " are the same in file " + filename);

        // Decrement
        --from[i];
        --to[i];

        // Check that they affect the same trait
        if (traitids[from[i]] != traitids[to[i]]) 
            throw std::runtime_error("Start and end loci of edge " + std::to_string(i + 1u) + " affect different traits in file " + filename);
       
        // Count the number of edges per trait
        ++nedgespertrait[traitids[from[i]]];

    }

    // Note: We do not need to check that the number of loci for a given trait is at least two whenever
    // some edges must be made, because this should have been caught by other checks (edge between loci
    // coding for the wrong trait or loci that do not exist). We also do not need to check that the
    // number of edges is not higher than the maximum possible for a given number of loci, because
    // this should also have been caught.

    // Check
    check();

}

// Function to generate a genetic architecture
void Architecture::generate(const Parameters &pars) {

    // pars: general hyperparameters

    // Override hyperparameters
    nloci = pars.nloci;
    nedges = pars.nedges;
    ntraits = pars.ntraits;
    nlocipertrait = pars.nlocipertrait;
    nedgespertrait = pars.nedgespertrait;

    // Reset
    traitids.resize(0u);
    effects.resize(0u);
    dominances.resize(0u);
    from.resize(0u);
    to.resize(0u);
    weights.resize(0u);

    // Reserve locus-wise vectors
    traitids.reserve(0u);
    effects.reserve(nloci);
    dominances.reserve(nloci);

    // Reserve edge-wise vectors
    from.reserve(nedges);
    to.reserve(nedges);
    weights.reserve(nedges);

    // Prepare a distribution to sample from
    rnd::normal getnormal(0.0, 1.0);

    // Counter
    size_t trait = 0u;

    // For each locus...
    for (size_t i = 0u; i < nloci; ++i) {

        // Update trait conter
        if (i > 0u && i % nlocipertrait[trait] == 0u) ++trait;

        // Trait affected by the locus
        traitids.push_back(trait);

        // Additive effect size of the locus on the trait
        effects.push_back(getnormal(rnd::rng) * pars.effect);

        // Dominance effect of the locus on the trait
        dominances.push_back(getnormal(rnd::rng));

        // Note: The dominance scaling parameter can be used to modify the
        // magnitued of dominance deviations for each trait separately.

    }

    // Shuffle encoded traits randomly
    std::shuffle(traitids.begin(), traitids.end(), rnd::rng);

    // If there are edges to add...
    if (nedges > 0u) {

        // Note: This section implements a preferential attachment 
        // algorithm by Barabasi and Albert (1999).

        // For each trait...
        for (size_t j = 0u; j < ntraits; ++j) {

            // Skip if no edges affect the trait
            if (nedgespertrait[j] == 0u) continue;

            // Initialize vertex connections
            std::vector<size_t> vfrom({0u});
            std::vector<size_t> vto({1u});

            // Note: This connects vertex 0 to vertex 1.

            // Initialize vector of degrees
            std::vector<size_t> degrees(nlocipertrait[j], 0u);

            // Update degrees with first connection
            ++degrees[0u];
            ++degrees[1u];

            // Reserve memory
            vfrom.reserve(nedgespertrait[j]);
            vto.reserve(nedgespertrait[j]);

            // Number of edges left to add
            size_t ne = nedgespertrait[j] - 1u;

            // Number of loci still to graft
            size_t nl = nlocipertrait[j] - 2u;

            // For each remaining vertex...
            for (size_t i = 2u; i < nlocipertrait[j]; ++i) {

                // Is it the last?
                const bool islast = i == nlocipertrait[j] - 1u;

                // Sample a number of connections to make or make them all if last
                const size_t n = islast ? ne : rnd::binomial(ne, 1.0 / nl)(rnd::rng);

                // Note: This makes sure that the network is connected. No nodes
                // should be left unattached.

                // Initialize a vector of probabilities
                std::vector<double> probs(i, 0.0);

                // Note: Only vertices already attached are counted.

                // Compute attachment probabilities
                for (size_t k = 0u; k < probs.size(); ++k)
                    probs[k] = pars.skews[j] == 1.0 ? degrees[k] : pow(degrees[k], pars.skews[j]);

                // Note: Probabilities are proportional to the degree of the vertex
                // to a certain power, which will determines the skewness of the degree
                // distribution. However, the pow() function is very slow, so we try
                // to avoid it if the skewness is 1.0, which is a common case.

                // For each connection to make...
                for (size_t e = 0u; e < n; ++e) {

                    // Sample a vertex to connect to
                    const size_t v = rnd::discrete(probs.cbegin(), probs.cend())(rnd::rng);

                    // Check
                    assert(v < i);

                    // Add the connection
                    vfrom.push_back(v);
                    vto.push_back(i);

                    // Update degrees
                    ++degrees[v];
                    ++degrees[i];

                    // Update probabilities to avoid sampling the same vertex again
                    probs[v] = 0.0;

                }

                // Update number of edges and loci still to graft
                ne -= n;
                --nl;

            }

            // Check
            assert(vfrom.size() <= vfrom.capacity());

            // Error if needed
            if (vfrom.size() < vfrom.capacity())
                throw std::runtime_error("Not enough edges could be made for trait " + std::to_string(j));

            // Note: This is because by construction this algorithm cannot
            // guarantee that the requested number of edges will be reached.
            // This error may trigger if the number of edges is very high.

            // Check
            assert(ne == 0u);
            assert(vfrom.size() == vto.size());

            // Prepare to store indices of the loci affecting the current trait
            std::vector<size_t> loci(nlocipertrait[j]);

            // For each locus...
            for (size_t i = 0u, k = 0u; i < nloci; ++i) {

                // Skip if not coding the right trait
                if (traitids[i] != j) continue;

                // Otherwise record
                loci[k] = i;

                // Increment
                ++k;

            }

            // While edges to store...
            while (!vfrom.empty()) {

                // Store
                from.push_back(loci[vfrom.back()]);
                to.push_back(loci[vto.back()]);

                // Delete
                vfrom.pop_back();
                vto.pop_back();

                // Sample interaction weight while we are at it
                weights.push_back(getnormal(rnd::rng) * pars.weight);

            }

            // Check
            assert(vfrom.empty());
            assert(vto.empty());

        }
    }
    
    // Check
    check();

}

// Function to check the consistency of internal parameters
void Architecture::checkinternal() const {

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

// Function to check the validity of the architecture
void Architecture::check() const {

    // Check internal consistency
    checkinternal();

    // Check
    assert(nloci > 0u);
    assert(ntraits > 0u);

    // Check sizes
    assert(traitids.size() == nloci);
    assert(effects.size() == nloci);
    assert(dominances.size() == nloci);
    assert(from.size() == nedges);
    assert(to.size() == nedges);
    assert(weights.size() == nedges);

    // Note: By now any user-supplied indices have been decremented
    // to match the zero-based indexing of C++. 

    // For each locus...
    for (size_t i = 0u; i < nloci; ++i) {

        // Check
        assert(traitids[i] < ntraits);

    }

    // For each edge...
    for (size_t i = 0u; i < nedges; ++i) {

        // Check
        assert(from[i] < nloci);
        assert(to[i] < nloci);
        assert(from[i] != to[i]);
        assert(traitids[from[i]] == traitids[to[i]]);

    }
}

// Function to save architecture parameters to file
void Architecture::save(const std::string &filename) const {

    // filename: name of the output file

    // Create output file stream
    std::ofstream file(filename);

    // Check that it is open
    if (!file.is_open()) 
        throw std::runtime_error("Unable to open file " + filename);

    // Write parameters to the file
    file << "nloci " << nloci << '\n';
    file << "nedges " << nedges << '\n';
    file << "ntraits " << ntraits << '\n';
    file << "traitids";
    for (size_t x : traitids) file << ' ' << x + 1u;
    file << '\n';
    file << "effects";
    for (double x : effects) file << ' ' << x;
    file << '\n';
    file << "dominances";
    for (double x : dominances) file << ' ' << x;
    file << '\n';

    // If edges...
    if (nedges > 0u) {

        // Write them too
        file << "from";
        for (size_t x : from) file << ' ' << x + 1u;
        file << '\n';
        file << "to";
        for (size_t x : to) file << ' ' << x + 1u;
        file << '\n';
        file << "weights";
        for (double x : weights) file << ' ' << x;
        file << '\n';

    }

    // Close the file
    file.close();

    // Check
    assert(!file.is_open());

}