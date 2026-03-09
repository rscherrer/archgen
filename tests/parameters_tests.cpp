#define BOOST_TEST_DYNAMIC_LINK
#define BOOST_TEST_MODULE Main

// Here we test the proper behavior of the Parameters class.

#include "testutils.hpp"
#include "../src/parameters.hpp"
#include <boost/test/unit_test.hpp>

// Test that parameters can be created
BOOST_AUTO_TEST_CASE(parametersCreated) {

    // Without input file should work fine
    Parameters pars;

    // Check a few default values
    BOOST_CHECK_EQUAL(pars.popsize, 10u);

}

// Parameters can update by reading from a file
BOOST_AUTO_TEST_CASE(readParameters)
{

    // Prepare content of parameter file
    std::ostringstream content;

    // Add lines
    content << "nrepl 1\n";
    content << "popsize 10\n";
    content << "ntraits 3\n";
    content << "mutation 0.1\n";
    content << "sdeffects 0.5\n";
    content << "sddomcoeffs 1\n";
    content << "sdweights 0.3\n";
    content << "nlocipertrait 5 10 15\n";
    content << "nedgespertrait 0 0 0\n";
    content << "skew 0.5 1.0 1.5\n";
    content << "epistasis 0.1 0.2 0.3\n";
    content << "dominance 0.4 0.5 0.6\n";
    content << "envnoise 0.7 0.8 0.9\n";
    content << "sampling 2\n";
    content << "ratio 0.25\n";
    content << "seed 12345\n";
    content << "import 0\n";
    content << "standard 0\n";
    content << "loadarch 1\n";
    content << "savearch 0\n";
    content << "savepars 1\n";
    content << "binary 1\n";
    content << "verbose 0\n";

    // Write the content to a file
    tst::write("parameters.txt", content.str());

    // Read the parameter file
    Parameters pars("parameters.txt");

    // Check that the parameters have been updated
    BOOST_CHECK_EQUAL(pars.nrepl, 1u);
    BOOST_CHECK_EQUAL(pars.popsize, 10u);
    BOOST_CHECK_EQUAL(pars.ntraits, 3u);
    BOOST_CHECK_EQUAL(pars.mutation, 0.1);
    BOOST_CHECK_EQUAL(pars.sdeffects, 0.5);
    BOOST_CHECK_EQUAL(pars.sddomcoeffs, 1.0);
    BOOST_CHECK_EQUAL(pars.sdweights, 0.3);
    BOOST_CHECK_EQUAL(pars.nlocipertrait[0], 5u);
    BOOST_CHECK_EQUAL(pars.nlocipertrait[1], 10u);
    BOOST_CHECK_EQUAL(pars.nlocipertrait[2], 15u);
    BOOST_CHECK_EQUAL(pars.nedgespertrait[0], 0u);
    BOOST_CHECK_EQUAL(pars.nedgespertrait[1], 0u);
    BOOST_CHECK_EQUAL(pars.nedgespertrait[2], 0u);
    BOOST_CHECK_EQUAL(pars.skew[0], 0.5);
    BOOST_CHECK_EQUAL(pars.skew[1], 1.0);
    BOOST_CHECK_EQUAL(pars.skew[2], 1.5);
    BOOST_CHECK_EQUAL(pars.epistasis[0], 0.1);
    BOOST_CHECK_EQUAL(pars.epistasis[1], 0.2);
    BOOST_CHECK_EQUAL(pars.epistasis[2], 0.3);
    BOOST_CHECK_EQUAL(pars.dominance[0], 0.4);
    BOOST_CHECK_EQUAL(pars.dominance[1], 0.5);
    BOOST_CHECK_EQUAL(pars.dominance[2], 0.6);
    BOOST_CHECK_EQUAL(pars.envnoise[0], 0.7);
    BOOST_CHECK_EQUAL(pars.envnoise[1], 0.8);
    BOOST_CHECK_EQUAL(pars.envnoise[2], 0.9);
    BOOST_CHECK_EQUAL(pars.sampling, 2u);
    BOOST_CHECK_EQUAL(pars.ratio, 0.25);
    BOOST_CHECK_EQUAL(pars.seed, 12345u);
    BOOST_CHECK(!pars.import);
    BOOST_CHECK(!pars.standard);
    BOOST_CHECK(pars.loadarch);
    BOOST_CHECK(!pars.savearch);
    BOOST_CHECK(pars.savepars);
    BOOST_CHECK(pars.binary);
    BOOST_CHECK(!pars.verbose);

    // Remove files
    std::remove("parameters.txt");

}

// Test that parameter reading fails when invalid parameters are provided
BOOST_AUTO_TEST_CASE(readInvalidParameters)
{

    // Write a file with invalid parameters
    tst::write("parameters.txt", "popsize 10\ninvalid 42\n");

    // Try to...
    tst::checkError([&]() {

        // Read the file
        Parameters pars("parameters.txt");

    }, "Invalid parameter: invalid in line 2 of file parameters.txt");

    // Remove files
    std::remove("parameters.txt");

}

// Test error upon invalid number of replicates
BOOST_AUTO_TEST_CASE(readInvalidNRepl)
{

    // Write a file with invalid number of replicates
    tst::write("p1.txt", "nrepl 0\n");
    tst::write("p2.txt", "nrepl 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter nrepl must be strictly positive in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter nrepl in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid starting population size
BOOST_AUTO_TEST_CASE(readInvalidPopSize)
{

    // Write a file with invalid population size
    tst::write("p1.txt", "popsize 0\n");
    tst::write("p2.txt", "popsize 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter popsize must be strictly positive in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter popsize in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid initial allele frequency
BOOST_AUTO_TEST_CASE(readInvalidMutation)
{

    // Write a file with invalid initial allele frequency
    tst::write("p1.txt", "mutation -0.5\n");
    tst::write("p2.txt", "mutation 0.5 0.5\n");
    tst::write("p3.txt", "mutation 1.5\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter mutation must be between 0 and 1 in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter mutation in line 1 of file p2.txt");
    tst::checkError([&]() { Parameters pars("p3.txt"); }, "Parameter mutation must be between 0 and 1 in line 1 of file p3.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");
    std::remove("p3.txt");

}

// Test error upon invalid effect size standard deviation
BOOST_AUTO_TEST_CASE(readInvalidEffect)
{

    // Write a file with invalid mutational effect standard deviation
    tst::write("p1.txt", "sdeffects -0.5\n");
    tst::write("p2.txt", "sdeffects 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter sdeffects must be positive in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter sdeffects in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid dominance coefficient standard deviation
BOOST_AUTO_TEST_CASE(readInvalidDomCoeff)
{

    // Write a file with invalid mutational dominance coefficient standard deviation
    tst::write("p1.txt", "sddomcoeffs -0.5\n");
    tst::write("p2.txt", "sddomcoeffs 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter sddomcoeffs must be positive in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter sddomcoeffs in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid interaction weight standard deviation
BOOST_AUTO_TEST_CASE(readInvalidWeight)
{

    // Write a file with invalid mutational interaction weight standard deviation
    tst::write("p1.txt", "sdweights -0.5\n");
    tst::write("p2.txt", "sdweights 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter sdweights must be positive in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter sdweights in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid number of traits
BOOST_AUTO_TEST_CASE(readInvalidNTraits)
{

    // Write a file with invalid number of traits
    tst::write("p1.txt", "ntraits 0\n");
    tst::write("p2.txt", "ntraits 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter ntraits must be strictly positive in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter ntraits in line 1 of file p2.txt");
    
    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");
    
}

// Test error upon invalid number of loci per trait
BOOST_AUTO_TEST_CASE(readInvalidNLociPerTrait)
{
    
    // Write a file with invalid number of loci per trait
    tst::write("p1.txt", "ntraits 3\nnlocipertrait 5 0 15\n");
    tst::write("p2.txt", "ntraits 3\nnlocipertrait 5 10 15 20\n");
    
    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter nlocipertrait must be strictly positive in line 2 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter nlocipertrait in line 2 of file p2.txt");
    
    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");
    
}

// Test error upon invalid number of edges per trait
BOOST_AUTO_TEST_CASE(readInvalidNEdgesPerTrait)
{

    // Write a file with invalid number of edges per trait
    tst::write("p1.txt", "ntraits 3\nnedgespertrait 1 2 3 4\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Too many values for parameter nedgespertrait in line 2 of file p1.txt");
    
    // Remove files
    std::remove("p1.txt");

}

// Test error upon invalid skew parameter
BOOST_AUTO_TEST_CASE(readInvalidSkew)
{

    // Write a file with invalid skew parameters
    tst::write("p1.txt", "ntraits 3\nskew 0.5 1.0 1.5 2.0\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Too many values for parameter skew in line 2 of file p1.txt");

    // Remove files
    std::remove("p1.txt");

}

// Test error upon invalid interaction importance scaling parameter
BOOST_AUTO_TEST_CASE(readInvalidEpistasis)
{

    // Write a file with invalid interaction importance scaling parameters
    tst::write("p1.txt", "ntraits 3\nepistasis 0.1 -0.2 0.3\n");
    tst::write("p2.txt", "ntraits 3\nepistasis 0.1 0.2 1.1\n");
    tst::write("p3.txt", "ntraits 3\nepistasis 0.1 0.2 0.3 0.4\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter epistasis must be between 0 and 1 in line 2 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Parameter epistasis must be between 0 and 1 in line 2 of file p2.txt");
    tst::checkError([&]() { Parameters pars("p3.txt"); }, "Too many values for parameter epistasis in line 2 of file p3.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");
    std::remove("p3.txt");

}

// Test error upon invalid dominance importance scaling parameter
BOOST_AUTO_TEST_CASE(readInvalidDominance)
{

    // Write a file with invalid dominance importance scaling parameters
    tst::write("p1.txt", "ntraits 3\ndominance 0.4 -0.5 0.6\n");
    tst::write("p2.txt", "ntraits 3\ndominance 0.4 0.5 0.6 0.7\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter dominance must be positive in line 2 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter dominance in line 2 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid environmental importance scaling parameter
BOOST_AUTO_TEST_CASE(readInvalidEnvNoise)
{

    // Write a file with invalid environmental importance scaling parameters
    tst::write("p1.txt", "ntraits 3\nenvnoise 0.7 -0.8 0.9\n");
    tst::write("p2.txt", "ntraits 3\nenvnoise 0.7 0.8 0.9 1.0\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter envnoise must be positive in line 2 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter envnoise in line 2 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid sampling mode
BOOST_AUTO_TEST_CASE(readInvalidSampling)
{

    // Write a file with invalid sampling mode
    tst::write("p1.txt", "sampling 1 1\n");
    tst::write("p2.txt", "sampling 4\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Too many values for parameter sampling in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Parameter sampling must be between 0 and 3 in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid ratio parameter
BOOST_AUTO_TEST_CASE(readInvalidRatio)
{

    // Write a file with invalid ratio parameter
    tst::write("p1.txt", "ratio -0.5\n");
    tst::write("p2.txt", "ratio 0.5 0.5\n");
    tst::write("p3.txt", "ratio 1.5\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter ratio must be between 0 and 1 in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter ratio in line 1 of file p2.txt");
    tst::checkError([&]() { Parameters pars("p3.txt"); }, "Parameter ratio must be between 0 and 1 in line 1 of file p3.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");
    std::remove("p3.txt");

}

// Test error upon invalid random seed
BOOST_AUTO_TEST_CASE(readInvalidSeed)
{

    // Write a file with invalid random seed
    tst::write("p1.txt", "seed -1\n");
    tst::write("p2.txt", "seed 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Invalid value type for parameter seed in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter seed in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid import flag
BOOST_AUTO_TEST_CASE(readInvalidImport)
{

    // Write a file with invalid import flag
    tst::write("p1.txt", "import -1\n");
    tst::write("p2.txt", "import 1 1\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Invalid value type for parameter import in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter import in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid standardization flag
BOOST_AUTO_TEST_CASE(readInvalidStandard)
{

    // Write a file with invalid standardization flag
    tst::write("p1.txt", "standard -1\n");
    tst::write("p2.txt", "standard 1 1\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Invalid value type for parameter standard in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter standard in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid architecture loading flag
BOOST_AUTO_TEST_CASE(readInvalidLoadArch)
{

    // Write a file with invalid architecture loading flag
    tst::write("p1.txt", "loadarch -1\n");
    tst::write("p2.txt", "loadarch 1 1\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Invalid value type for parameter loadarch in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter loadarch in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid architecture saving flag
BOOST_AUTO_TEST_CASE(readInvalidSaveArch)
{

    // Write a file with invalid architecture saving flag
    tst::write("p1.txt", "savearch -1\n");
    tst::write("p2.txt", "savearch 1 1\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Invalid value type for parameter savearch in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter savearch in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid parameters saving flag
BOOST_AUTO_TEST_CASE(readInvalidSavePars)
{

    // Write a file with invalid parameters saving flag
    tst::write("p1.txt", "savepars -1\n");
    tst::write("p2.txt", "savepars 1 1\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Invalid value type for parameter savepars in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter savepars in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid binary saving flag
BOOST_AUTO_TEST_CASE(readInvalidBinary)
{

    // Write a file with invalid binary saving flag
    tst::write("p1.txt", "binary -1\n");
    tst::write("p2.txt", "binary 1 1\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Invalid value type for parameter binary in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter binary in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid verbosity flag
BOOST_AUTO_TEST_CASE(readInvalidVerbose)
{

    // Write a file with invalid verbosity flag
    tst::write("p1.txt", "verbose 2\n");
    tst::write("p2.txt", "verbose 1 1\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Invalid value type for parameter verbose in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter verbose in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Case with too few edges to connect all loci
BOOST_AUTO_TEST_CASE(readTooFewEdgesGivenLoci)
{

    // Write parameter file
    tst::write("p1.txt", "ntraits 3\nnlocipertrait 3 3 3\nnedgespertrait 1 0 0\nskew 1 1 1\nepistasis 1 1 1\ndominance 1 1 1\nenvnoise 1 1 1\n");

    // Check error
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Not enough edges to connect all loci for trait 1 in file p1.txt");

    // Remove files
    std::remove("p1.txt");

}

// Case with too many edges for the number of loci
BOOST_AUTO_TEST_CASE(readTooManyEdgesGivenLoci)
{

    // Write parameter file
    tst::write("p1.txt", "ntraits 3\nnlocipertrait 3 3 3\nnedgespertrait 5 0 0\nskew 1 1 1\nepistasis 1 1 1\ndominance 1 1 1\nenvnoise 1 1 1\n");

    // Check error
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Too many edges for the number of loci for trait 1 in file p1.txt");

    // Remove files
    std::remove("p1.txt");

}

// Test that the parameter saving function works
BOOST_AUTO_TEST_CASE(parameterSavingWorks) {

    // Create parameters
    Parameters pars1;

    // Modify
    pars1.popsize = 42u;

    // Save
    pars1.save("parameters.txt");

    // Overwrite the other
    Parameters pars2("parameters.txt");

    // Make sure the values have been properly replaced
    BOOST_CHECK_EQUAL(pars2.popsize, 42u);

    // Remove files
    std::remove("parameters.txt");

}

// Test when parameter saving does not work
BOOST_AUTO_TEST_CASE(errorWhenSavingParameters) {

    // Check
    tst::checkError([&] {

        // Create parameters
        Parameters pars;

        // Save
        pars.save("");

    }, "Unable to open file ");

}