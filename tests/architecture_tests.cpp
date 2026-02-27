#define BOOST_TEST_DYNAMIC_LINK
#define BOOST_TEST_MODULE Main

// Here we test the proper behavior of the Architecture class.

#include "testutils.hpp"
#include "../src/architecture.hpp"
#include "../src/parameters.hpp"
#include <boost/test/unit_test.hpp>

// Test that architecture can be created
BOOST_AUTO_TEST_CASE(architectureCreated) {

    // Without input file should work fine
    Architecture arch;

    // Check a few default values
    BOOST_CHECK_EQUAL(arch.nloci, 10u);
    BOOST_CHECK_EQUAL(arch.nedges, 0u);
    BOOST_CHECK_EQUAL(arch.ntraits, 1u);

}

// Test that architecture can be read from a file
BOOST_AUTO_TEST_CASE(readArchitecture) {

    // Write a file with an architecture
    std::ostringstream content;
    content << "nloci 5\n";
    content << "nedges 2\n";
    content << "ntraits 2\n";
    content << "traitids 1 1 2 2 2\n";
    content << "effects 0.1 0.2 0.3 0.4 0.5\n";
    content << "dominances 0.01 0.02 0.03 0.04 0.05\n";
    content << "from 1 3\n";
    content << "to 2 4\n";
    content << "weights 0.5 0.6\n";
    tst::write("architecture.txt", content.str());

    // Read the architecture
    Architecture arch("architecture.txt");

    // Check that the parameters have been updated
    BOOST_CHECK_EQUAL(arch.nloci, 5u);
    BOOST_CHECK_EQUAL(arch.nedges, 2u);
    BOOST_CHECK_EQUAL(arch.ntraits, 2u);
    BOOST_CHECK_EQUAL(arch.traitids[0], 0u);
    BOOST_CHECK_EQUAL(arch.traitids[1], 0u);
    BOOST_CHECK_EQUAL(arch.traitids[2], 1u);
    BOOST_CHECK_EQUAL(arch.traitids[3], 1u);
    BOOST_CHECK_EQUAL(arch.traitids[4], 1u);
    BOOST_CHECK_CLOSE(arch.effects[0], 0.1, 1e-6);
    BOOST_CHECK_CLOSE(arch.effects[1], 0.2, 1e-6);
    BOOST_CHECK_CLOSE(arch.effects[2], 0.3, 1e-6);
    BOOST_CHECK_CLOSE(arch.effects[3], 0.4, 1e-6);
    BOOST_CHECK_CLOSE(arch.effects[4], 0.5, 1e-6);
    BOOST_CHECK_CLOSE(arch.dominances[0], 0.01, 1e-6);
    BOOST_CHECK_CLOSE(arch.dominances[1], 0.02, 1e-6);
    BOOST_CHECK_CLOSE(arch.dominances[2], 0.03, 1e-6);
    BOOST_CHECK_CLOSE(arch.dominances[3], 0.04, 1e-6);
    BOOST_CHECK_CLOSE(arch.dominances[4], 0.05, 1e-6);
    BOOST_CHECK_EQUAL(arch.from[0], 0u);
    BOOST_CHECK_EQUAL(arch.from[1], 2u);
    BOOST_CHECK_EQUAL(arch.to[0], 1u);
    BOOST_CHECK_EQUAL(arch.to[1], 3u);
    BOOST_CHECK_CLOSE(arch.weights[0], 0.5, 1e-6);
    BOOST_CHECK_CLOSE(arch.weights[1], 0.6, 1e-6);

    // Remove file
    std::remove("architecture.txt");

}

// Test that architecture reading fails when invalid parameters are provided
BOOST_AUTO_TEST_CASE(readInvalidArchitecture) {

    // Write a file with invalid architecture parameters
    std::ostringstream content;
    content << "nloci 5\n";
    content << "nedges 2\n";
    content << "ntraits 2\n";
    content << "invalid 0 0 1 1 2\n";
    tst::write("architecture.txt", content.str());

    // Check
    tst::checkError([&]() { Architecture arch("architecture.txt"); }, "Invalid parameter: invalid in line 4 of file architecture.txt");

    // Remove file
    std::remove("architecture.txt");

}

// Test that error when invalid number of loci
BOOST_AUTO_TEST_CASE(readInvalidNLoci)
{

    // Write a file with invalid number of loci
    tst::write("a1.txt", "nloci 0\n");
    tst::write("a2.txt", "nloci 10 10\n");

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Parameter nloci must be strictly positive in line 1 of file a1.txt");
    tst::checkError([&]() { Architecture arch("a2.txt"); }, "Too many values for parameter nloci in line 1 of file a2.txt");

    // Remove file
    std::remove("a1.txt");
    std::remove("a2.txt");

}

// Test that error when invalid number of edges
BOOST_AUTO_TEST_CASE(readInvalidNEdges) {

    // Write a file with invalid number of edges
    tst::write("a1.txt", "nedges 10 10\n");

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Too many values for parameter nedges in line 1 of file a1.txt");

    // Remove file
    std::remove("a1.txt");
    
}

// Test that error when invalid number of traits
BOOST_AUTO_TEST_CASE(readInvalidNTraits) {

    // Write a file with invalid number of traits
    tst::write("a1.txt", "ntraits 0\n");
    tst::write("a2.txt", "ntraits 10 10\n");

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Parameter ntraits must be strictly positive in line 1 of file a1.txt");
    tst::checkError([&]() { Architecture arch("a2.txt"); }, "Too many values for parameter ntraits in line 1 of file a2.txt");

    // Remove file
    std::remove("a1.txt");
    std::remove("a2.txt");

}

// Test that error when invalid encoded traits
BOOST_AUTO_TEST_CASE(readInvalidTraitIds) {

    // Write a file with invalid encoded traits
    tst::write("a1.txt", "nloci 5\nntraits 2\ntraitids 0 0 1 1 2\n");
    tst::write("a2.txt", "nloci 5\nntraits 2\ntraitids 1 1 2 2 2 2\n");

    // TODO: What if some loci end up encoded by zero traits?

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Parameter traitids must be strictly positive in line 3 of file a1.txt");
    tst::checkError([&]() { Architecture arch("a2.txt"); }, "Too many values for parameter traitids in line 3 of file a2.txt");

    // Remove files
    std::remove("a1.txt");
    std::remove("a2.txt");

}

// Test that error when invalid effect sizes
BOOST_AUTO_TEST_CASE(readInvalidEffects) {

    // Write a file with invalid effect sizes
    tst::write("a1.txt", "nloci 5\neffects 0.1 0.2 0.3 0.4 0.5 0.6\n");

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Too many values for parameter effects in line 2 of file a1.txt");

    // Remove file
    std::remove("a1.txt");

}

// Test that error when invalid dominance coefficients
BOOST_AUTO_TEST_CASE(readInvalidDominances) {

    // Write a file with invalid dominance coefficients
    tst::write("a1.txt", "nloci 5\ndominances 0.01 0.02 0.03 0.04 0.05 0.06\n");

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Too many values for parameter dominances in line 2 of file a1.txt");

    // Remove file
    std::remove("a1.txt");

}

// Test that error when invalid edge starting locus
BOOST_AUTO_TEST_CASE(readInvalidFrom) {

    // Write a file with invalid edge starting locus
    tst::write("a1.txt", "nloci 5\nnedges 2\nfrom 0 5\n");
    tst::write("a2.txt", "nloci 5\nnedges 2\nfrom 1 2 2\n");

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Parameter from must be strictly positive in line 3 of file a1.txt");
    tst::checkError([&]() { Architecture arch("a2.txt"); }, "Too many values for parameter from in line 3 of file a2.txt");

    // Remove file
    std::remove("a1.txt");
    std::remove("a2.txt");

}

// Test that error when invalid edge ending locus
BOOST_AUTO_TEST_CASE(readInvalidTo) {

    // Write a file with invalid edge ending locus
    tst::write("a1.txt", "nloci 5\nnedges 2\nto 0 5\n");
    tst::write("a2.txt", "nloci 5\nnedges 2\nto 1 1 2\n");

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Parameter to must be strictly positive in line 3 of file a1.txt");
    tst::checkError([&]() { Architecture arch("a2.txt"); }, "Too many values for parameter to in line 3 of file a2.txt");

    // Remove file
    std::remove("a1.txt");
    std::remove("a2.txt");

}

// Test that error when invalid edge weights
BOOST_AUTO_TEST_CASE(readInvalidWeights) {

    // Write a file with invalid edge weights
    tst::write("a1.txt", "nloci 5\nnedges 2\nweights 0.5 0.6 0.7\n");

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Too many values for parameter weights in line 3 of file a1.txt");

    // Remove file
    std::remove("a1.txt");

}

// Test that error when more traits than loci
BOOST_AUTO_TEST_CASE(readTooManyTraitsGivenLoci)
{

    // Write architecture file
    std::ostringstream content;
    content << "nloci 5\n";
    content << "nedges 2\n";
    content << "ntraits 6\n";
    content << "traitids 1 1 2 2 2\n";
    content << "effects 0.1 0.2 0.3 0.4 0.5\n";
    content << "dominances 0.01 0.02 0.03 0.04 0.05\n";
    content << "from 1 3\n";
    content << "to 2 4\n";
    content << "weights 0.5 0.6\n";
    tst::write("a1.txt", content.str());

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Too many traits for the number of loci in file a1.txt");

    // Remove file
    std::remove("a1.txt");

}

// Test that error when encoded trait is out of bounds
BOOST_AUTO_TEST_CASE(readEncodedTraitOutOfBounds)
{

    // Write architecture file
    std::ostringstream content;
    content << "nloci 5\n";
    content << "nedges 2\n";
    content << "ntraits 2\n";
    content << "traitids 1 1 2 2 3\n";
    content << "effects 0.1 0.2 0.3 0.4 0.5\n";
    content << "dominances 0.01 0.02 0.03 0.04 0.05\n";
    content << "from 1 3\n";
    content << "to 2 4\n";
    content << "weights 0.5 0.6\n";
    tst::write("a1.txt", content.str());

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Encoded trait 3 of locus 4 is out of bounds in file a1.txt");

    // Remove file
    std::remove("a1.txt");

}

// Test that error when start locus is out of bounds
BOOST_AUTO_TEST_CASE(readStartLocusOutOfBounds)
{

    // Write architecture file
    std::ostringstream content;
    content << "nloci 5\n";
    content << "nedges 2\n";
    content << "ntraits 2\n";
    content << "traitids 1 1 2 2 2\n";
    content << "effects 0.1 0.2 0.3 0.4 0.5\n";
    content << "dominances 0.01 0.02 0.03 0.04 0.05\n";
    content << "from 6 3\n";
    content << "to 2 4\n";
    content << "weights 0.5 0.6\n";
    tst::write("a1.txt", content.str());

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Start locus 6 of edge 1 is out of bounds in file a1.txt");

    // Remove file
    std::remove("a1.txt");

}

// Test that error when end locus is out of bounds
BOOST_AUTO_TEST_CASE(readEndLocusOutOfBounds)
{   

    // Write architecture file
    std::ostringstream content;
    content << "nloci 5\n";
    content << "nedges 2\n";
    content << "ntraits 2\n";
    content << "traitids 1 1 2 2 2\n";
    content << "effects 0.1 0.2 0.3 0.4 0.5\n";
    content << "dominances 0.01 0.02 0.03 0.04 0.05\n";
    content << "from 1 3\n";
    content << "to 2 6\n";
    content << "weights 0.5 0.6\n";
    tst::write("a1.txt", content.str());

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "End locus 6 of edge 2 is out of bounds in file a1.txt");

    // Remove file
    std::remove("a1.txt");

}

// Test that error when start and end loci are the same
BOOST_AUTO_TEST_CASE(readSameStartEndLocus)
{

    // Write architecture file
    std::ostringstream content;
    content << "nloci 5\n";
    content << "nedges 2\n";
    content << "ntraits 2\n";
    content << "traitids 1 1 2 2 2\n";
    content << "effects 0.1 0.2 0.3 0.4 0.5\n";
    content << "dominances 0.01 0.02 0.03 0.04 0.05\n";
    content << "from 1 3\n";
    content << "to 1 4\n";
    content << "weights 0.5 0.6\n";
    tst::write("a1.txt", content.str());

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Start and end loci of edge 1 are the same in file a1.txt");

    // Remove file
    std::remove("a1.txt");

}

// Test that error when start and end loci affect different traits
BOOST_AUTO_TEST_CASE(readDifferentTraitLoci)
{
    
    // Write architecture file
    std::ostringstream content;
    content << "nloci 5\n";
    content << "nedges 2\n";
    content << "ntraits 2\n";
    content << "traitids 1 1 2 2 2\n";
    content << "effects 0.1 0.2 0.3 0.4 0.5\n";
    content << "dominances 0.01 0.02 0.03 0.04 0.05\n";
    content << "from 1 1\n";
    content << "to 2 4\n";
    content << "weights 0.5 0.6\n";
    tst::write("a1.txt", content.str());

    // Check
    tst::checkError([&]() { Architecture arch("a1.txt"); }, "Start and end loci of edge 2 affect different traits in file a1.txt");

    // Remove file
    std::remove("a1.txt");

}

// Test that architecture can be saved to a file
BOOST_AUTO_TEST_CASE(saveArchitecture) {

    // Create architecture
    Architecture arch;

    // Modify
    arch.nloci = 5u;
    arch.nedges = 2u;
    arch.ntraits = 2u;
    arch.traitids = {0u, 0u, 1u, 1u, 1u};
    arch.effects = {0.1, 0.2, 0.3, 0.4, 0.5};
    arch.dominances = {0.01, 0.02, 0.03, 0.04, 0.05};
    arch.from = {0u, 2u};
    arch.to = {1u, 3u};
    arch.weights = {0.5, 0.6};

    // Save
    arch.save("architecture.txt");

    // Read the architecture back
    Architecture arch2("architecture.txt");

    // Check that the parameters have been properly replaced
    BOOST_CHECK_EQUAL(arch2.nloci, 5u);
    BOOST_CHECK_EQUAL(arch2.nedges, 2u);
    BOOST_CHECK_EQUAL(arch2.ntraits, 2u);
    BOOST_CHECK_EQUAL(arch2.traitids[0], 0u);
    BOOST_CHECK_EQUAL(arch2.traitids[1], 0u);
    BOOST_CHECK_EQUAL(arch2.traitids[2], 1u);
    BOOST_CHECK_EQUAL(arch2.traitids[3], 1u);
    BOOST_CHECK_EQUAL(arch2.traitids[4], 1u);
    BOOST_CHECK_CLOSE(arch2.effects[0], 0.1, 1e-6);
    BOOST_CHECK_CLOSE(arch2.effects[1], 0.2, 1e-6);
    BOOST_CHECK_CLOSE(arch2.effects[2], 0.3, 1e-6);
    BOOST_CHECK_CLOSE(arch2.effects[3], 0.4, 1e-6);
    BOOST_CHECK_CLOSE(arch2.effects[4], 0.5, 1e-6);
    BOOST_CHECK_CLOSE(arch2.dominances[0], 0.01, 1e-6);
    BOOST_CHECK_CLOSE(arch2.dominances[1], 0.02, 1e-6);
    BOOST_CHECK_CLOSE(arch2.dominances[2], 0.03, 1e-6);
    BOOST_CHECK_CLOSE(arch2.dominances[3], 0.04, 1e-6);
    BOOST_CHECK_CLOSE(arch2.dominances[4], 0.05, 1e-6);
    BOOST_CHECK_EQUAL(arch2.from[0], 0u);
    BOOST_CHECK_EQUAL(arch2.from[1], 2u);
    BOOST_CHECK_EQUAL(arch2.to[0], 1u);
    BOOST_CHECK_EQUAL(arch2.to[1], 3u);
    BOOST_CHECK_CLOSE(arch2.weights[0], 0.5, 1e-6);
    BOOST_CHECK_CLOSE(arch2.weights[1], 0.6, 1e-6); 

    // Remove files
    std::remove("architecture.txt");

}

// Test that error when architecture cannot be saved
BOOST_AUTO_TEST_CASE(errorWhenSavingArchitecture) {

    // Create architecture
    Architecture arch;

    // Save
    tst::checkError([&]() { arch.save(""); }, "Unable to open file ");

}

// Test that architecture can be generated from parameters
BOOST_AUTO_TEST_CASE(generateArchitecture) {

    // Create parameters
    Parameters pars;
    pars.ntraits = 3u;
    pars.nlocipertrait = {100u, 100u, 99u};
    pars.nedgespertrait = {100u, 100u, 100u};
    pars.skews = {0.5, 1.0, 1.5};

    // Update internal parameters
    pars.update();

    // Generate architecture
    Architecture arch;
    arch.generate(pars);

    // Check that the architecture has been properly generated
    BOOST_CHECK_EQUAL(arch.nloci, 299u);
    BOOST_CHECK_EQUAL(arch.nedges, 300u);
    BOOST_CHECK_EQUAL(arch.ntraits, 3u);
    BOOST_CHECK_EQUAL(arch.nlocipertrait[0], 100u);
    BOOST_CHECK_EQUAL(arch.nlocipertrait[1], 100u);
    BOOST_CHECK_EQUAL(arch.nlocipertrait[2], 99u);
    BOOST_CHECK_EQUAL(arch.nedgespertrait[0], 100u);
    BOOST_CHECK_EQUAL(arch.nedgespertrait[1], 100u);
    BOOST_CHECK_EQUAL(arch.nedgespertrait[2], 100u);
    BOOST_CHECK_EQUAL(arch.traitids.size(), 299u);
    BOOST_CHECK_EQUAL(arch.effects.size(), 299u);
    BOOST_CHECK_EQUAL(arch.dominances.size(), 299u);
    BOOST_CHECK_EQUAL(arch.from.size(), 300u);
    BOOST_CHECK_EQUAL(arch.to.size(), 300u);
    BOOST_CHECK_EQUAL(arch.weights.size(), 300u);

}

// Test that error when not all edges can be made
BOOST_AUTO_TEST_CASE(errorWhenGeneratingArchitecture) {

    // Note: This test is PROBABILISTIC.

    // Create parameters
    Parameters pars;
    pars.ntraits = 1u;
    pars.nlocipertrait = {100u};
    pars.nedgespertrait = {4950u};
    pars.skews = {1.0};

    // Update internal parameters
    pars.update();

    // Check that it throws an error
    Architecture arch;
    tst::checkError([&]() { arch.generate(pars); }, "Not all requested edges could be made for trait 1 with the given parameters");

}