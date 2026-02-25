## Output data

### Parameters

If `savepars` is `1` the program saves the parameters it has used to run the simulation in a parameter log file called `paramlog.txt`. These include the `seed` of the pseudo-random number generator and any parameters that have been updated (e.g. by reading them from file). See [here](PARAMETERS.md) for details about parameters.

### Genetic architecture

If `savearch` is `1`, the program saves the genetic architecture of the traits in a file called `architecture.txt`. This file contains the list of loci affecting each trait, the edges in the gene network of each trait, and the effect sizes, dominance coefficients and weights of all loci and edges. See [here](ARCHITECTURE.md) for details about how this file is formatted.

### Trait values

The program saves the trait values of all individuals in a file called `traits.csv`. This file contains one row per individual and one column per trait, with the value in each cell corresponding to the trait value of the individual for that trait.

### Allele data

The program saves the genotypes of all individuals in a file called `alleles.dat` (or `alleles.csv` if the `binary` parameter is set to `0`). This file contains one row per individual and two columns per locus, with the value in each cell corresponding to the allele value (0 or 1) of the individual for that locus. The first half of the columns correspond to the first allele for each locus, and the second half correspond to the second allele for each locus (diploid genetics). Like so:

```
           Hapl. 1    Hapl. 2
     Loc. 1 2 3 4 5  1 2 3 4 5
     --------------------------
Ind. 1 -> 0 0 1 1 0  0 1 1 0 1
Ind. 2 -> 1 0 0 0 1  1 0 0 1 1
Ind. 3 -> 0 1 0 0 0  1 1 0 0 1
```

**Note** that if `binary` is set to `1`, the file `alleles.dat` will be saved in binary format, which is more compact and faster to read/write but not human-readable. To read this file, use a binary file reader that can handle the data format used (e.g., in [R](https://r-project.org), you can use the `readBin` function). Make sure to read as single bits to retrieve the allele values (0 or 1). You can also check the [readsim](https://github.com/rscherrer/readsim) package, designed to allow reading this type of data in R.

Also note that you **should make sure that you know how many alleles there are to read in total** if the file is in binary, which should be `2 * nloci` (two alleles per locus) times `popsize` (number of individuals in the population, see parameter [guide](PARAMETERS.md) for details). This is because the binary file is written by coercing sets of bits (the alleles) into 64-bit (unsigned long) integers. Therefore, the total number of bits saved is a multiple of 64, but the total number of alleles may not be. If the total number of alleles is not a multiple of 64, the last integer in the file will contain some padding bits that should be ignored when reading the data.

#### Known issue

Because the program encodes allele data into 64-bit integers when `binary` is `1`, this option can only accurately be used on a 64-bit system, and will save erroneous data if run on a 32-bit system. If running a 32-bit system, please set `binary` to `0` in order to save alleles as text.