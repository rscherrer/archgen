#define BOOST_TEST_DYNAMIC_LINK
#define BOOST_TEST_MODULE Main

// Here we test all the uses and misuses of the program. These mostly have to
// with calling the program, passing arguments, reading from and writing to
// files, and error handling.

#include "testutils.hpp"
#include "../src/MAIN.hpp"
#include "../src/parameters.hpp"
#include <boost/test/unit_test.hpp>

// Test that the simulation runs
BOOST_AUTO_TEST_CASE(useCase) {

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program"}));

}

// Test that it fails when too many arguments are provided
BOOST_AUTO_TEST_CASE(abuseTooManyArgs) {

    // Check error
    tst::checkError([&] { 
        doMain({"program", "parameter.txt", "onetoomany.txt"});
    }, "Too many arguments provided");

}

// Test that the simulation runs with a parameter file
BOOST_AUTO_TEST_CASE(useCaseWithParameterFile) {

    // Write a parameter file
    tst::write("parameters.txt", "popsize 9");

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Cleanup
    std::remove("parameters.txt");

}

// Test that error when invalid parameter file
BOOST_AUTO_TEST_CASE(abuseInvalidParameterFile) {

    // Check error
    tst::checkError([&] {
        doMain({"program", "nonexistent.txt"});
    }, "Unable to open file nonexistent.txt");

}

// Test that it works when saving the parameters
BOOST_AUTO_TEST_CASE(useCaseWithParameterSaving) {

    // Write a parameter file specifying to save parameters
    tst::write("parameters.txt", "savepars 1");

    // Run a simulation
    doMain({"program", "parameters.txt"});

    // Read the saved parameters
    Parameters pars1("paramlog.txt");

    // Rerun the simulation with the saved parameters
    doMain({"program", "paramlog.txt"});

    // Read the overwritten parameters
    Parameters pars2("paramlog.txt");

    // Check that the (clock-generated) seed is the same
    BOOST_CHECK_EQUAL(pars1.seed, pars2.seed);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");

}

// Test that it works when loading the architecture file
BOOST_AUTO_TEST_CASE(useCaseWithArchitectureLoading) {

    // Write a parameter file specifying to save the architecture
    tst::write("parameters.txt", "savearch 1");

    // Run a simulation
    doMain({"program", "parameters.txt"});

    // Check that the expected output file is present
    BOOST_CHECK_NO_THROW(tst::readtext("architecture.txt"));

    // Rerun the simulation with the same architecture
    tst::write("parameters.txt", "loadarch 1");
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");

}

// Test that error when invalid architecture file
BOOST_AUTO_TEST_CASE(abuseInvalidArchitectureFile) {

    // Write a parameter file specifying to load the architecture
    tst::write("parameters.txt", "loadarch 1");

    // Check error
    tst::checkError([&] {
        doMain({"program", "parameters.txt"});
    }, "Unable to open file architecture.txt");

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");

}

// Test that it works when saving the architecture file
BOOST_AUTO_TEST_CASE(useCaseWithArchitectureSaving) {

    // Write a parameter file specifying to save the architecture
    tst::write("parameters.txt", "savearch 1");

    // Run a simulation
    doMain({"program", "parameters.txt"});

    // Check that the expected output file is present
    BOOST_CHECK_NO_THROW(tst::readtext("architecture.txt"));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");

}

// Test that the right output files are being written
BOOST_AUTO_TEST_CASE(useCaseOutputFiles) {

    // Write a parameter file specifying to save data
    tst::write("parameters.txt", "binary 1");

    // Run the simulation
    doMain({"program", "parameters.txt"});

    // Check that the expected output files are present
    BOOST_CHECK_NO_THROW(tst::read("alleles.dat"));
    BOOST_CHECK_NO_THROW(tst::readtext("traits.csv"));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("alleles.dat");
    std::remove("traits.csv");

}

// Test that it works when verbose is on
BOOST_AUTO_TEST_CASE(useCaseWithVerbose) {

    // Write a parameter file with verbose
    tst::write("parameters.txt", "verbose 1");

    // Capture output
    const std::string output = tst::captureOutput([&] { doMain({"program", "parameters.txt"}); });

    // Find relevant bits in output
    BOOST_CHECK(output.find("successfully") != std::string::npos);

    // Cleanup
    std::remove("parameters.txt");

}