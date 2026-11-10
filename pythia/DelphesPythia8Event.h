// DelphesPythia8Event.h: run Delphes on Pythia 8 events in memory.
//
// Pythia's main164 normally writes each event as HepMC3 text, which
// DelphesHepMC3 then reads back. Building the HepMC3 event, formatting it and
// parsing it again take about a third of the CPU time of the shower and the
// detector simulation together. DelphesPythia8Reader fills the Delphes arrays
// straight from the Pythia event record instead. It gives the same Delphes
// output, value for value, because it follows the whole chain on plain
// indices:
//   - HepMC3::Pythia8ToHepMC3::fill_next_event (Pythia 8.317,
//     Pythia8Plugins/HepMC3.h) builds the vertices from the mother lists, and
//     HepMC3::GenEvent::add_tree (HepMC3 3.3.1) orders the particles;
//   - HepMC3::WriterAscii::write_event (HepMC3 3.3.1) decides which vertices
//     it writes and what parent each particle line names. The text keeps every
//     double (17 significant digits), except the cross section (9);
//   - DelphesHepMC3Reader (Delphes 3.5.1, classes/DelphesHepMC3Reader.cc)
//     turns those lines into Delphes candidates. Its vertex and particle code
//     is copied here unchanged, apart from hash maps in place of std::map.
// main164 stores no PDF or process information in HepMC (set_store_pdf and
// set_store_proc are off), so DelphesHepMC3 leaves Event.ProcessID, Event.MPI,
// Event.Scale, Event.AlphaQCD, Event.AlphaQED and the PDF fields (ID1, ID2,
// X1, X2, ScalePDF, PDF1, PDF2) uninitialized. Here MPI is 1 and the rest 0.
//
// Delphes is licensed under the GNU GPL version 3 or later.

#ifndef DelphesPythia8Event_h
#define DelphesPythia8Event_h

#include <algorithm>
#include <cstring>
#include <deque>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "Pythia8/Pythia.h"

#include "TDatabasePDG.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TObjArray.h"
#include "TParticlePDG.h"
#include "TROOT.h"
#include "TStopwatch.h"

#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "classes/DelphesStream.h"
#include "modules/Delphes.h"

#include "ExRootAnalysis/ExRootConfReader.h"
#include "ExRootAnalysis/ExRootTreeBranch.h"
#include "ExRootAnalysis/ExRootTreeWriter.h"

class DelphesPythia8Reader
{
public:
  DelphesPythia8Reader() :
    fEventNumber(0), fVertexCounter(-2), fParticleCounter(-1),
    fMPI(1), fProcessID(0),
    fScale(0.), fAlphaQCD(0.), fAlphaQED(0.),
    fCrossSection(0.), fCrossSectionError(0.),
    fID1(0), fID2(0), fX1(0.), fX2(0.), fScalePDF(0.), fPDF1(0.), fPDF2(0.),
    fInternalEventNumber(0)
  {
    fPDG = TDatabasePDG::Instance();
  }

  void Clear()
  {
    fWeights.clear();
    fMomentumCoefficient = 1.0;
    fPositionCoefficient = 1.0;
    fVertexCounter = -2;
    fParticleCounter = -1;
    fVertices.clear();
    fParticles.clear();
    fInVertexMap.clear();
    fOutVertexMap.clear();
    fMotherMap.clear();
    fDaughterMap.clear();
  }

  bool EventReady()
  {
    return (fVertexCounter == -1) && (fParticleCounter == 0);
  }

  // Fill the Delphes arrays from the current Pythia event. Returns false when
  // DelphesHepMC3 would not have completed the event either.
  bool ReadEvent(Pythia8::Pythia &pythia, DelphesFactory *factory,
    TObjArray *allParticleOutputArray,
    TObjArray *stableParticleOutputArray,
    TObjArray *partonOutputArray)
  {
    const Pythia8::Event &pyev = pythia.event;
    BuildRecord(pyev);

    // E line
    Clear();
    fEventNumber = fInternalEventNumber++;
    // GenEvent::vertices().size(): the vertices in the event, without the
    // root vertex, which add_tree takes out again
    fVertexCounter = 0;
    for(int v = 1; v < fNVertices; ++v)
    {
      if(fVertexStore[v].inEvent) ++fVertexCounter;
    }
    fParticleCounter = fOrder.size();

    // U line: Pythia8ToHepMC3 sets GeV and mm, so both coefficients stay 1.

    // W line: printed with 23 significant digits, which keeps every double
    for(int i = 0; i < pythia.info.numberOfWeights(); ++i)
    {
      fWeights.push_back(pythia.info.weightValueByIndex(i));
    }

    // A line of the GenCrossSection attribute, which is printed with
    // GenCrossSection::to_string, through the text parser itself
    {
      double xsec = pythia.info.sigmaGen() * 1e9;
      double xsecErr = pythia.info.sigmaErr() * 1e9;
      std::vector<double> xsecVec = pythia.info.weightContainerPtr->getTotalXsec();
      if(!xsecVec.empty()) xsec = xsecVec[0] * 1e9;
      std::ostringstream os;
      os << "A 0 GenCrossSection " << std::setprecision(8) << std::scientific
         << xsec << " " << xsecErr << "\n";
      std::string line = os.str();
      std::vector<char> buffer(line.begin(), line.end());
      buffer.push_back('\0');
      ReadAttribute(buffer.data());
    }

    // V and P lines, in the order of HepMC3::WriterAscii::write_event
    for(int v = 0; v < fNVertices; ++v) fVertexStore[v].written = false;
    for(int p : fOrder)
    {
      int v = fProd[p];
      int parentObject = 0;
      if(v >= 0)
      {
        Vertex &vertex = fVertexStore[v];
        if(vertex.in.size() > 1 || !vertex.zero)
        {
          parentObject = VertexCode(v);
        }
        else if(vertex.in.size() == 1)
        {
          parentObject = fId[vertex.in.front()];
        }
        if(!vertex.written && parentObject < 0)
        {
          ReadVertex(factory, v);
          vertex.written = true;
        }
      }
      ReadParticle(factory, pyev[p], fId[p], parentObject);
    }

    if(!EventReady()) return false;
    FinalizeParticles(allParticleOutputArray, stableParticleOutputArray, partonOutputArray);
    return true;
  }

  void AnalyzeEvent(ExRootTreeBranch *branch, long long /*eventNumber*/,
    TStopwatch *readStopWatch, TStopwatch *procStopWatch)
  {
    HepMCEvent *element;

    element = static_cast<HepMCEvent *>(branch->NewEntry());
    element->Number = fEventNumber;

    element->ProcessID = fProcessID;
    element->MPI = fMPI;
    element->Weight = fWeights.size() > 0 ? fWeights[0] : 1.0;
    element->CrossSection = fCrossSection;
    element->CrossSectionError = fCrossSectionError;
    element->Scale = fScale;
    element->AlphaQED = fAlphaQED;
    element->AlphaQCD = fAlphaQCD;

    element->ID1 = fID1;
    element->ID2 = fID2;
    element->X1 = fX1;
    element->X2 = fX2;
    element->ScalePDF = fScalePDF;
    element->PDF1 = fPDF1;
    element->PDF2 = fPDF2;

    element->ReadTime = readStopWatch->RealTime();
    element->ProcTime = procStopWatch->RealTime();
  }

  void AnalyzeWeight(ExRootTreeBranch *branch)
  {
    Weight *element;
    std::vector<double>::const_iterator itWeights;

    for(itWeights = fWeights.begin(); itWeights != fWeights.end(); ++itWeights)
    {
      element = static_cast<Weight *>(branch->NewEntry());

      element->Weight = *itWeights;
    }
  }

private:
  // A HepMC3 vertex of the event being built. Vertex 0 is the GenEvent's root
  // vertex, which holds the particles without a production vertex.
  struct Vertex
  {
    std::vector<int> in, out; // Pythia indices, in HepMC3's order
    double x, y, z, t;
    bool zero; // GenVertexData::is_zero(): the status is always 0
    bool inEvent, written;
    int queued; // times it is in add_tree's deque
  };

  int VertexCode(int v) const { return v == 0 ? 0 : -v; }

  int NewVertex()
  {
    if(fNVertices == int(fVertexStore.size())) fVertexStore.emplace_back();
    Vertex &vertex = fVertexStore[fNVertices];
    vertex.in.clear();
    vertex.out.clear();
    vertex.x = vertex.y = vertex.z = vertex.t = 0.0;
    vertex.zero = true;
    vertex.inEvent = false;
    vertex.written = false;
    vertex.queued = 0;
    return fNVertices++;
  }

  static void Erase(std::vector<int> &list, int p)
  {
    list.erase(std::remove(list.begin(), list.end(), p), list.end());
  }

  // GenVertex::add_particle_in
  void AddIn(int v, int p)
  {
    std::vector<int> &in = fVertexStore[v].in;
    if(std::find(in.begin(), in.end(), p) != in.end()) return;
    in.push_back(p);
    if(fEnd[p] >= 0) Erase(fVertexStore[fEnd[p]].in, p);
    fEnd[p] = v;
    if(fVertexStore[v].inEvent) AddParticle(p);
  }

  // GenVertex::add_particle_out
  void AddOut(int v, int p)
  {
    std::vector<int> &out = fVertexStore[v].out;
    if(std::find(out.begin(), out.end(), p) != out.end()) return;
    out.push_back(p);
    if(fProd[p] >= 0) Erase(fVertexStore[fProd[p]].out, p);
    fProd[p] = v;
    if(fVertexStore[v].inEvent) AddParticle(p);
  }

  // GenEvent::add_particle
  void AddParticle(int p)
  {
    if(fId[p]) return;
    fOrder.push_back(p);
    fId[p] = fOrder.size();
    if(fProd[p] < 0) AddOut(0, p);
  }

  // GenEvent::add_vertex
  void AddVertex(int v)
  {
    if(fVertexStore[v].inEvent) return;
    fVertexStore[v].inEvent = true;
    for(size_t i = 0; i < fVertexStore[v].in.size(); ++i)
    {
      AddParticle(fVertexStore[v].in[i]);
    }
    for(size_t i = 0; i < fVertexStore[v].out.size(); ++i)
    {
      AddParticle(fVertexStore[v].out[i]);
    }
  }

  void PushFront(int v) { fSorting.push_front(v); ++fVertexStore[v].queued; }
  void PushBack(int v) { fSorting.push_back(v); ++fVertexStore[v].queued; }

  // Pythia8ToHepMC3::fill_next_event, steps 3 and 4, with add_tree in between
  void BuildRecord(const Pythia8::Event &pyev)
  {
    const int n = pyev.size();
    fNVertices = 0;
    NewVertex(); // the root vertex
    fProd.assign(n, -1);
    fEnd.assign(n, -1);
    fId.assign(n, 0);
    fOrder.clear();
    fBeams.clear();

    // 3. Fill vertex information.
    for(int i = 1; i < n; ++i)
    {
      fMothers = pyev[i].motherList();
      std::sort(fMothers.begin(), fMothers.end());
      fMothers.erase(fMothers.begin(),
        std::find_if(fMothers.begin(), fMothers.end(), [](int m) { return m != 0; }));
      if(!fMothers.empty())
      {
        int v = fEnd[fMothers[0]];
        if(v < 0)
        {
          v = NewVertex();
          for(int m : fMothers) AddIn(v, m);
        }
        const Pythia8::Particle &particle = pyev[i];
        double x = particle.xProd(), y = particle.yProd(), z = particle.zProd(), t = particle.tProd();
        Vertex &vertex = fVertexStore[v];
        if(!(x == 0 && y == 0 && z == 0 && t == 0) && vertex.zero)
        {
          vertex.x = x;
          vertex.y = y;
          vertex.z = z;
          vertex.t = t;
          vertex.zero = false;
        }
        AddOut(v, i);
      }
      else
      {
        fBeams.push_back(i);
      }
    }

    // GenEvent::add_tree(beam_particles); its cycle check only adds an
    // attribute that DelphesHepMC3 ignores.
    fSorting.clear();
    for(int p : fBeams)
    {
      int v = fProd[p];
      if(v < 0 || fVertexStore[v].in.empty())
      {
        if(fEnd[p] >= 0) PushBack(fEnd[p]);
      }
    }
    while(!fSorting.empty())
    {
      int v = fSorting.front();
      bool added = false;
      for(size_t i = 0; i < fVertexStore[v].in.size(); ++i)
      {
        int v2 = fProd[fVertexStore[v].in[i]];
        if(v2 >= 0 && !fVertexStore[v2].inEvent && fVertexStore[v2].queued == 0)
        {
          PushFront(v2);
          added = true;
        }
      }
      if(added) continue;
      if(!fVertexStore[v].inEvent)
      {
        AddVertex(v);
        for(size_t i = 0; i < fVertexStore[v].out.size(); ++i)
        {
          int v2 = fEnd[fVertexStore[v].out[i]];
          if(v2 >= 0 && !fVertexStore[v2].inEvent && fVertexStore[v2].queued == 0)
          {
            PushBack(v2);
          }
        }
      }
      --fVertexStore[fSorting.front()].queued;
      fSorting.pop_front();
    }

    // 4. Attach the particles that come from nowhere to a vertex of their own.
    for(int i = 1; i < n; ++i)
    {
      if(!fId[i])
      {
        int v = NewVertex();
        AddOut(v, i);
        AddVertex(v);
      }
    }
  }

  // The A branch of DelphesHepMC3Reader::ReadBlock
  void ReadAttribute(char *line)
  {
    DelphesStream bufferStream(line + 1);
    int rc;

    if(bufferStream.FindStr("mpi"))
    {
      rc = bufferStream.ReadInt(fMPI);
      if(!rc) throw std::runtime_error("invalid MPI format");
    }
    else if(bufferStream.FindStr("signal_process_id"))
    {
      rc = bufferStream.ReadInt(fProcessID);
      if(!rc) throw std::runtime_error("invalid process ID format");
    }
    else if(bufferStream.FindStr("event_scale"))
    {
      rc = bufferStream.ReadDbl(fScale);
      if(!rc) throw std::runtime_error("invalid event scale format");
    }
    else if(bufferStream.FindStr("alphaQCD"))
    {
      rc = bufferStream.ReadDbl(fAlphaQCD);
      if(!rc) throw std::runtime_error("invalid alphaQCD format");
    }
    else if(bufferStream.FindStr("alphaQED"))
    {
      rc = bufferStream.ReadDbl(fAlphaQED);
      if(!rc) throw std::runtime_error("invalid alphaQED format");
    }
    else if(bufferStream.FindStr("GenCrossSection"))
    {
      rc = bufferStream.ReadDbl(fCrossSection)
        && bufferStream.ReadDbl(fCrossSectionError);
      if(!rc) throw std::runtime_error("invalid cross section format");
    }
    else if(bufferStream.FindStr("GenPdfInfo"))
    {
      rc = bufferStream.ReadInt(fID1)
        && bufferStream.ReadInt(fID2)
        && bufferStream.ReadDbl(fX1)
        && bufferStream.ReadDbl(fX2)
        && bufferStream.ReadDbl(fScalePDF)
        && bufferStream.ReadDbl(fPDF1)
        && bufferStream.ReadDbl(fPDF2);
      if(!rc) throw std::runtime_error("invalid PDF format");
    }
  }

  // The V branch of DelphesHepMC3Reader::ReadBlock
  void ReadVertex(DelphesFactory *factory, int v)
  {
    const Vertex &vertex = fVertexStore[v];

    fParticles.clear();
    for(int p : vertex.in) fParticles.push_back(fId[p]);
    std::sort(fParticles.begin(), fParticles.end());

    fX = vertex.x;
    fY = vertex.y;
    fZ = vertex.z;
    fT = vertex.t;

    AnalyzeVertex(factory, VertexCode(v));
  }

  // The P branch of DelphesHepMC3Reader::ReadBlock
  void ReadParticle(DelphesFactory *factory, const Pythia8::Particle &particle, int id, int parentObject)
  {
    if(fParticleCounter <= 0) return;
    --fParticleCounter;

    fParticleCode = id;
    fOutVertexCode = parentObject;
    fPID = particle.id();
    fPx = particle.px();
    fPy = particle.py();
    fPz = particle.pz();
    fE = particle.e();
    fMass = particle.m();
    fParticleStatus = particle.statusHepMC();

    AnalyzeParticle(factory);
  }

  void AnalyzeVertex(DelphesFactory *factory, int code, Candidate *candidate = 0)
  {
    int index;
    TLorentzVector *position;
    TObjArray *array;
    std::vector<int>::iterator itParticle;
    std::unordered_map<int, int>::iterator itVertexMap;

    itVertexMap = fOutVertexMap.find(code);
    if(itVertexMap == fOutVertexMap.end())
    {
      --fVertexCounter;

      index = fVertices.size();
      fOutVertexMap[code] = index;
      if(candidate && code > 0) fInVertexMap[code] = index;

      position = factory->New<TLorentzVector>();
      array = factory->NewArray();
      position->SetXYZT(0.0, 0.0, 0.0, 0.0);
      fVertices.push_back(std::make_pair(position, array));
    }
    else
    {
      index = itVertexMap->second;
      position = fVertices[index].first;
      array = fVertices[index].second;
    }

    if(candidate)
    {
      array->Add(candidate);
    }
    else
    {
      position->SetXYZT(fX, fY, fZ, fT);
      for(itParticle = fParticles.begin(); itParticle != fParticles.end(); ++itParticle)
      {
        fInVertexMap[*itParticle] = index;
      }
    }
  }

  void AnalyzeParticle(DelphesFactory *factory)
  {
    Candidate *candidate;

    candidate = factory->NewCandidate();

    candidate->PID = fPID;

    candidate->Status = fParticleStatus;

    candidate->Mass = fMass;

    candidate->Momentum.SetPxPyPzE(fPx, fPy, fPz, fE);

    candidate->D1 = fParticleCode;

    AnalyzeVertex(factory, fOutVertexCode, candidate);
  }

  void FinalizeParticles(TObjArray *allParticleOutputArray,
    TObjArray *stableParticleOutputArray,
    TObjArray *partonOutputArray)
  {
    TLorentzVector *position;
    TObjArray *array;
    Candidate *candidate;
    Candidate *candidateDaughter;
    TParticlePDG *pdgParticle;
    int pdgCode;
    std::unordered_map<int, int>::iterator itVertexMap;
    std::unordered_map<int, std::pair<int, int> >::iterator itMotherMap;
    std::unordered_map<int, std::pair<int, int> >::iterator itDaughterMap;
    size_t i;
    int j, code, counter;

    counter = 0;
    for(i = 0; i < fVertices.size(); ++i)
    {
      position = fVertices[i].first;
      array = fVertices[i].second;

      for(j = 0; j < array->GetEntriesFast(); ++j)
      {
        candidate = static_cast<Candidate *>(array->At(j));

        candidate->Position = *position;
        if(fPositionCoefficient != 1.0)
        {
          candidate->Position *= fPositionCoefficient;
        }

        if(fMomentumCoefficient != 1.0)
        {
          candidate->Momentum *= fMomentumCoefficient;
        }

        candidate->M1 = i;

        itDaughterMap = fDaughterMap.find(i);
        if(itDaughterMap == fDaughterMap.end())
        {
          fDaughterMap[i] = std::make_pair(counter, counter);
        }
        else
        {
          itDaughterMap->second.second = counter;
        }

        code = candidate->D1;

        itVertexMap = fInVertexMap.find(code);
        if(itVertexMap == fInVertexMap.end())
        {
          candidate->D1 = -1;
        }
        else
        {
          code = itVertexMap->second;

          candidate->D1 = code;

          itMotherMap = fMotherMap.find(code);
          if(itMotherMap == fMotherMap.end())
          {
            fMotherMap[code] = std::make_pair(counter, -1);
          }
          else
          {
            itMotherMap->second.second = counter;
          }
        }

        allParticleOutputArray->Add(candidate);

        ++counter;

        pdgParticle = fPDG->GetParticle(candidate->PID);

        candidate->Charge = pdgParticle ? int(pdgParticle->Charge() / 3.0) : -999;

        if(!pdgParticle) continue;

        pdgCode = TMath::Abs(candidate->PID);

        if(candidate->Status == 1)
        {
          stableParticleOutputArray->Add(candidate);
        }
        else if(pdgCode <= 5 || pdgCode == 21 || pdgCode == 15)
        {
          partonOutputArray->Add(candidate);
        }
      }
    }

    for(j = 0; j < allParticleOutputArray->GetEntriesFast(); ++j)
    {
      candidate = static_cast<Candidate *>(allParticleOutputArray->At(j));

      itMotherMap = fMotherMap.find(candidate->M1);
      if(itMotherMap == fMotherMap.end())
      {
        candidate->M1 = -1;
        candidate->M2 = -1;
      }
      else
      {
        candidate->M1 = itMotherMap->second.first;
        candidate->M2 = itMotherMap->second.second;
      }

      if(candidate->D1 < 0)
      {
        candidate->D1 = -1;
        candidate->D2 = -1;
      }
      else
      {
        itDaughterMap = fDaughterMap.find(candidate->D1);
        if(itDaughterMap == fDaughterMap.end())
        {
          candidate->D1 = -1;
          candidate->D2 = -1;
          const TLorentzVector &decayPosition = candidate->Position;
          candidate->DecayPosition.SetXYZT(decayPosition.X(), decayPosition.Y(), decayPosition.Z(), decayPosition.T());
        }
        else
        {
          candidate->D1 = itDaughterMap->second.first;
          candidate->D2 = itDaughterMap->second.second;
          candidateDaughter = static_cast<Candidate *>(allParticleOutputArray->At(candidate->D1));
          const TLorentzVector &decayPosition = candidateDaughter->Position;
          candidate->DecayPosition.SetXYZT(decayPosition.X(), decayPosition.Y(), decayPosition.Z(), decayPosition.T());
        }
      }
    }
  }

  TDatabasePDG *fPDG;

  long long fEventNumber;
  int fVertexCounter, fParticleCounter;
  int fMPI, fProcessID;
  double fScale, fAlphaQCD, fAlphaQED;

  double fMomentumCoefficient, fPositionCoefficient;

  std::vector<double> fWeights;

  double fCrossSection, fCrossSectionError;

  int fID1, fID2;
  double fX1, fX2, fScalePDF, fPDF1, fPDF2;

  double fX, fY, fZ, fT;

  int fParticleCode, fPID, fParticleStatus, fOutVertexCode;
  double fPx, fPy, fPz, fE, fMass;

  std::vector<std::pair<TLorentzVector *, TObjArray *> > fVertices;
  std::vector<int> fParticles;

  std::unordered_map<int, int> fInVertexMap;
  std::unordered_map<int, int> fOutVertexMap;

  std::unordered_map<int, std::pair<int, int> > fMotherMap;
  std::unordered_map<int, std::pair<int, int> > fDaughterMap;

  // The HepMC3 record of the current event
  long long fInternalEventNumber;
  std::vector<Vertex> fVertexStore;
  int fNVertices;
  std::vector<int> fProd, fEnd; // vertex of each Pythia index, or -1
  std::vector<int> fId; // HepMC3 particle id of each Pythia index, or 0
  std::vector<int> fOrder; // Pythia indices in HepMC3 particle order
  std::vector<int> fBeams, fMothers;
  std::deque<int> fSorting;
};

// The event loop of DelphesHepMC3 (Delphes 3.5.1, readers/DelphesHepMC3.cpp),
// fed one Pythia event at a time.
class DelphesPythia8Output
{
public:
  DelphesPythia8Output(const std::string &card, const std::string &output) :
    fEventCounter(0)
  {
    gROOT->SetBatch();

    fOutputFile = TFile::Open(output.c_str(), "CREATE");
    if(fOutputFile == NULL)
    {
      throw std::runtime_error("can't create output file " + output);
    }

    fTreeWriter = new ExRootTreeWriter(fOutputFile, "Delphes");

    fBranchEvent = fTreeWriter->NewBranch("Event", HepMCEvent::Class());
    fBranchWeight = fTreeWriter->NewBranch("Weight", Weight::Class());

    fConfReader = new ExRootConfReader;
    fConfReader->ReadFile(card.c_str());

    fMaxEvents = fConfReader->GetInt("::MaxEvents", 0);
    fSkipEvents = fConfReader->GetInt("::SkipEvents", 0);

    if(fMaxEvents < 0)
    {
      throw std::runtime_error("MaxEvents must be zero or positive");
    }

    if(fSkipEvents < 0)
    {
      throw std::runtime_error("SkipEvents must be zero or positive");
    }

    fModularDelphes = new Delphes("Delphes");
    fModularDelphes->SetConfReader(fConfReader);
    fModularDelphes->SetTreeWriter(fTreeWriter);

    fFactory = fModularDelphes->GetFactory();
    fAllParticleOutputArray = fModularDelphes->ExportArray("allParticles");
    fStableParticleOutputArray = fModularDelphes->ExportArray("stableParticles");
    fPartonOutputArray = fModularDelphes->ExportArray("partons");

    fModularDelphes->InitTask();

    fTreeWriter->Clear();
    fModularDelphes->Clear();
    fReader.Clear();
    fReadStopWatch.Start();
  }

  ~DelphesPythia8Output()
  {
    delete fModularDelphes;
    delete fConfReader;
    delete fTreeWriter;
    delete fOutputFile;
  }

  void Process(Pythia8::Pythia &pythia)
  {
    if(fMaxEvents > 0 && fEventCounter - fSkipEvents >= fMaxEvents) return;

    if(fReader.ReadEvent(pythia, fFactory, fAllParticleOutputArray,
         fStableParticleOutputArray, fPartonOutputArray))
    {
      ++fEventCounter;

      fReadStopWatch.Stop();

      if(fEventCounter > fSkipEvents)
      {
        fProcStopWatch.Start();
        fModularDelphes->ProcessTask();
        fProcStopWatch.Stop();

        fReader.AnalyzeEvent(fBranchEvent, fEventCounter, &fReadStopWatch, &fProcStopWatch);
        fReader.AnalyzeWeight(fBranchWeight);

        fTreeWriter->Fill();

        fTreeWriter->Clear();
      }

      fReadStopWatch.Start();
    }

    fModularDelphes->Clear();
    fReader.Clear();
  }

  void Finish()
  {
    fModularDelphes->FinishTask();
    fTreeWriter->Write();
  }

private:
  TFile *fOutputFile;
  ExRootTreeWriter *fTreeWriter;
  ExRootTreeBranch *fBranchEvent, *fBranchWeight;
  ExRootConfReader *fConfReader;
  Delphes *fModularDelphes;
  DelphesFactory *fFactory;
  TObjArray *fStableParticleOutputArray, *fAllParticleOutputArray, *fPartonOutputArray;
  DelphesPythia8Reader fReader;
  TStopwatch fReadStopWatch, fProcStopWatch;
  int fMaxEvents, fSkipEvents;
  long long fEventCounter;
};

#endif // DelphesPythia8Event_h
