# ArchGen: Simple Genetic Architecture and Quantitative Genetic Data Simulator 

This is the source code for a simple simulator of alleles and trait values in a population, for use in quantitative genetics method testing and benchmarking.

![Build](https://img.shields.io/badge/build-passing-brightgreen)
![Tests](https://img.shields.io/badge/tests-passing-brightgreen)
![Coverage](https://img.shields.io/badge/coverage-100%25-brightgreen)

## Description

This program simulates a population of individuals, each characterized by a set of (diploid) genotypes and trait values. The genotype-phenotype map used to generate the trait values can be supplied, or it can be generated randomly. It includes additive effects, dominance coefficients, epistatic interactions and environmental noise. There may be multiple traits, and the user chooses the amount of initial genetic variation in the population.

## Installation

This program must be compiled. It is written in C++ and can be built using any modern C++ compiler and build system. [Here](doc/SETUP.md) we provide an example setup to build the program on several platforms using CMake.

## Usage

This program has a command-line interface. Assuming the name of the compiled executable is `archgen`, use the following command in the terminal from the working directory to run a simulation with default parameter values:

```shell
./archgen
```

To run a simulation with custom parameters, use:

```shell
./archgen parameters.txt
```

where `parameters.txt` is a text file containing a list of parameter names and values.

### Parameters

The parameter file should look like this:

```
popsize 100
nloci 300
nedges 3000
ntraits 3
allfreq 0.2
effect 0.1
nlocipertrait 100 100 100
nedgespertrait 1000 1000 1000
skews 1 1 1
epistasis 0.5 0.5 0.5
dominance 0.3 0.3 0.3
envnoise 0.2 0.2 0.2
sampling 1
ratio 0.25
seed 12345
loadarch 0
savearch 1
savepars 1
binary 0
verbose 1   
```

In the parameter file, each line should contain a parameter name followed by its value. Lines starting with **#** will be treated as comments. Click [here](doc/PARAMETERS.md) for a guide of the available parameters and their meaning. Please note that **the order of parameters in the file matters**, as some are needed to know how many values to read for others. Please keep them in the order shown above or in the parameter guide. Parameters that are omitted will take default values.

### Genetic architecture

The program simulates trait values based on a genetic architecture, composed of additive effect sizes of loci, dominance coefficients, and epistatic interaction effects. Since parameter values for these are numerous (one per locus or one per pair of interacting loci), they are best stored in a separate parameter file called `architecture.txt`. If the `loadarch` parameter is set to `1`, the program will look for this file in the working directory and load the genetic architecture from it. If `loadarch` is set to `0`, a random genetic architecture will be generated based on parameters supplied in the regular parameter file (as described [here](doc/PARAMETERS.md)). If `savearch` is set to `1`, the genetic architecture will be saved to `architecture.txt` in the working directory. See [this page](doc/ARCHITECTURE.md) for details about how this architecture file is formatted.

### Output

The program outputs two main data files. The first, `traits.csv`, contains the trait values of all individuals in the population (individuals in rows, traits in columns). The second, `alleles.dat` (or `alleles.csv` if the `binary` parameter is set to `0`), contains the genotypes of all individuals in the population (individuals in rows, loci in columns). Note that in the latter, the number of columns is twice the number of loci, with the first half containing the first allele for each locus and the second half containing the second allele for each locus (diploid genetics). See [here](doc/OUTPUT.md) for details.

## Tests

This program was tested using the [Boost.Test](https://www.boost.org/doc/libs/1_85_0/libs/test/doc/html/index.html) library. All the tests can be found in the `tests/` folder. [Here](doc/TESTS.md) we show how we tested the program locally using our own custom setup.

## About

This code is written in C++20. It was developed on Ubuntu Linux 24.04 LTS, making mostly use of [Visual Studio Code](https://code.visualstudio.com/) 1.99.0 ([C/C++ Extension Pack](https://marketplace.visualstudio.com/items/?itemName=ms-vscode.cpptools-extension-pack) 1.3.1). [CMake](https://cmake.org/) 3.28.3 was used as build system, with [g++](https://gcc.gnu.org/) 13.3.0 as compiler. [GDB](https://www.gnu.org/savannah-checkouts/gnu/gdb/index.html) 15.0.50.20240403 was used for debugging. Tests (see [here](doc/TESTS.md)) were written with [Boost.Test](https://www.boost.org/doc/libs/1_85_0/libs/test/doc/html/index.html) 1.87, itself retrieved with [Git](https://git-scm.com/) 2.43.0 and [vcpkg](https://github.com/microsoft/vcpkg) 2025.04.09. Memory use was checked with [Valgrind](https://valgrind.org/) 3.22.0. Code coverage was analyzed with [gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html)  v13.3.0, and [LCOV](https://github.com/linux-test-project/lcov) 2.0-1. Profiling was performed with [gprof](https://ftp.gnu.org/old-gnu/Manuals/gprof-2.9.1/html_mono/gprof.html) 2.42. (See the `dev/` folder and [this page](dev/README.md) for details about the checks performed.) During development, occasional use was also made of [ChatGPT](https://chatgpt.com/) and [GitHub Copilot](https://github.com/features/copilot).

## Links

* [archgen](https://github.com/rscherrer/archgen): source code

Optional extra links:

* [readsim](https://github.com/rscherrer/readsim): assemble binary simulation data in R
* [speciome](https://github.com/rscherrer/speciome): simulation of speciation with gene networks

## Permissions

Copyright (c) Raphaël Scherrer, 2026.

This code is licensed under the MIT license (see [license](LICENSE.txt) file). This project is inspired from the genetic architecture implementation of a [previous work](https://github.com/rscherrer/speciom) by Raphaël Scherrer and G. Sander van Doorn. This code comes with no guarantee whatsoever.
