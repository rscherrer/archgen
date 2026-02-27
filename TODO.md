## Things left to do

### Code

These are possible small changes for the code as it is now.

* Allow the program to run with just a warning when not all edges can be made when generating the genetic architecture
* Right now, if `binary` is `1`, the program compresses alleles into 64-bit (integral) numbers upon saving, but this can only be done on a 64-bit system. Running this program on a 32-bit system will save erroneous data. Could we find a way to make it work for any system? (Note that the size of a bitset is hard-coded in C++; maybe use aliases and templates, or store in 32 bits and save two in one if running on 64?)
* The `check()` function of the `Architecture` class loops a lot to perform assertions. In release mode, these assertions are not performed but the loops are still there. Is there a way to avoid that?
* I think technically the `ratio` parameter will not allow full shuffle ever if it is greater than 0.5 (because when more than half of the alleles must mutate we first mutate everything, then mutate back some). Change accepted value?

### Documentation

Suggested changes for the documentation.

* Explain how trait values are generated
* Contribution guidelines?

### Possible extensions

These are larger avenues for extension.

* Discuss recombination and linkage
* Pleiotropy
* Flexible ploidy

### For other projects

* In [readpars](https://github.com/rscherrer/readpars): say that extra checks may be performed after reading in parmeter values, typically checks that are very specific to each parameter being read (e.g. not just whether it is strictly positive or between zero and one)
* In [readpars](https://github.com/rscherrer/readpars): make it work with string parameters? and vectors of strings? maybe single characters?
* In [readpars](https::github.com/rscherrer/readpars): is code coverage 100%? Maybe thoroughly test the parsing error handling for different types of numerical input because here the boolean case seemed not to be covered.
