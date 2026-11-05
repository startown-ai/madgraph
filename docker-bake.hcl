group "default" {
  targets = ["madgraph"]
}

target "_common" {
  platforms = ["linux/amd64", "linux/arm64"]
}

target "expat" {
  inherits = ["_common"]
  context  = "expat"
}

target "hepmc" {
  inherits = ["_common"]
  context  = "hepmc"
}

target "lhapdf" {
  inherits = ["_common"]
  context  = "lhapdf"
}

target "fastjet" {
  inherits = ["_common"]
  context  = "fastjet"
}

target "root" {
  inherits = ["_common"]
  context  = "root"
}

target "pythia" {
  inherits = ["_common"]
  context  = "pythia"
  contexts = {
    hepmc  = "target:hepmc"
    lhapdf = "target:lhapdf"
  }
}

target "emela" {
  inherits = ["_common"]
  context  = "emela"
  contexts = {
    lhapdf = "target:lhapdf"
  }
}

target "delphes" {
  inherits = ["_common"]
  context  = "delphes"
  contexts = {
    root = "target:root"
  }
}

target "madgraph" {
  inherits = ["_common"]
  context  = "madgraph"
  contexts = {
    delphes = "target:delphes"
    emela   = "target:emela"
    fastjet = "target:fastjet"
    hepmc   = "target:hepmc"
    lhapdf  = "target:lhapdf"
    pythia  = "target:pythia"
    root    = "target:root"
  }
  tags   = ["startown/madgraph:3.8.1"]
  # mode=max embeds the package builds, which Docker Scout fails to parse
  attest = ["type=provenance,mode=min", "type=sbom"]
}
