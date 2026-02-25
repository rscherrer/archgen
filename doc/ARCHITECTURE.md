## Genetic architecture

Details of the genetic architecture are stored in a separate parameter file called `architecture.txt`. This file can be supplied in the working directory and will be read if the `loadarch` parameter is set to `1` (see [here](PARAMETERS.md) for a guide on regular parameters), or it can be generated anew and saved from within the program (if `loadarch` is set to `0`and `savearch` is set to `1`).

The archicture file is similarly constructed to the [general parameter file](PARAMETERS.md) and should look something like this:

```
nloci 5
nedges 4
ntraits 2
traitids 1 1 1 2 2
effects 0.1 0.1 0.1 0.1 0.1
dominances 0.05 0.05 0.05 0.05 0.05
from 1 1 2 4
to 2 3 3 5
weights 0.1 0.2 0.3 0.4
```

where each parameter's value(s) is given on a separate line. The above states that:

* there are 5 loci in the genome
* there are 4 epistatic interactions in total
* 2 different traits are being encoded
* the first three loci affect trait 1, while the last two affect trait 2
* the additive effect size of all loci is 0.1
* the dominance coefficient of all loci is 0.05
* the 4 epistatic interactions are between loci 1 and 2, 1 and 3, 2 and 3, and 4 and 5
* the weights of these interactions are 0.1, 0.2, 0.3 and 0.4, respectively

Below is a more detailed description of each parameter of the genetic architecture.

| Parameter name | Accepted values | No. values | Description | Notes |
|--|--|--|--|--|
| `nloci` | Strictly positive integers | 1 | Number of loci in the genome | |
| `nedges` | Positive integers | 1 | Number of edges across the gene networks for all traits | |
| `ntraits` | Strictly positive integers | 1 | Number of traits in the model | | 
| `traitids` | Strictly positive integers from `1` to `ntraits` | `nloci` | Trait affected by each locus | |
| `effects` | Decimals | `nloci` | Additive effect size of each locus | |
| `dominances` | Decimals | `nloci` | Dominance coefficient of each locus | |
| `from` | Strictly positive integers from `1` to `nloci` | `nedges` | Starting locus of each epistatic interaction | See below |
| `to` | Strictly positive integers from `1` to `nloci` | `nedges` | Ending locus of each epistatic interaction | See below |
| `weights` | Decimals | `nedges` | Weight of each epistatic interaction | |

Make sure to avoid linking loci to themselves when supplying `from` and `to`. For example,

```
from 1 1 2 4
to 1 3 3 5
```

is attempting to connect locus 1 with itself. This will trigger an error.

Please also make sure that the connected loci code for the same trait. The following:

```
from 1 1 2 4
to 2 3 4 5
```

will trigger an error because locus 2 is connected to locus 4, but locus 2 affects trait 1 while locus 4 affects trait 2. Note that some loci may be left unconnected.

It is important that certain parameters be supplied in the correct order. For example, `nloci` should be given before `traitids`, `effects` and `dominances`, and `nedges` should be given before `from`, `to` and `weights`. Additionally, `nloci` should be given before `from` and `to` so that the program can check that the locus indices are within bounds, and `ntraits` should be given before `traitids` so encoded traits an be checked for validity. If unsure, just stick to the order in which parameters are listed in the example above.

**Note** that generating a new architecture (`loadarch` is `1`) will override (if needed) the following general parameters: `nloci`, `nedges`, and `ntraits` (see [here](PARAMETERS.md) for details). These parameters will be updated in the output parameter log file (`paramlog.txt`) to reflect the true values with which the architecture was built. For example, if `nloci` is set to 10 in the general parameter file, but the architecture file specifies `nloci` as 5, then the program will use 5 loci for generating the architecture and will update `nloci` to 5 in the output parameter log file. 

Parameters `epistasis`, `dominance` and `envnoise` (see [here](PARAMETERS.md)), which are used in the translation of genotypes into phenotypes, remain properties of the regular parameter file. Please **make sure that they match the number of traits** in case a new architecture is provided (the program will error otherwise).

### Simulation algorithm

If `loadarch` is `0`, a new architecture will be generated according to the general parameters provided (see [here](PARAMETERS.md)). Here is a description of how the algorithm works.

First, the program generates `nloci` loci and assigns them encoded `traitids` based on the `nlocipertrait` parameters. All loci are given `effects` sampled from a normal distribution with mean zero and standard deviations as given by the general parameters `effect`, as well as `dominances` sampled from normal distribution with mean zero and standard deviation one. Then, `nedgespertrait` are generated for each respective trait to form a gene network for that trait (the networks are independent among traits, i.e. there is no pleiotropy). Edges are generated according to the preferential attachment algorithm of Barabasi and Albert (1999), which, starting with a single edge between two vertices, loops through remaining vertices and attaches them randomly to already attached vertices, with probability proportional to the degree of the already attached vertices. This algorithm is known to generate scale-free networks, which are common in biology. The `weights` of the edges are sampled from a normal distribution with mean zero and standard deviation as given by the general parameter `weight`. Note that the program will attempt to generate the requested number of edges, but it may not always be possible to do so (e.g. if `nedgespertrait` is very high). In this case, an error will be triggered. The `nedgespertrait` should sum up to `nedges`. Also keep in mind that the maximum number of edges for a given number of vertices `n` is `n * (n - 1) / 2` (full graph). An error will be thrown if the requested number of edges is greater than this for any trait (`n` being the `nlocipertrait` of the respective trait).

### References

Barabási, A.-L., & Albert, R. (1999). Emergence of scaling in random networks. Science, 286(5439), 509–512. https://doi.org/10.1126/science.286.5439.509