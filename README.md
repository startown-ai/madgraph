# MadGraph

Docker images for MadGraph5_aMC@NLO and the high-energy physics (HEP) tools around it, each built from source and tested with the package's own tests.

MadGraph5_aMC@NLO simulates particle collisions, but a working setup needs Pythia, HepMC, LHAPDF, FastJet, ROOT, Delphes and more, built to fit together. This repository builds every one of them in its own image, so each tool can also be used on its own, and the final `madgraph` image copies them all in.

## Images

| Image | Contains | Use it for |
|---|---|---|
| `startown/hepmc2:2.06.11` | HepMC 2, patched for named weights | reading and writing `.hepmc` event records |
| `startown/lhapdf:6.5.6` | LHAPDF with its Python module | evaluating PDFs |
| `startown/fastjet:3.5.3` | FastJet with fjcontrib 1.105 | jet finding |
| `startown/emela:1.0.1` | eMELA, LHAPDF | QED evolution of lepton PDFs |
| `startown/pythia8:8317` | Pythia 8.317, HepMC 2, LHAPDF | parton showers and hadronization |
| `startown/root:6.40.04` | ROOT (headless) with PyROOT | data analysis, reading `.root` files |
| `startown/delphes:3.5.1` | Delphes, ROOT | fast detector simulation |
| `startown/madgraph:3.5.17` | MadGraph5_aMC@NLO LTS with all of the above | the full chain from process to detector |

Every image installs its package under `/opt/<package>`, registers its libraries, puts its programs on `PATH` (HepMC 2 has none), and keeps `g++` and `make`, so you can compile code against it inside the container.

## Dependencies

```
 hepmc2 2.06.11 ───▶ pythia8 8.317 ──┐
                       ▲             │
 lhapdf 6.5.6 ─────────┤             │
                       ▼             │
                     emela 1.0.1 ────┤
                                     ├──▶ madgraph 3.5.17
 fastjet 3.5.3 ──────────────────────┤
                                     │
 root 6.40.04 ─────▶ delphes 3.5.1 ──┘
```

The `madgraph` image also builds MadGraph's Pythia8 interface (MG5aMC_PY8_interface 1.3).

- [MadGraph5_aMC@NLO](https://launchpad.net/mg5amcnlo): generates parton-level events (`.lhe`) from a process definition. This is the 3.5.x long-term-support line.
- [Pythia](https://www.pythia.org/): parton showers, hadronization and decays.
- [HepMC](https://hepmc.web.cern.ch/hepmc/): the event record Pythia writes for MadGraph (`.hepmc`).
- [LHAPDF](https://lhapdf.hepforge.org/): parton distribution functions (PDFs). The `madgraph` image ships `NNPDF23_lo_as_0130_qed`, which MadGraph's systematics use by default.
- [eMELA](https://github.com/gstagnit/eMELA): QED evolution of lepton PDFs at next-to-leading-log accuracy.
- [FastJet](https://fastjet.fr/) and [fjcontrib](https://fastjet.hepforge.org/contrib/): jet algorithms and tools.
- [ROOT](https://root.cern/): the HEP data analysis framework.
- [Delphes](https://delphes.github.io/): fast detector simulation, writing reconstructed events (`.root`).

## Run

Start MadGraph with your working directory mounted at `/work`:

```bash
docker run -it --rm -v "$PWD":/work -w /work startown/madgraph:3.5.17 mg5_aMC
```

Run a MadGraph script without the prompt:

```bash
docker run --rm -v "$PWD":/work -w /work startown/madgraph:3.5.17 mg5_aMC commands.mg5
```

For example, `commands.mg5` with

```
generate p p > t t~
output ttbar
launch ttbar
shower=Pythia8
detector=Delphes
set nevents 1000
set iseed 42
```

writes the parton-level events, the showered `.hepmc` file and the Delphes `.root` file to `ttbar/Events/run_01/`.

The smaller images work the same way. For example, open a Delphes output file in ROOT; the Delphes classes load automatically:

```bash
docker run -it --rm -v "$PWD":/work -w /work startown/delphes:3.5.1 root -l tag_1_delphes_events.root
```

## Changes from upstream

- **HepMC 2:** a small patch adds `WeightContainer::push_back(value, name)`, the same addition MadGraph's own installer makes. With it, Pythia writes every weight (scales, PDFs, merging) as a named weight in one `.hepmc` file, instead of one file per weight.
- **ROOT without graphics:** X11 and OpenGL are off, so ROOT runs in batch mode and saves plots as PDF, SVG or EPS, but not PNG. XRootD, Davix, curl, FITS, SQLite and TMVA's CPU deep learning are off too; FFTW is on.
- **Delphes with CMake:** `delphes/no-display.patch` drops the event display, which needs ROOT's graphics. The build also adds two things missing from Delphes's install:
  - an `include/external` link, so the installed example macros compile;
  - a `libDelphes.rootmap`, so ROOT loads the Delphes classes by itself.
- **MadGraph:**
  - the Pythia8 interface is compiled with `-DHEPMC2HACK -DHEPMC2` against the patched HepMC;
  - `gzip` is `pigz`, and MadGraph always uses the external `gzip`, so large event files compress on all cores;
  - `mg5_configuration.txt` points at Pythia8, LHAPDF, FastJet, eMELA and Delphes, with automatic updates and browser windows turned off.

## License

This repository is licensed under the [Apache License 2.0](LICENSE), except the two patches, which keep the license of the code they change:

- `hepmc2/named-weights.patch` changes HepMC 2 (GNU GPL) and adds MadGraph5_aMC@NLO's HepMC 2 hack, under the MadGraph5_aMC@NLO license reproduced in the patch.
- `delphes/no-display.patch` changes Delphes (GPL-3.0-or-later).

The images contain software under its own licenses, and each image includes the license files of what it contains:

| Software | License | License files in the image |
|---|---|---|
| MadGraph5_aMC@NLO, MG5aMC_PY8_interface | MadGraph5_aMC@NLO license, adapted from the University of Illinois/NCSA license | `/opt/madgraph/LICENSE` |
| Pythia 8 | GPL-2.0-or-later | `/opt/pythia8/share/Pythia8/COPYING` |
| HepMC 2 | GNU GPL, no version stated | `/opt/hepmc2/share/licenses/hepmc2/` |
| LHAPDF | GPL-3.0 | `/opt/lhapdf/share/licenses/lhapdf/` |
| FastJet | GPL-2.0-or-later | `/opt/fastjet/share/licenses/fastjet/` |
| fjcontrib | GPL-2.0, some packages GPL-3.0 | `/opt/fastjet/share/licenses/fjcontrib/` |
| eMELA | MIT | `/opt/emela/share/licenses/emela/` |
| ROOT | LGPL-2.1-or-later | `/opt/root/LICENSE` |
| Delphes, with the parts it bundles | GPL-3.0-or-later; bundled parts listed in its `LICENSE` | `/opt/delphes/share/licenses/delphes/` |

The `NNPDF23_lo_as_0130_qed` PDF set comes from LHAPDF's PDF set repository and has no license file. The Debian packages in the base image keep their copyright files in `/usr/share/doc`.

Every package is built from its official release, mirrored at `https://assets.startown.ai/packages/`, with the patches in this repository. The `Dockerfile`s show exactly how each image is built.
