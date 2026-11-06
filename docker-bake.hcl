group "default" {
  targets = ["madgraph"]
}

target "fastjet" {
  platforms  = ["linux/amd64", "linux/arm64"]
  context    = "fastjet"
  cache-from = ["type=registry,ref=startown/madgraph-cache:fastjet"]
  cache-to   = ["type=registry,ref=startown/madgraph-cache:fastjet,mode=max"]
}

target "hepmc" {
  platforms  = ["linux/amd64", "linux/arm64"]
  context    = "hepmc"
  cache-from = ["type=registry,ref=startown/madgraph-cache:hepmc"]
  cache-to   = ["type=registry,ref=startown/madgraph-cache:hepmc,mode=max"]
}

target "lhapdf" {
  platforms  = ["linux/amd64", "linux/arm64"]
  context    = "lhapdf"
  cache-from = ["type=registry,ref=startown/madgraph-cache:lhapdf"]
  cache-to   = ["type=registry,ref=startown/madgraph-cache:lhapdf,mode=max"]
}

target "root" {
  platforms  = ["linux/amd64", "linux/arm64"]
  context    = "root"
  cache-from = ["type=registry,ref=startown/madgraph-cache:root"]
  cache-to   = ["type=registry,ref=startown/madgraph-cache:root,mode=max"]
}

target "delphes" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "delphes"
  contexts = {
    root = "target:root"
  }
  cache-from = ["type=registry,ref=startown/madgraph-cache:delphes"]
  cache-to   = ["type=registry,ref=startown/madgraph-cache:delphes,mode=max"]
}

target "emela" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "emela"
  contexts = {
    lhapdf = "target:lhapdf"
  }
  cache-from = ["type=registry,ref=startown/madgraph-cache:emela"]
  cache-to   = ["type=registry,ref=startown/madgraph-cache:emela,mode=max"]
}

target "pythia" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "pythia"
  contexts = {
    hepmc  = "target:hepmc"
    lhapdf = "target:lhapdf"
  }
  cache-from = ["type=registry,ref=startown/madgraph-cache:pythia"]
  cache-to   = ["type=registry,ref=startown/madgraph-cache:pythia,mode=max"]
}

target "madgraph" {
  platforms = ["linux/amd64", "linux/arm64"]
  context   = "madgraph"
  contexts = {
    delphes = "target:delphes"
    emela   = "target:emela"
    fastjet = "target:fastjet"
    hepmc   = "target:hepmc"
    lhapdf  = "target:lhapdf"
    pythia  = "target:pythia"
    root    = "target:root"
  }
  cache-from = ["type=registry,ref=startown/madgraph-cache:madgraph"]
  cache-to   = ["type=registry,ref=startown/madgraph-cache:madgraph,mode=max"]
  tags       = ["startown/madgraph:3.8.1-2"]
  attest     = ["type=provenance,mode=min", "type=sbom"]
}
