group "default" {
  targets = ["madgraph"]
}

target "binutils" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "binutils"
}

target "fastjet" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "fastjet"
}

target "hepmc" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "hepmc"
}

target "lhapdf" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "lhapdf"
}

target "root" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "root"
}

target "delphes" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "delphes"
  contexts = {
    root = "target:root"
  }
}

target "emela" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "emela"
  contexts = {
    lhapdf = "target:lhapdf"
  }
}

target "pythia" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "pythia"
  contexts = {
    hepmc  = "target:hepmc"
    lhapdf = "target:lhapdf"
  }
}

target "madgraph" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "madgraph"
  contexts = {
    binutils = "target:binutils"
    delphes  = "target:delphes"
    emela    = "target:emela"
    fastjet  = "target:fastjet"
    hepmc    = "target:hepmc"
    lhapdf   = "target:lhapdf"
    pythia   = "target:pythia"
    root     = "target:root"
  }
  tags   = ["startown/madgraph:3.8.1-5"]
  attest = ["type=provenance,mode=min", "type=sbom"]
}
