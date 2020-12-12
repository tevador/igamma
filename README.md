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
* The PRNG requires a unique 32-byte key per output (the key image can be used since it's public and already included in the transaction). A unique key per output ensures that different outputs cannot use the same set of decoys. The PoC uses a fixed key.
* Seed generation requires a CSPRNG. The PoC uses a ChaCha20-based generator.

## Bounty

In effort to find flaws in this approach, the following 3 XMR bounty is offered:

Given the following 10 seed values:

```
1a838224c31f2bb4763b183d88d9b2c93af45fed
67d74e878c20961320f1ecfdd7611f6388a4a665
9e59b94606cd0fda671fd251889e90c144ebe5fd
d40b82834572c803bf1ff05c31521b780ce32aa3
51d98c963a8778041d49872221d1dcdec91eee21
184e45884af9990e1b65c9d40ab88cd7c0b92ae8
24059f13f8424a137b5956bf6876d4f12674881b
f81dbf3b615f87c02817096688e2a4ce7d220e5d
f7d7ff6d3a62983bdade34ad4e2460b255d19068
e8d942dc4861ac6ccef9c9e4e2f31b51b620cf3c
```

The ring member ages can be recovered using:

```
./igamma --gen --seed <seed>
```

The goal is to select 4 ring members (out of 11) for each of the 10 seeds so that the real output is among the selected members (partial deanonymization).

Solution commitments (SHA256):

```
f746ec7bc88f5e4cc09c9c0a44a976aa03601aa34be2227bea0848c10acf0fbf
7bc49f19a484f61a0d336d7cc35e64ba8217ba1d47992e39ae2708bb3b731a1e
4bc812c31ec170c9c8db6491b3c5e02e7b4d71b1fffa387e11813d0bcfe5a785
dbee29aaef6b7a04d6023fe2c7ce6899201775c4dd1a00984679f5b52333cf4c
89f9a14c6ed378fe410d3869d36d321863240bc4548784c9f074368a3dcdd731
57ccb1cc79b3a6e097c971c39de0016c2c67b76bd07a9bc1c4b45f5f15efbd51
e7d2591dbb3c9915d1c3183b9e23e710db33ae8ad5a741441d709c42efb8b44e
126864cf077e794c7dbf384fa4789294419495f126d8abf3e045791283e36c86
c705059753a026cac0d9cf9f81c48cf8854ae627335feebabbad125e04369266
92aee9f2b56800ce4dd501ccc972fed6b6bf1d4d1ebcb9ab28a260a48b6afd74
```

### Conditions

Solutions must be sent to tevador (at) gmail [dot] com and must include:

* four selected ring members for each of the 10 seeds
* description of the method that was used to select the ring members
* XMR address

The bounty expires when the first of the following occurs:
* a correct solution is received
* 31st January 2021 22:59 UTC

I reserve the right to disqualify solutions obtained by guessing (the chance is about 1:25000).
