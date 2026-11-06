# MadGraph

![Docker Hub](https://img.shields.io/docker/v/startown/madgraph?sort=date&label=docker%20hub&logo=docker&logoColor=white&style=flat-square) ![Image size](https://img.shields.io/docker/image-size/startown/madgraph/3.5.17-1?label=size&style=flat-square) ![License](https://img.shields.io/github/license/startown-ai/madgraph?style=flat-square)

MadGraph5_aMC@NLO with Pythia and Delphes.

## Installation

Pull the image:

```bash
docker pull startown/madgraph:3.5.17-1
```

## Usage

Start MadGraph interactively:

```bash
docker run -it --rm -v "$PWD":/data -w /data startown/madgraph:3.5.17-1 mg5_aMC
```

Or run a script:

```bash
docker run --rm -v "$PWD":/data -w /data startown/madgraph:3.5.17-1 mg5_aMC commands.mg5
```

For example, this `commands.mg5` generates 1,000 top-quark pairs with parton shower and detector simulation. The fixed core count and random seeds make it reproducible:

```
set nb_core 4

generate p p > t t~
output pp2tt
launch

shower=Pythia8
detector=Delphes
done

set nevents 1000
set iseed 1
add pythia8_card Random:setSeed = on
add pythia8_card Random:seed = 1
add delphes_card --line_position=0 set RandomSeed 1
done
```

The results land in `pp2tt/Events/run_01/`:

| File                            | Contents                           |
| ------------------------------- | ---------------------------------- |
| `run_01_tag_1_banner.txt`       | every card and setting of the run  |
| `unweighted_events.lhe.gz`      | parton-level events                |
| `tag_1_pythia8_events.hepmc.gz` | particle-level events from Pythia  |
| `tag_1_delphes_events.root`     | detector-level events from Delphes |

## License

This repository is licensed under the [Apache License 2.0](LICENSE), while the packages it builds keep their own licenses:

| Package                          | Version | License            |
| -------------------------------- | ------- | ------------------ |
| FastJet                          | 3.5.3   | GPL-2.0-or-later   |
| fjcontrib                        | 1.105   | GPL-2.0-or-later   |
| HepMC<br>+ `named-weights.patch` | 2.06.11 | GPL-2.0-or-later   |
| LHAPDF                           | 6.5.6   | GPL-3.0            |
| ROOT                             | 6.36.14 | LGPL-2.1-or-later  |
| Delphes                          | 3.5.1   | GPL-3.0-or-later   |
| eMELA                            | 1.0.1   | MIT                |
| Pythia                           | 8.317   | GPL-2.0-or-later   |
| MadGraph5_aMC@NLO                | 3.5.17  | NCSA-style license |
