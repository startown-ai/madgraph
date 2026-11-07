# MadGraph

![Docker Hub](https://img.shields.io/docker/v/startown/madgraph?sort=date&label=docker%20hub&logo=docker&logoColor=white&style=flat-square) ![Image size](https://img.shields.io/docker/image-size/startown/madgraph/3.8.1-5?label=size&style=flat-square) ![License](https://img.shields.io/github/license/startown-ai/madgraph?style=flat-square)

MadGraph5_aMC@NLO with Pythia and Delphes.

## Installation

Pull the image:

```bash
docker pull startown/madgraph:3.8.1-5
```

## Usage

Start MadGraph interactively:

```bash
docker run -it --rm -v "$PWD":/data -w /data startown/madgraph:3.8.1-5 mg5_aMC
```

Or run a script:

```bash
docker run --rm -v "$PWD":/data -w /data startown/madgraph:3.8.1-5 mg5_aMC commands.mg5
```

For example, this `commands.mg5` generates 1,000 top-quark pairs with parton shower and detector simulation. The fixed core count and random seed make it reproducible:

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

This image defaults to `HEPMCoutput:file autoremove`, unlike MadGraph's `hepmc.gz`. In MadGraph, this setting deletes the HepMC file at the end of the run; with our patches, it runs Delphes inside each Pythia job instead, so no HepMC is written.

## License

This repository is licensed under the [Apache License 2.0](LICENSE), while the packages it builds keep their own licenses:

| Package                                                                                                                                                                                           | Version | License            |
| ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------- | ------------------ |
| binutils                                                                                                                                                                                          | 2.47    | GPL-3.0-or-later   |
| Python<br>+ `getaddrinfo-null-bytes.patch`<br>+ `http-client-framing.patch`                                                                                                                       | 3.13.18 | Python-2.0         |
| FastJet                                                                                                                                                                                           | 3.5.3   | GPL-2.0-or-later   |
| fjcontrib                                                                                                                                                                                         | 1.105   | GPL-2.0-or-later   |
| HepMC                                                                                                                                                                                             | 3.3.1   | LGPL-3.0-or-later  |
| LHAPDF                                                                                                                                                                                            | 6.5.6   | GPL-3.0            |
| ROOT<br>+ `eg-no-graphics.patch`                                                                                                                                                                  | 6.36.14 | LGPL-2.1-or-later  |
| Delphes<br>+ `no-display.patch`<br>+ `no-graphics.patch`                                                                                                                                          | 3.5.1   | GPL-3.0-or-later   |
| eMELA                                                                                                                                                                                             | 1.0.1   | MIT                |
| Pythia                                                                                                                                                                                            | 8.317   | GPL-2.0-or-later   |
| `main164delphes`: Pythia's `main164`<br>+ `main164delphes.patch`<br>+ `DelphesPythia8Event.h`                                                                                                     | 8.317   | GPL-3.0-or-later   |
| MadGraph5_aMC@NLO<br>+ `quiet-warnings.patch`<br>+ `seeds.patch`<br>+ `cores.patch`<br>+ `delphes-stream.patch`<br>+ `fast-lhe-split.patch`<br>+ `hadd-no-j.patch`<br>+ `delphes-inprocess.patch` | 3.8.1   | NCSA-style license |
