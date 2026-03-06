#define BOOST_TEST_DYNAMIC_LINK
#define BOOST_TEST_MODULE Main

// Here we test all the uses and misuses of the program. These mostly have to
// with calling the program, passing arguments, reading from and writing to
// files, and error handling.

#include "testutils.hpp"
#include "../src/MAIN.hpp"
#include "../src/parameters.hpp"
#include <boost/test/unit_test.hpp>
#include <cmath>

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
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

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
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

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
    doMain({"program", "parameters.txt"});
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

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
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test that the right output files are being written
BOOST_AUTO_TEST_CASE(useCaseOutputFiles) {

    // Write a parameter file specifying to save data
    tst::write("parameters.txt", "popsize 3\nnlocipertrait 3");

    // Run the simulation
    doMain({"program", "parameters.txt"});

    // Check that the expected output files are present
    BOOST_CHECK_NO_THROW(tst::readtext("genotypes.csv"));
    BOOST_CHECK_NO_THROW(tst::readtext("traits.csv"));

    // Read values in
    std::vector<double> genotypes = tst::readcsv("genotypes.csv");
    std::vector<double> traits = tst::readcsv("traits.csv");

    // Check that the right number of values are present
    BOOST_CHECK_EQUAL(genotypes.size(), 4u * 3u);
    BOOST_CHECK_EQUAL(traits.size(), 2u * 3u);

    // Note: Here alleles are saved as genotypes (0, 1 or 2), not as separate alleles.
    // We also must keep in mind that one of the columns is the individual identifier.

    // Prepare to check
    bool iswrong = false;

    // For each genotype...
    for (size_t i = 0u; i < genotypes.size(); ++i) {

        // Skip individual identifier column
        if (i % 4u == 0u) continue;

        // Read it
        const size_t n = static_cast<size_t>(genotypes[i]);

        // If out of bounds...
        if (n > 2u) {
            
            // Flag and exit
            iswrong = true;
            break;

        }
    }

    // Make sure everything is fine
    BOOST_CHECK(!iswrong);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test that the right output files are being written in binary format
BOOST_AUTO_TEST_CASE(useCaseOutputFilesBinary) {

    // Write a parameter file specifying to save data
    tst::write("parameters.txt", "popsize 3\nnlocipertrait 3\nbinary 1");

    // Number of bits that should have been saved
    const size_t nbits = 3u * 3u * 2u;

    // Run the simulation
    doMain({"program", "parameters.txt"});

    // Read binary alleles into encoded decimal numbers
    std::vector<double> alleles = tst::readbin("alleles.dat");

    // Extract all the bits
    std::bitset<64u> bits(alleles[0u]);

    // Test maximum number of bits equal to one
    BOOST_CHECK(bits.count() <= nbits);

    // Count trailing bits
    size_t trailing = 0u;
    for (size_t i = nbits; i < bits.size(); ++i)
        trailing += bits.test(i);        

    // Check that they are all zeros
    BOOST_CHECK_EQUAL(trailing, 0u);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
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
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test that it works when there are edges in the gene network
BOOST_AUTO_TEST_CASE(useCaseWithEdges) {

    // Write a parameter file with epistasis
    tst::write("parameters.txt", "nedgespertrait 11");

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test that it works with more than one trait
BOOST_AUTO_TEST_CASE(useCaseWithMultipleTraits) {

    // Write a parameter file with multiple traits
    std::ostringstream content;
    content << "ntraits 3\n";
    content << "nlocipertrait 9 9 7\n";
    content << "nedgespertrait 9 9 9\n";
    content << "skews 1 1 1\n";
    content << "epistasis 0.1 0.1 0.1\n";
    content << "dominance 0.4 0.4 0.4\n";
    content << "envnoise 0.7 0.7 0.7\n";
    tst::write("parameters.txt", content.str());

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test that error when cannot open trait output file
BOOST_AUTO_TEST_CASE(abuseCannotOpenTraitOutputFile) {

    // Test with empty file name
    BOOST_CHECK_THROW(stf::saveTraits({1.0, 2.0, 3.0}, 3, ""), std::runtime_error);

}

// Test that error when cannot open allele output file
BOOST_AUTO_TEST_CASE(abuseCannotOpenAlleleOutputFile) {

    // Create alleles to save
    std::bitset<64u> bits;
    std::vector<std::bitset<64u> > alleles = {bits};

    // Test with empty file name
    BOOST_CHECK_THROW(stf::saveAlleles(alleles, 4u, 8u, "", false), std::runtime_error);

}

// Test that it works with given number of mutations to sample (default)
BOOST_AUTO_TEST_CASE(useCaseWithGivenSampling) {

    // Write a parameter file with given sampling
    tst::write("parameters.txt", "mutation 0.2");

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test that it works with Bernoulli sampling of mutations
BOOST_AUTO_TEST_CASE(useCaseWithBernoulliSampling) {

    // Write a parameter file with Bernoulli sampling
    tst::write("parameters.txt", "mutation 0.2\nsampling 1");

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test that it works with binomial sampling of mutations
BOOST_AUTO_TEST_CASE(useCaseWithBinomialSampling) {

    // Write a parameter file with binomial sampling
    tst::write("parameters.txt", "mutation 0.2\nsampling 2");

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test that it works with binomial sampling and high mutation rate
BOOST_AUTO_TEST_CASE(useCaseWithBinomialSamplingHighMutationRate) {

    // Write a parameter file with binomial sampling and high mutation rate
    tst::write("parameters.txt", "mutation 0.7\nsampling 2");

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test that it works with binomial sampling and full shuffle (forced by low ratio)
BOOST_AUTO_TEST_CASE(useCaseWithBinomialSamplingFullShuffle) {

    // Write a parameter file with binomial sampling and low ratio to force full shuffle
    tst::write("parameters.txt", "mutation 0.2\nsampling 2\nratio 0");

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test that it works with geometric sampling of mutations
BOOST_AUTO_TEST_CASE(useCaseWithGeometricSampling) {

    // Write a parameter file with geometric sampling
    tst::write("parameters.txt", "mutation 0.2\nsampling 3");

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test that it works with geometric sampling and very high mutation rate
BOOST_AUTO_TEST_CASE(useCaseWithGeometricSamplingHighMutationRate) {

    // Write a parameter file with geometric sampling and very high mutation rate
    tst::write("parameters.txt", "mutation 0.99\nsampling 3");

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test that it works with full mutations
BOOST_AUTO_TEST_CASE(useCaseWithFullSampling) {

    // Write a parameter file with full sampling
    tst::write("parameters.txt", "mutation 1");

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test with multiple replicates
BOOST_AUTO_TEST_CASE(useCaseWithMultipleReplicates) {

    // Write a parameter file with multiple replicates
    tst::write("parameters.txt", "nrepl 3");

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Check that the output files have been written
    BOOST_CHECK_NO_THROW(tst::readtext("architecture_1.txt"));
    BOOST_CHECK_NO_THROW(tst::readtext("architecture_2.txt"));
    BOOST_CHECK_NO_THROW(tst::readtext("architecture_3.txt"));
    BOOST_CHECK_NO_THROW(tst::readtext("genotypes_1.csv"));
    BOOST_CHECK_NO_THROW(tst::readtext("genotypes_2.csv"));
    BOOST_CHECK_NO_THROW(tst::readtext("genotypes_3.csv"));
    BOOST_CHECK_NO_THROW(tst::readtext("traits_1.csv"));
    BOOST_CHECK_NO_THROW(tst::readtext("traits_2.csv"));
    BOOST_CHECK_NO_THROW(tst::readtext("traits_3.csv"));

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture_1.txt");
    std::remove("architecture_2.txt");
    std::remove("architecture_3.txt");
    std::remove("genotypes_1.csv");
    std::remove("genotypes_2.csv");
    std::remove("genotypes_3.csv");
    std::remove("traits_1.csv");
    std::remove("traits_2.csv");
    std::remove("traits_3.csv");

}

// Test with multiple replicates and supplied architecture
BOOST_AUTO_TEST_CASE(useCaseWithMultipleReplicatesSuppliedArchitecture) {

    // Write a parameter file that saves architecture
    tst::write("parameters.txt", "savearch 1");

    // Run a simulation to save the architecture
    doMain({"program", "parameters.txt"});

    // Write a parameter file with multiple replicates
    tst::write("parameters.txt", "nrepl 3\nloadarch 1");

    // Check that the program runs
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Check that the output files have been written
    BOOST_CHECK_NO_THROW(tst::readtext("architecture.txt"));
    BOOST_CHECK_NO_THROW(tst::readtext("architecture_1.txt"));
    BOOST_CHECK_NO_THROW(tst::readtext("architecture_2.txt"));
    BOOST_CHECK_NO_THROW(tst::readtext("architecture_3.txt"));
    BOOST_CHECK_NO_THROW(tst::readtext("genotypes_1.csv"));
    BOOST_CHECK_NO_THROW(tst::readtext("genotypes_2.csv"));
    BOOST_CHECK_NO_THROW(tst::readtext("genotypes_3.csv"));
    BOOST_CHECK_NO_THROW(tst::readtext("traits_1.csv"));
    BOOST_CHECK_NO_THROW(tst::readtext("traits_2.csv"));
    BOOST_CHECK_NO_THROW(tst::readtext("traits_3.csv"));

    // Check that all architecture files are the same
    const std::string arch1 = tst::readtext("architecture.txt");
    const std::string arch2 = tst::readtext("architecture_1.txt");
    const std::string arch3 = tst::readtext("architecture_2.txt");
    const std::string arch4 = tst::readtext("architecture_3.txt");
    BOOST_CHECK_EQUAL(arch1, arch2);
    BOOST_CHECK_EQUAL(arch1, arch3);
    BOOST_CHECK_EQUAL(arch1, arch4);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("architecture_1.txt");
    std::remove("architecture_2.txt");
    std::remove("architecture_3.txt");
    std::remove("genotypes_1.csv");
    std::remove("genotypes_2.csv");
    std::remove("genotypes_3.csv");
    std::remove("traits_1.csv");
    std::remove("traits_2.csv");
    std::remove("traits_3.csv");

}

// Test that error when architecture supplied with different number of traits than specified
BOOST_AUTO_TEST_CASE(abuseSuppliedArchitectureWithDifferentNumberOfTraits) {

    // Write a parameter file that saves architecture
    tst::write("parameters.txt", "savearch 1");

    // Run a simulation to save the architecture
    doMain({"program", "parameters.txt"});

    // Write a parameter file with different number of traits
    tst::write("parameters.txt", "loadarch 1\nntraits 2\nnlocipertrait 3 3\nnedgespertrait 3 3\nskews 1 1\nepistasis 0.1 0.1\ndominance 0.4 0.4\nenvnoise 0.7 0.7");

    // Check error
    BOOST_CHECK_THROW(doMain({"program", "parameters.txt"}), std::runtime_error);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test that import of matrix of alleles from file works
BOOST_AUTO_TEST_CASE(useCaseWithAlleleImport) {

    // Parameter file
    tst::write("parameters.txt", "mutation 1");

    // Run the program to save a genotype file
    doMain({"program", "parameters.txt"});

    // Parameter file
    tst::write("parameters.txt", "import 1\nmutation 1");

    // Check that the program runs when importing the saved genotype file
    BOOST_CHECK_NO_THROW(doMain({"program", "parameters.txt"}));

    // Read in the saved genotype file (skip header and identifier column)
    const std::vector<double> genotypes = tst::readcsv("genotypes.csv", true, true);

    // Check that all values are 0 (fully de-mutated)
    bool iswrong = false;
    for (size_t i = 0u; i < genotypes.size(); ++i) {
        if (genotypes[i] != 0.0) {
            iswrong = true;
            break;
        }
    }
    BOOST_CHECK(!iswrong);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test error when cannot open allele import file
BOOST_AUTO_TEST_CASE(abuseCannotOpenAlleleImportFile) {

    // Write a parameter file with invalid import file name
    tst::write("parameters.txt", "import 1");

    // Check error
    BOOST_CHECK_THROW(doMain({"program", "parameters.txt"}), std::runtime_error);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");

}

// Test error when reading invalid genotype from import file
BOOST_AUTO_TEST_CASE(abuseInvalidGenotypeInImportFile) {

    // Write a parameter file with invalid import file name
    tst::write("parameters.txt", "import 1");

    // Write an invalid genotype file (skip header and identifier column)
    tst::write("genotypes.csv", "id,locus1\n1,3");

    // Check error
    BOOST_CHECK_THROW(doMain({"program", "parameters.txt"}), std::runtime_error);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");

}

// Test error when reading genotype out of bounds
BOOST_AUTO_TEST_CASE(abuseGenotypeOutOfBoundsInImportFile) {

    // Write a parameter file with invalid import file name
    tst::write("parameters.txt", "import 1");

    // Write an invalid genotype file (skip header and identifier column)
    tst::write("genotypes.csv", "id,locus1\n1,-1");

    // Check error
    BOOST_CHECK_THROW(doMain({"program", "parameters.txt"}), std::runtime_error);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");

}

// Test error when incorrect number of genotypes in import file
BOOST_AUTO_TEST_CASE(abuseIncorrectNumberOfGenotypesInImportFile) {

    // Write a parameter file with invalid import file name
    tst::write("parameters.txt", "import 1\npopsize 3\nnlocipertrait 3");

    // Write an invalid genotype file (skip header and identifier column)
    tst::write("genotypes.csv", "id,locus1,locus2,locus3\n1,0,0,0\n2,0,0,0");

    // Check error
    BOOST_CHECK_THROW(doMain({"program", "parameters.txt"}), std::runtime_error);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");

}

// Test error when too many genotypes are provided in import file
BOOST_AUTO_TEST_CASE(abuseTooManyGenotypesInImportFile) {

    // Expect 1 genotype entry (popsize=1, nloci=1)
    tst::write("parameters.txt", "import 1\npopsize 1\nnlocipertrait 1");

    // Provide 2 genotype entries to trigger the in-loop overflow check
    tst::write("genotypes.csv", "id,locus1\n1,0\n2,0");

    // Check exact error from the in-loop guard in gen::import
    tst::checkError(
        [&] { doMain({"program", "parameters.txt"}); },
        "Incorrect number of genotypes read from file genotypes.csv"
    );

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");

}

// Test error when incorrect format in import file
BOOST_AUTO_TEST_CASE(abuseIncorrectFormatInImportFile) {

    // Write a parameter file with invalid import file name
    tst::write("parameters.txt", "import 1");

    // Write an invalid genotype file (skip header and identifier column)
    tst::write("genotypes.csv", "id,locus1\n1,notanumber");

    // Check error
    BOOST_CHECK_THROW(doMain({"program", "parameters.txt"}), std::runtime_error);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");

}

// Test error when a data row is empty in import file
BOOST_AUTO_TEST_CASE(abuseEmptyRowInImportFile) {

    // Write a parameter file with import enabled
    tst::write("parameters.txt", "import 1");

    // Write an invalid genotype file with an empty data row
    // This should fail when trying to read the identifier column.
    tst::write("genotypes.csv", "id,locus1\n\n");

    // Check error
    BOOST_CHECK_THROW(doMain({"program", "parameters.txt"}), std::runtime_error);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");

}

// Test error when genotype value overflows integer in import file
BOOST_AUTO_TEST_CASE(abuseOutOfRangeValueInImportFile) {

    // Write a parameter file with import enabled
    tst::write("parameters.txt", "import 1");

    // Write an invalid genotype file with an integer too large for stoi
    tst::write("genotypes.csv", "id,locus1\n1,999999999999999999999999999999999");

    // Check error
    BOOST_CHECK_THROW(doMain({"program", "parameters.txt"}), std::runtime_error);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");

}

// Test the correct calculation of trait values
BOOST_AUTO_TEST_CASE(useCaseTraitValueCalculation) {

    // Write a parameter file with one trait and known architecture
    std::ostringstream content;
    content << "ntraits 1\n";
    content << "nlocipertrait 3\n";
    content << "nedgespertrait 0\n";
    content << "skews 0\n";
    content << "epistasis 0\n";
    content << "dominance 0\n";
    content << "envnoise 0\n";
    content << "loadarch 1\n";
    tst::write("parameters.txt", content.str());

    // Prepare a genetic architecture
    std::ostringstream arch;
    arch << "nloci 3\n";
    arch << "nedges 0\n";
    arch << "ntraits 1\n";
    arch << "traitids 1 1 1\n";
    arch << "effects 0.3 0.4 -0.1\n";
    arch << "dominances 0 0 0\n";
    tst::write("architecture.txt", arch.str());

    // Effect sizes as numbers
    const std::vector<double> effects = {0.3, 0.4, -0.1};

    // Run the simulation
    doMain({"program", "parameters.txt"});

    // Read in the saved genotype and trait files (skip header and identifier column)
    const std::vector<double> genotypes = tst::readcsv("genotypes.csv", true, true);
    const std::vector<double> traits = tst::readcsv("traits.csv", true, true);

    // Prepare expected values
    std::vector<double> expected(traits.size(), 0.0);

    // Genotype counter
    size_t k = 0u;

    // Flag
    bool iswrong = false;

    // For each individual...
    for (size_t i = 0u; i < traits.size(); ++i) {

        // Prepare
        double value = 0.0;

        // For each locus...
        for (size_t j = 0u; j < effects.size(); ++j) {

            // Update individual phenotype
            value += (genotypes[k] - 1.0) * effects[j];

            // Increment genotype counter
            ++k;

        }
        
        // If no match...
        if (value != traits[i]) {

            // Exit
            iswrong = true;
            break;

        }
    }

    // Make sure everything is fine
    BOOST_CHECK(!iswrong);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test the correct calculation of trait values with dominance
BOOST_AUTO_TEST_CASE(useCaseTraitValueCalculationWithDominance) {

    // Write a parameter file with one trait and known architecture
    std::ostringstream content;
    content << "ntraits 1\n";
    content << "nlocipertrait 3\n";
    content << "nedgespertrait 0\n";
    content << "skews 0\n";
    content << "epistasis 0\n";
    content << "dominance 1\n";
    content << "envnoise 0\n";
    content << "loadarch 1\n";
    tst::write("parameters.txt", content.str());

    // Prepare a genetic architecture
    std::ostringstream arch;
    arch << "nloci 3\n";
    arch << "nedges 0\n";
    arch << "ntraits 1\n";
    arch << "traitids 1 1 1\n";
    arch << "effects 0.3 0.4 -0.1\n";
    arch << "dominances 0.5 0.5 0.5\n";
    tst::write("architecture.txt", arch.str());

    // Effect sizes as numbers
    const std::vector<double> effects = {0.3, 0.4, -0.1};

    // Dominance coefficients as numbers
    const std::vector<double> dominances = {0.5, 0.5, 0.5};

    // Run the simulation
    doMain({"program", "parameters.txt"});

    // Read in the saved genotype and trait files (skip header and identifier column)
    const std::vector<double> genotypes = tst::readcsv("genotypes.csv", true, true);
    const std::vector<double> traits = tst::readcsv("traits.csv", true, true);

    // Prepare expected values
    std::vector<double> expected(traits.size(), 0.0);

    // Genotype counter
    size_t k = 0u;

    // Flag
    bool iswrong = false;

    // For each individual...
    for (size_t i = 0u; i < traits.size(); ++i) {

        // Prepare
        double trait = 0.0;

        // For each locus...
        for (size_t j = 0u; j < effects.size(); ++j) {

            // Compute genetic value
            double value = genotypes[k] - 1.0;

            // Add dominance deviation for heterozygotes
            if (value == 0.0)  value += dominances[j];

            // Update individual phenotype
            trait += value * effects[j];

            // Increment genotype counter
            ++k;

        }
        
        // If no match...
        if (trait != traits[i]) {

            // Exit
            iswrong = true;
            break;

        }
    }

    // Make sure everything is fine
    BOOST_CHECK(!iswrong);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test the correct calculation of trait values with epistasis
BOOST_AUTO_TEST_CASE(useCaseTraitValueCalculationWithEpistasis) {

    // Write a parameter file with one trait and known architecture
    std::ostringstream content;
    content << "ntraits 1\n";
    content << "nlocipertrait 3\n";
    content << "nedgespertrait 3\n";
    content << "skews 0\n";
    content << "epistasis 0.1\n";
    content << "dominance 0\n";
    content << "envnoise 0\n";
    content << "loadarch 1\n";
    tst::write("parameters.txt", content.str());

    // Prepare a genetic architecture
    std::ostringstream arch;
    arch << "nloci 3\n";
    arch << "nedges 3\n";
    arch << "ntraits 1\n";
    arch << "traitids 1 1 1\n";
    arch << "effects 0.3 0.4 -0.1\n";
    arch << "dominances 0 0 0\n";
    arch << "from 1 2 3\n";
    arch << "to 2 3 1\n";
    arch << "weights 0.1 0.1 0.1\n";
    tst::write("architecture.txt", arch.str());

    // Key values as numbers
    std::vector<double> effects = {0.3, 0.4, -0.1};
    std::vector<double> weights = {0.1, 0.1, 0.1};

    // Run the simulation
    doMain({"program", "parameters.txt"});

    // Read in the saved genotype and trait files (skip header and identifier column)
    const std::vector<double> genotypes = tst::readcsv("genotypes.csv", true, true);
    const std::vector<double> traits = tst::readcsv("traits.csv", true, true);

    // Prepare expected values
    std::vector<double> expected(traits.size(), 0.0);

    // Genotype counter
    size_t k = 0u;

    // Flag
    bool iswrong = false;

    // For each individual...
    for (size_t i = 0u; i < traits.size(); ++i) {

        // Prepare
        double trait = 0.0;

        // For each locus...
        for (size_t j = 0u; j < effects.size(); ++j) {

            // Compute genetic value
            trait += (genotypes[k] - 1.0) * effects[j] * 0.9;

            // Increment genotype counter
            ++k;

        }

        // Epistatic contributions
        const double edge01 = weights[0u] * (genotypes[i * 3u + 0u] - 1.0) * (genotypes[i * 3u + 1u] - 1.0);
        const double edge12 = weights[1u] * (genotypes[i * 3u + 1u] - 1.0) * (genotypes[i * 3u + 2u] - 1.0);
        const double edge20 = weights[2u] * (genotypes[i * 3u + 2u] - 1.0) * (genotypes[i * 3u + 0u] - 1.0);

        // Update individual phenotype
        trait += (edge01 + edge12 + edge20) * 0.1;

        // If no match...
        if (fabs(trait - traits[i]) > 1e-6) {

            // Exit
            iswrong = true;
            break;

        }
    }

    // Make sure everything is fine
    BOOST_CHECK(!iswrong);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}

// Test the correct calculation of trait values with epistasis and dominance
BOOST_AUTO_TEST_CASE(useCaseTraitValueCalculationWithEpistasisAndDominance) {

    // Write a parameter file with one trait and known architecture
    std::ostringstream content;
    content << "ntraits 1\n";
    content << "nlocipertrait 3\n";
    content << "nedgespertrait 3\n";
    content << "skews 0\n";
    content << "epistasis 0.1\n";
    content << "dominance 1\n";
    content << "envnoise 0\n";
    content << "loadarch 1\n";
    tst::write("parameters.txt", content.str());

    // Prepare a genetic architecture
    std::ostringstream arch;
    arch << "nloci 3\n";
    arch << "nedges 3\n";
    arch << "ntraits 1\n";
    arch << "traitids 1 1 1\n";
    arch << "effects 0.3 0.4 -0.1\n";
    arch << "dominances 0.2 0.2 0.2\n";
    arch << "from 1 2 3\n";
    arch << "to 2 3 1\n";
    arch << "weights 0.1 0.1 0.1\n";
    tst::write("architecture.txt", arch.str());

    // Key values as numbers
    std::vector<double> effects = {0.3, 0.4, -0.1};
    std::vector<double> weights = {0.1, 0.1, 0.1};
    std::vector<double> dominances = {0.2, 0.2, 0.2};

    // Run the simulation
    doMain({"program", "parameters.txt"});

    // Read in the saved genotype and trait files (skip header and identifier column)
    const std::vector<double> genotypes = tst::readcsv("genotypes.csv", true, true);
    const std::vector<double> traits = tst::readcsv("traits.csv", true, true);

    // Prepare expected values
    std::vector<double> expected(traits.size(), 0.0);

    // Genotype counter
    size_t k = 0u;

    // Flag
    bool iswrong = false;

    // For each individual...
    for (size_t i = 0u; i < traits.size(); ++i) {

        // Prepare
        double trait = 0.0;

        // For each locus...
        for (size_t j = 0u; j < effects.size(); ++j) {

            // Prepare genetic value
            double value = genotypes[k] - 1.0;

            // Add dominance deviation for heterozygotes
            if (value == 0.0) value += dominances[j];
            
            // Update individual phenotype
            trait += value * effects[j] * 0.9;

            // Increment genotype counter
            ++k;

        }

        // Gene expression
        double exp0 = (genotypes[i * 3u + 0u] - 1.0);
        double exp1 = (genotypes[i * 3u + 1u] - 1.0);
        double exp2 = (genotypes[i * 3u + 2u] - 1.0);

        // Add dominance to heterozygotes
        exp0 += (exp0 == 0.0) * dominances[0u];
        exp1 += (exp1 == 0.0) * dominances[1u];
        exp2 += (exp2 == 0.0) * dominances[2u];

        // Epistatic contributions
        const double edge01 = weights[0u] * exp0 * exp1;
        const double edge12 = weights[1u] * exp1 * exp2;
        const double edge20 = weights[2u] * exp2 * exp0;

        // Update individual phenotype
        trait += (edge01 + edge12 + edge20) * 0.1;

        // If no match...
        if (fabs(trait - traits[i]) > 1e-6) {

            // TOREMOVE
            std::cout << "Trait value mismatch for individual " << i << ": expected " << trait << ", got " << traits[i] << std::endl;

            // Exit
            iswrong = true;
            break;

        }
    }

    // Make sure everything is fine
    BOOST_CHECK(!iswrong);

    // Cleanup
    std::remove("parameters.txt");
    std::remove("paramlog.txt");
    std::remove("architecture.txt");
    std::remove("genotypes.csv");
    std::remove("traits.csv");

}