## Things left to do

### Code

These are possible small changes for the code as it is now.

* Maybe some information is redundant in parameter files (e.g. `nloci` and `nlocipertrait`). Perhaps some internal update of some parameters would be nice.
* Allow the program to run with just a warning when not all edges can be made when generating the genetic architecture
* Right now, if `binary` is `1`, the program compresses alleles into 64-bit (integral) numbers upon saving, but this can only be done on a 64-bit system. Running this program on a 32-bit system will save erroneous data. Could we find a way to make it work for any system? (Note that the size of a bitset is hard-coded in C++; maybe use aliases and templates, or store in 32 bits and save two in one if running on 64?)
* What happens with architecture reading if zero edges are supplied? Should the `from`, `to` and `weights` field be absent? If they are present, will the ReadPars library exit before reading? Is the pipeline all right when edge-specific containers are empty?

### Documentation

Suggested changes for the documentation.

* Explain how trait values are generated

### Possible extensions

These are larger avenues for extension.

* Discuss recombination and linkage
* Pleiotropy
* Flexible ploidy

### For other projects

* In [readpars](https://github.com/rscherrer/readpars): say that extra checks may be performed after reading in parmeter values, typically checks that are very specific to each parameter being read (e.g. not just whether it is strictly positive or between zero and one)
* In [readpars](https://github.com/rscherrer/readpars): make it work with string parameters? and vectors of strings? maybe single characters?