# MadGraph

[![Docker Hub](https://img.shields.io/docker/v/startown/madgraph?sort=semver&label=docker%20hub&logo=docker&logoColor=white&style=flat-square)](https://hub.docker.com/r/startown/madgraph) [![Image size](https://img.shields.io/docker/image-size/startown/madgraph/3.8.1?label=size&style=flat-square)](https://hub.docker.com/r/startown/madgraph/tags) [![License](https://img.shields.io/github/license/startown-ai/madgraph?style=flat-square)](LICENSE)

A containerized MadGraph5_aMC@NLO with Pythia and Delphes.

## Installation

Pull the image:

```bash
docker pull startown/madgraph:3.8.1
```

## Usage

Start MadGraph interactively:

```bash
docker run -it --rm -v "$PWD":/data startown/madgraph:3.8.1 mg5_aMC
```

Or run a script:

```bash
docker run --rm -v "$PWD":/data startown/madgraph:3.8.1 mg5_aMC commands.mg5
```

For example, this `commands.mg5` generates 1,000 top-quark pairs with parton shower and detector simulation. The fixed core count and random seed make it reproducible:

```
# Fix the core count of every stage
set nb_core 4

generate p p > t t~
output pp2tt
launch

shower=Pythia8
detector=Delphes
done

set nevents 1000

# Fix the random seed of every stage
set iseed 1

# Keep the HepMC file, which this image removes by default
set HEPMCoutput:file hepmc.gz
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

| Package                                                                                                                                                                                           | Version | License            |
| ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------- | ------------------ |
| MadGraph5_aMC@NLO<br>+ `seeds.patch`<br>+ `cores.patch`<br>+ `quiet-warnings.patch`<br>+ `delphes-stream.patch`<br>+ `fast-lhe-split.patch`<br>+ `hadd-no-j.patch`<br>+ `delphes-inprocess.patch` | 3.8.1   | NCSA-style license |
| Pythia                                                                                                                                                                                            | 8.317   | GPL-2.0-or-later   |
| HepMC                                                                                                                                                                                             | 3.3.1   | LGPL-3.0-or-later  |
| LHAPDF                                                                                                                                                                                            | 6.5.6   | GPL-3.0            |
| FastJet                                                                                                                                                                                           | 3.5.3   | GPL-2.0-or-later   |
| fjcontrib                                                                                                                                                                                         | 1.105   | GPL-2.0-or-later   |
| eMELA                                                                                                                                                                                             | 1.0.1   | MIT                |
| ROOT                                                                                                                                                                                              | 6.40.04 | LGPL-2.1-or-later  |
| Delphes<br>+ `no-display.patch`                                                                                                                                                                   | 3.5.1   | GPL-3.0-or-later   |
| `main164delphes`: Pythia's `main164`<br>+ `main164delphes.patch`<br>+ `DelphesPythia8Event.h`                                                                                                     | 8.317   | GPL-3.0-or-later   |
