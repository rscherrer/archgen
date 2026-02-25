## Mutation sampling

The population is initialized as a large array of `2 * nloci` alleles per individual, where all alleles are set to 0 (alleles are binary, either 0 or 1). The parameter `allfreq` (see [here](PARAMETERS.md)) specifies the desired frequency of the 1-allele in the population, and mutations (from 0 to 1) are therefore randomly sampled across the genomes of individuals to reach that frequency. The type of algorithm used to do that is given by the `sampling` parameter.

### Given value

If `sampling` is set to `0`, the program will take the requested `allfreq` as given and will throw (nearly) exactly that frequency of mutations across the genome (randomly picking between floor and ceiling if `allfreq * nloci` is not an integer).

### Stochastic sampling

The other algorithms are more stochastic and will sample mutations with probability `allfreq`, but the exact outcome may deviate from that target frequency due to the variance introduced by the stochasticity of the sampling process. Note that in what follows, the choice of the most efficient algorithm **depends on how far** `allfreq` **is from 0.5** (as mutating many alleles is equivalent to picking just a few alleles not to mutate)(*).

#### Bernoulli sampling

If `sampling` is `1`, the program will sample mutations through Bernoulli sampling, i.e. each allele will be mutated with probability `allfreq` independently of the others. This is the simplest but also also perhaps the slowest algorithm, and should perhaps only be used for testing purposes. It might be worth using when `allfreq` is close to 0.5.

#### Binomial sampling

If `sampling` is `2`, the program will first sample the number of mutations from a binomial distribution with number of trials `nloci` and success probability `allfreq`, and then scatter that number of mutations randomly across the genome. This is more efficient than Bernoulli sampling, especially when `allfreq` is substantially smaller or larger than 0.5.

#### Geometric sampling

If `sampling` is `3`, the program will sample the position of the next mutation from a geometric distribution with success probability `allfreq`. This is the most efficient algorithm when `allfreq` is very close to 0 or to 1.

### Summary

| `sampling` value | Algorithm | Efficiency |
|--|--|--|
| `1` | Bernoulli sampling | Simple but potentially slow, to be used only when `allfreq` is close to 0.5 |
| `2` | Binomial sampling | More efficient than Bernoulli sampling, especially when `allfreq` is substantially smaller or larger than 0.5 |
| `3` | Geometric sampling | Most efficient when `allfreq` is very close to 0 or to 1 |

(*) In fact, for the geometric (`sampling 3`) and the binomial (`sampling 2`) algorithms, all alleles are first mutated if the (expected, for geometric, or realized, for binomial) number of mutations is more than half of all alleles, and only then some alleles are mutated back to their original state. By sampling the rarer event, we minimize the number of costly operations to perform.

### Shuffling ratio

When `sampling` is `0` (given) or `2` (binomial), the program will use random shuffling to sample (with replacement) the indices of the mutated loci. Shuffling the entire array of allele indices can be unnecessarily expensive when the number of alleles to flip is small relative to the total number of alleles. To increase efficiency, we offer parameter `ratio`, which determines the (realized) frequency of the rarest allele under which the shuffle algorithm is only partial (indices early in the list of alleles are swapped with random indices further away, see source code for details). The value of `ratio` should be relatively small for best results, and probably does not need to be changed from its [default](PARAMETERS.md).