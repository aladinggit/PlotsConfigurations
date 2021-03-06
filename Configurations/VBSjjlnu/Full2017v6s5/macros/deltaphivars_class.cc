#include "LatinoAnalysis/MultiDraw/interface/TTreeFunction.h"
#include "LatinoAnalysis/MultiDraw/interface/FunctionLibrary.h"

#include "TMath.h"
#include "TGraph.h"
#include "TVector2.h"
#include "TSystem.h"
#include "TLorentzVector.h"

#include <cmath>
#include <string>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <tuple>

class DeltaPhiVars : public multidraw::TTreeFunction {
public:
  DeltaPhiVars(char const* type);
  DeltaPhiVars(unsigned type);

  char const* getName() const override { return "DeltaPhiVars"; }
  TTreeFunction* clone() const override { return new DeltaPhiVars(returnVar_); }

  unsigned getNdata() override { return 1; }
  double evaluate(unsigned) override;

protected:
  enum ReturnType {
                   deltaphi_lep_whad,
                   deltaphi_lep_vbsjets,
                   deltaphi_lep_jet0,
                   deltaphi_lep_ww,
                   deltaphi_lep_alljets,
                   deltaeta_lep_alljets,
                   deltaR_lep_alljets,
                   nVarTypes
  };
  
 
  void bindTree_(multidraw::FunctionLibrary&) override;

  unsigned returnVar_{nVarTypes};
 
  UIntValueReader* run{};
  UIntValueReader* luminosityBlock{};
  ULong64ValueReader* event{}; 

  static std::tuple<UInt_t, UInt_t, ULong64_t> currentEvent;
  static UIntValueReader* nJets; 
  static FloatArrayReader* Jet_pt;
  static FloatArrayReader* Jet_eta;
  static FloatArrayReader* Jet_phi;
  static IntArrayReader* Jet_idx;
  static FloatArrayReader* Jet_mass;
  static FloatArrayReader* Lepton_pt;
  static FloatArrayReader* Lepton_eta;
  static FloatArrayReader* Lepton_phi;
  static IntArrayReader* vbs_jets;
  static IntArrayReader* v_jets;

  static std::array<double, nVarTypes> returnValues;

  static void setValues(UInt_t, UInt_t, ULong64_t);
};

std::tuple<UInt_t, UInt_t, ULong64_t> DeltaPhiVars::currentEvent{};
UIntValueReader* DeltaPhiVars::nJets; 
FloatArrayReader* DeltaPhiVars::Jet_pt{};
FloatArrayReader* DeltaPhiVars::Jet_eta{};
FloatArrayReader* DeltaPhiVars::Jet_phi{};
IntArrayReader* DeltaPhiVars::Jet_idx{};
FloatArrayReader* DeltaPhiVars::Jet_mass{};
FloatArrayReader* DeltaPhiVars::Lepton_pt{};
FloatArrayReader* DeltaPhiVars::Lepton_eta{};
FloatArrayReader* DeltaPhiVars::Lepton_phi{};
IntArrayReader* DeltaPhiVars::vbs_jets{};
IntArrayReader* DeltaPhiVars::v_jets{};

std::array<double, DeltaPhiVars::nVarTypes> DeltaPhiVars::returnValues{};

DeltaPhiVars::DeltaPhiVars(char const* _type) :
  TTreeFunction()
{
  std::string type(_type);
  if (type == "deltaphi_lep_whad")
    returnVar_ = deltaphi_lep_whad;
  else if (type == "deltaphi_lep_vbsjets")
    returnVar_ = deltaphi_lep_vbsjets;
  else if (type == "deltaphi_lep_jet0")
    returnVar_ = deltaphi_lep_jet0;
  else if (type == "deltaphi_lep_ww")
    returnVar_ = deltaphi_lep_ww;
  else if (type == "deltaphi_lep_alljets")
    returnVar_ = deltaphi_lep_alljets;
  else if (type == "deltaeta_lep_alljets")
    returnVar_ = deltaeta_lep_alljets;
  else if (type == "deltaR_lep_alljets")
    returnVar_ = deltaR_lep_alljets;
  else
    throw std::runtime_error("unknown return type " + type);
  
}


DeltaPhiVars::DeltaPhiVars(unsigned type) :
  TTreeFunction(),
  returnVar_(type) {}


double
DeltaPhiVars::evaluate(unsigned)
{
  setValues(*run->Get(), *luminosityBlock->Get(), *event->Get());
  return returnValues[returnVar_];
}

void
DeltaPhiVars::bindTree_(multidraw::FunctionLibrary& _library)
{   
    _library.bindBranch(run, "run");
    _library.bindBranch(luminosityBlock, "luminosityBlock");
    _library.bindBranch(event, "event");

    _library.bindBranch(nJets, "nCleanJet");
    _library.bindBranch(Jet_pt, "CleanJet_pt");
    _library.bindBranch(Jet_eta, "CleanJet_eta");
    _library.bindBranch(Jet_phi, "CleanJet_phi");
    _library.bindBranch(Jet_mass, "Jet_mass");
    _library.bindBranch(Jet_idx, "CleanJet_jetIdx");
    _library.bindBranch(Lepton_pt, "Lepton_pt");
    _library.bindBranch(Lepton_eta, "Lepton_eta");
    _library.bindBranch(Lepton_phi, "Lepton_phi");
    _library.bindBranch(vbs_jets, "VBS_jets_maxmjj_massWZ");
    _library.bindBranch(v_jets, "V_jets_maxmjj_massWZ");

    currentEvent = std::make_tuple(0, 0, 0);

    _library.addDestructorCallback([]() {
                                     nJets = nullptr;
                                     Jet_pt = nullptr;
                                     Jet_eta = nullptr;
                                     Jet_phi = nullptr;
                                     Jet_mass = nullptr;
                                     Jet_idx = nullptr;
                                     Lepton_pt = nullptr;
                                     Lepton_eta = nullptr;
                                     Lepton_phi = nullptr;
                                     vbs_jets = nullptr;
                                     v_jets = nullptr; 
                                   });
}

/*static*/
void
DeltaPhiVars::setValues(UInt_t _run, UInt_t _luminosityBlock, ULong64_t _event)
{

  if (std::get<0>(currentEvent) == _run && \
      std::get<1>(currentEvent) == _luminosityBlock && \
      std::get<2>(currentEvent) == _event)
    return;

  currentEvent = std::make_tuple(_run, _luminosityBlock, _event);

  TLorentzVector vbsjets;
  for (auto ij : *vbs_jets){
    TLorentzVector v;
    float pt = Jet_pt->At(ij);
    float eta = Jet_eta->At(ij);
    float phi = Jet_phi->At(ij);
    float mass = Jet_mass->At(Jet_idx->At(ij));
    v.SetPtEtaPhiM(pt,eta,phi, mass);
    vbsjets += v;
  }


  TLorentzVector whad;
  for (auto ij : *v_jets){
    TLorentzVector v;
    float pt = Jet_pt->At(ij);
    float eta = Jet_eta->At(ij);
    float phi = Jet_phi->At(ij);
    float mass = Jet_mass->At(Jet_idx->At(ij));
    v.SetPtEtaPhiM(pt,eta,phi, mass);
    whad += v;
  }

  TLorentzVector total_jet;
  cout << "njets " << *(nJets->Get()) <<endl;
  for (int ij=0; ij < *(nJets->Get()); ij++){
    TLorentzVector v;
    float pt = Jet_pt->At(ij);
    float eta = Jet_eta->At(ij);
    float phi = Jet_phi->At(ij);
    float mass = Jet_mass->At(Jet_idx->At(ij));
    v.SetPtEtaPhiM(pt,eta,phi, mass);
    total_jet += v;
  }
  cout << total_jet.Pt() << endl;

  TLorentzVector lep; 
  lep.SetPtEtaPhiM(Lepton_pt->At(0), Lepton_eta->At(0), Lepton_phi->At(0), 0.);

  TLorentzVector jet0;
  jet0.SetPtEtaPhiM(Jet_pt->At(0), Jet_eta->At(0),Jet_phi->At(0),Jet_mass->At(Jet_idx->At(0)));   

  returnValues[deltaphi_lep_whad] = abs(lep.DeltaPhi(whad));
  returnValues[deltaphi_lep_vbsjets] = abs(lep.DeltaPhi(vbsjets));
  returnValues[deltaphi_lep_ww] = abs(lep.DeltaPhi(whad+vbsjets));
  returnValues[deltaphi_lep_jet0] = abs(lep.DeltaPhi(jet0));
  returnValues[deltaphi_lep_alljets] = abs(lep.DeltaPhi(total_jet));
  returnValues[deltaeta_lep_alljets] = abs(lep.Eta() - total_jet.Eta());
  returnValues[deltaR_lep_alljets] = abs(lep.DeltaR(total_jet));
}
