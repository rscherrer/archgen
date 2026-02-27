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

    // Reserve memory
    traitids.reserve(nloci);
    effects.reserve(nloci);
    dominances.reserve(nloci);
    from.reserve(nedges);
    to.reserve(nedges);
    weights.reserve(nedges);

    // Prepare a distribution to sample from
    rnd::normal getnormal(0.0, 1.0);

    // Counter
    size_t trait = 0u;

    // Tracker for the next trait
    size_t nextn = nlocipertrait[trait];

    // For each locus...
    for (size_t i = 0u; i < nloci; ++i) {

        // Update trait counter
        if (i == nextn) {

            // Move to next trait
            ++trait;

            // Update trait tracker
            nextn += nlocipertrait[trait];

        }

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

    // Prepare to store indices of the loci affecting each trait
    std::vector<std::vector<size_t> > indices(ntraits);

    // Note: These will be useful later.
    
    // Reserve memory
    for (size_t j = 0u; j < ntraits; ++j) 
        indices[j].reserve(nlocipertrait[j]);

    // Collect indices for each trait
    for (size_t i = 0u; i < nloci; ++i) {
        indices[traitids[i]].push_back(i);
    }

    // Note: What follows implements a modified version of the Barabasi-Albert 
    // preferential attachment algorithm. In this version the number of
    // connections made by each new vertex is not fixed but randomly sampled, so
    // that the mean degree at the end is the number of edges divided by the number
    // of vertices in each network.

    // For each trait...
    for (size_t j = 0u; j < ntraits; ++j) {

        // Useful numbers
        const size_t nL = nlocipertrait[j];
        const size_t nE = nedgespertrait[j];
        const double skew = pars.skews[j];

        // Skip if no edges affect the trait
        if (nE == 0u) continue;

         // Check
        assert(nL > 1u);
        assert(nE >= nL - 1u);
        assert(nE <= nL * (nL - 1u) / 2u);

        // First connection
        from.push_back(indices[j][0u]);
        to.push_back(indices[j][1u]);

        // Note: This connects vertex 0 to vertex 1.

        // Sample first interaction weight
        weights.push_back(getnormal(rnd::rng) * pars.weight);

        // Initialize vector of degrees across vertices
        std::vector<size_t> degrees(nL, 0u);

        // Update degrees with first connection
        ++degrees[0u];
        ++degrees[1u];

        // Initialize a vector of partnering probabilities
        std::vector<double> probs(2u, 1.0);

        // Note: The probability is to the power determined by the
        // skewness parameter but it remains one for now because
        // degrees of the two first vertices are one.

        // All other vertices get probability zero for now
        probs.resize(nL);

        // Number of edges left to add
        size_t ne = nE - 1u;

        // Number of loci still to graft
        size_t nl = nL - 2u;

        // For each remaining vertex...
        for (size_t i = 2u; i < nL; ++i) {

            // Number of edges to spare
            const size_t nspare = ne - nl;

            // Note: One edge will be made for sure, but there may be more, and the number
            // of extra edges depends on how many edges we have to spare (we are reserving
            // at least one for each vertex still to come to make sure the network is connected).

            // Sample a number of connections to make or make them all if last
            size_t n = nl == 0u ? ne : 1u + rnd::binomial(nspare, 1.0 / nl)(rnd::rng);

            // Note: This sampling makes the expected number of connections made by each
            // vertex increase as we loop through vertices (contrary to the classic
            // Barabasi-Albert algorithm where that number is fixed). The last vertex
            // makes all the remaining edges.

            // Cap the number of connections if not enough partners
            n = (n > i) ? i : n;

            // Check
            assert(n > 0u);
            assert(n <= i);

            // Vector of probabilities from the point of view of the current vertex
            std::vector<double> iprobs(probs.begin(), probs.begin() + i);

            // Note: This one will be updated to avoid connecting the current
            // vertex to the same partner twice. We also just need the probabilities
            // the vertices already attached; the rest have zero probability.

            // Flag for whether no sampling needed
            const bool nosample = (n == i);

            // For each connection to make...
            while (n > 0u) {

                // Sample a vertex to connect to
                const size_t v = nosample ? i - 1u : rnd::discrete(iprobs.cbegin(), iprobs.cend())(rnd::rng);

                // Note: This make sure that we avoid expensive sampling in case
                // we should anyway connect to all the vertices that came before.

                // Check
                assert(v < i);

                // Add the connection
                from.push_back(indices[j][i]);
                to.push_back(indices[j][v]);

                // Update degrees
                ++degrees[i];
                ++degrees[v];

                // Update probabilities to avoid sampling the same vertex again
                iprobs[v] = 0.0;

                // Update external probability of the partner vertex               
                probs[v] = skew == 1.0 ? degrees[v] : pow(degrees[v], skew);

                // Note: We try to avoid regenerating entire vectors of probabilities
                // too often because the power function is expensive. Here the fast
                // special case when skewness is one.

                // Sample an interaction weight
                weights.push_back(getnormal(rnd::rng) * pars.weight);

                // Decrement the number of connections left to make
                --n;
                --ne;

            }

            // Update external probability of the focal vertex
            probs[i] = skew == 1.0 ? degrees[i] : pow(degrees[i], skew);

            // Update number of vertices yet to graft
            --nl;

        }

        // Error if needed
        if (ne > 0u)
            throw std::runtime_error("Not all requested edges could be made for trait " + std::to_string(j + 1u) + " with the given parameters");

        // Note: This is because by construction this algorithm cannot
        // guarantee that the requested number of edges will be reached.
        // This error may trigger if the number of edges is very high.
        
        // Check
        assert(ne == 0u);

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

    // Prepare to verify number of loci and edges for each trait
    std::vector<size_t> nl(ntraits, 0u);
    std::vector<size_t> ne(ntraits, 0u);

    // For each locus...
    for (size_t i = 0u; i < nloci; ++i) {

        // Check
        assert(traitids[i] < ntraits);

        // Count
        ++nl[traitids[i]];

    }

    // For each edge...
    for (size_t i = 0u; i < nedges; ++i) {

        // Check
        assert(from[i] < nloci);
        assert(to[i] < nloci);
        assert(from[i] != to[i]);
        assert(traitids[from[i]] == traitids[to[i]]);

        // Count
        ++ne[traitids[from[i]]];

    }

    // For each trait...
    for (size_t i = 0u; i < ntraits; ++i) {

        // Check
        assert(nl[i] == nlocipertrait[i]);
        assert(ne[i] == nedgespertrait[i]);

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