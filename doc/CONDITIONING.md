## Conditioning on heritability

The program offers the possibility to supply genetic effects (locus effect sizes, dominance coefficients, interaction parameters, see [parameter](PARAMETERS.md) and [architecture](ARCHITECTURE.md) documentation) and environmental noise separately and independently of each other, but can also accommodate conditioning the environmental noise so as to reach a certain target heritability for each trait, given the additive effect sizes of the (generated or loaded) genetic architecture.

Here, heritability refers to the broad-sense heritability (narrow-sense in simulations with only additive genetic effects).

This is done by setting the `conditioned` parameter to `1` (see [here](PARAMETERS.md)). If this is the case, the program will update the `envnoise` parameter to match the given `heritability` values given the generated architecture. If `conditioned` is `0`, the program will not update the `envnoise` parameter and the `heritability` values will be ignored.

The conditioning is performed using the equation $V_\text{P} = V_\text{G} + V_\text{E} = V_\text{A} / H^2$, where $V_\text{P}$ is the phenotypic variance, $V_\text{G}$ is the genetic variance, $V_\text{E}$ is the environmental variance, $V_\text{A}$ is the additive genetic variance and $H^2$ is the broad-sense heritability. From this equation, we can derive the following formula for the environmental variance:

$$V_\text{E} = V_\text{G} / H^2 - V_\text{G}$$

where $V_\text{G}$ is computed as a sample variance from the simulated data.

Note that conditioning will change the `envnoise` parameter internally, but the program will not save that updated value in the output parameter file `paramlog.txt`, which is saved if `savepars` is set to `1` (see [here](PARAMETERS.md)). This is because conditioning will cause `envnoise` to be updated for each replicate simulation separately, and we do not want to have to save one `paramlog.txt` file for each replicate (instead we only save one).