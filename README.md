This is a proof-of-concept for deterministic generation of ring members in Monero.
It implements an invertible gamma distribution and an invertible pseudo-random number generator (PRNG) with a 160-bit seed.

## Build

```
git clone https://github.com/tevador/igamma.git
cd igamma
mkdir build
cd build
cmake ..
make
```

## Sender

The sender would not specify the ring members by their index, but would instead include a 160-bit seed that can be used to deterministically
generate the ring members.

To generate the seed, specify a randomly generated index of the real output and the required output value
of `exp(z)`, where `z` is a gamma-distributed random variable. The output is a seed value that produces a sequence of
gamma-distributed values with the age of the real output at the required index:

```
> ./igamma --invert --real-output 9 --value 3.14159e+11
Generated seed: a2778a993ccf44be18809a1ff47a04a722677e9b
Generated ring member ages:
  1.27264e+06
  271922
  6.1305e+06
  683335
  128029
  371122
  1872.07
  1.0268e+07
  15694
  3.14159e+11 <------
  7243.3
```

The advantages of this approach:

* All ring members are encoded as a 20-byte seed value regardless of the actual number of ring members.
* Enforces random selection of ring members because only a single index can be specified when generating the seed.

Disadvantages:

* Introduces floating point math in consensus-critical code.
* Requires the transaction to somehow specify an anchor point from which the gamma distribution is generated (e.g. block height).
* Slightly more computation is required to recover the ring members.

## Verification

Verifiers can regenerate the sequence from the seed:

```
> ./igamma --gen --seed a2778a993ccf44be18809a1ff47a04a722677e9b
Generated ring member ages:
  1.27264e+06
  271922
  6.1305e+06
  683335
  128029
  371122
  1872.07
  1.0268e+07
  15694
  3.14159e+11
  7243.3
```

The real output index cannot be determined from the seed.

## Implementation details

* The seed must be 160 bits long because that's the minimum size that can reversibly encode three 52-bit integers needed to generate a gamma-distributed value using the Marsaglia and Tsang's Method.
* The PRNG is AES-based. The PoC code requires a CPU that supports the AES-NI instruction set.
* The PRNG requires a unique 32-byte key per output (the key image can be used since it's public and already included in the transaction). A unique key per output ensures that different outputs cannot use the same set of decoys.
* Seed generation requires a secure PRNG (the PoC code uses a LCG-based generator for simplicity).

