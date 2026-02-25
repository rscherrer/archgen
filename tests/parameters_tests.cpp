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
    content << "popsize 10\n";
    content << "nloci 20\n";
    content << "nedges 5\n";
    content << "ntraits 3\n";
    content << "allfreq 0.1\n";
    content << "effect 0.5\n";
    content << "weight 0.3\n";
    content << "nlocipertrait 5 10 15\n";
    content << "nedgespertrait 1 2 3\n";
    content << "skews 0.5 1.0 1.5\n";
    content << "epistasis 0.1 0.2 0.3\n";
    content << "dominance 0.4 0.5 0.6\n";
    content << "envnoise 0.7 0.8 0.9\n";
    content << "sampling 2\n";
    content << "ratio 0.25\n";
    content << "seed 12345\n";
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
    BOOST_CHECK_EQUAL(pars.popsize, 10u);
    BOOST_CHECK_EQUAL(pars.nloci, 20u);
    BOOST_CHECK_EQUAL(pars.nedges, 5u);
    BOOST_CHECK_EQUAL(pars.ntraits, 3u);
    BOOST_CHECK_EQUAL(pars.allfreq, 0.1);
    BOOST_CHECK_EQUAL(pars.effect, 0.5);
    BOOST_CHECK_EQUAL(pars.weight, 0.3);
    BOOST_CHECK_EQUAL(pars.nlocipertrait[0], 5u);
    BOOST_CHECK_EQUAL(pars.nlocipertrait[1], 10u);
    BOOST_CHECK_EQUAL(pars.nlocipertrait[2], 15u);
    BOOST_CHECK_EQUAL(pars.nedgespertrait[0], 1u);
    BOOST_CHECK_EQUAL(pars.nedgespertrait[1], 2u);
    BOOST_CHECK_EQUAL(pars.nedgespertrait[2], 3u);
    BOOST_CHECK_EQUAL(pars.skews[0], 0.5);
    BOOST_CHECK_EQUAL(pars.skews[1], 1.0);
    BOOST_CHECK_EQUAL(pars.skews[2], 1.5);
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
BOOST_AUTO_TEST_CASE(readInvalidAllFreq)
{

    // Write a file with invalid initial allele frequency
    tst::write("p1.txt", "allfreq -0.5\n");
    tst::write("p2.txt", "allfreq 0.5 0.5\n");
    tst::write("p3.txt", "allfreq 1.5\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter allfreq must be between 0 and 1 in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter allfreq in line 1 of file p2.txt");
    tst::checkError([&]() { Parameters pars("p3.txt"); }, "Parameter allfreq must be between 0 and 1 in line 1 of file p3.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");
    std::remove("p3.txt");

}

// Test error upon invalid effect size standard deviation
BOOST_AUTO_TEST_CASE(readInvalidEffect)
{

    // Write a file with invalid mutational effect standard deviation
    tst::write("p1.txt", "effect -0.5\n");
    tst::write("p2.txt", "effect 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter effect must be positive in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter effect in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid interaction weight standard deviation
BOOST_AUTO_TEST_CASE(readInvalidWeight)
{

    // Write a file with invalid mutational interaction weight standard deviation
    tst::write("p1.txt", "weight -0.5\n");
    tst::write("p2.txt", "weight 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter weight must be positive in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter weight in line 1 of file p2.txt");

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
BOOST_AUTO_TEST_CASE(readInvalidSkews)
{

    // Write a file with invalid skew parameters
    tst::write("p1.txt", "ntraits 3\nskews 0.5 1.0 1.5 2.0\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Too many values for parameter skews in line 2 of file p1.txt");

    // Remove files
    std::remove("p1.txt");

}

// Test error upon invalid interaction importance scaling parameter
BOOST_AUTO_TEST_CASE(readInvalidEpistasis)
{

    // Write a file with invalid interaction importance scaling parameters
    tst::write("p1.txt", "ntraits 3\nepistasis 0.1 -0.2 0.3\n");
    tst::write("p2.txt", "ntraits 3\nepistasis 0.1 0.2 0.3 1.1\n");
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
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter seed must be non-negative in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter seed in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid architecture loading flag
BOOST_AUTO_TEST_CASE(readInvalidLoadArch)
{

    // Write a file with invalid architecture loading flag
    tst::write("p1.txt", "loadarch -1\n");
    tst::write("p2.txt", "loadarch 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter loadarch must be 0 or 1 in line 1 of file p1.txt");
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
    tst::write("p2.txt", "savearch 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter savearch must be 0 or 1 in line 1 of file p1.txt");
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
    tst::write("p2.txt", "savepars 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter savepars must be 0 or 1 in line 1 of file p1.txt");
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
    tst::write("p2.txt", "binary 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter binary must be 0 or 1 in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter binary in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Test error upon invalid verbosity flag
BOOST_AUTO_TEST_CASE(readInvalidVerbose)
{

    // Write a file with invalid verbosity flag
    tst::write("p1.txt", "verbose -1\n");
    tst::write("p2.txt", "verbose 10 10\n");

    // Check
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Parameter verbose must be 0 or 1 in line 1 of file p1.txt");
    tst::checkError([&]() { Parameters pars("p2.txt"); }, "Too many values for parameter verbose in line 1 of file p2.txt");

    // Remove files
    std::remove("p1.txt");
    std::remove("p2.txt");

}

// Case with too many edges for the number of loci
BOOST_AUTO_TEST_CASE(readTooManyEdgesGivenLoci)
{

    // Write parameter file
    tst::write("p1.txt", "ntraits 3\nnlocipertrait 3 3 3\nnedgespertrait 5 0 0\nskews 1 1 1\nepistasis 1 1 1\ndominance 1 1 1\nenvnoise 1 1 1\n");

    // Check error
    tst::checkError([&]() { Parameters pars("p1.txt"); }, "Too many edges for trait 1 given the number of loci in line 2 of file p1.txt");

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