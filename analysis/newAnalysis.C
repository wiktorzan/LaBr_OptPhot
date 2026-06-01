#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TH1I.h"
#include "TCanvas.h"


std::string DecodeProcess(int code);
std::string DecodeVolume(int id);

int newAnalysis()
{
  // TFile* file = new TFile("100k_out2.root", "READ");
  TFile* file = new TFile("histomanager_out2.root", "READ");
  if (!file || file->IsZombie()) {
    std::cerr << "Error opening file!" << std::endl;
    return -1;
  }

  TTree* tree = (TTree*)(file->Get("EventTree0"));
  if (!tree) {
    std::cerr << "Error getting tree!" << std::endl;
    file->Close();
    return -1;
  }

  // Process the tree...
  // tree->Draw("Photons.posX:Photons.posY","", "colz");

  std::vector<GammaData>* gammaDataVector = nullptr;
  std::vector<PhotonData>* photonDataVector = nullptr;
  std::vector<VetoData>* vetoDataVector = nullptr;

  tree->SetBranchAddress("GammaTrack", &gammaDataVector);
  tree->SetBranchAddress("Photons", &photonDataVector);
  tree->SetBranchAddress("VetoNr", &vetoDataVector);

  TCanvas* canvas = new TCanvas("Canvas", "Photon Analysis", 800, 600);
  TH1I* photonCountWithShield = new TH1I("PhotonCountWithShield", "Number of photons per event with active veto; Count; Counts", 100, 0., 3000);
  TH1I* photonCountVetoed = new TH1I("PhotonCountVetoed", "Number of photons per event Vetoed; Count; Counts", 100, 0., 3000);
  TH1I* photonCount = new TH1I("PhotonCount", "Number of photons per event; Count; Counts", 100, 0., 3000);
  TH1I* photonCountEscaped = new TH1I("PhotonCountEscaped", "Number of photons per event where gamma escaped; Count; Counts", 100, 0., 3000);

  Long64_t nEntries = tree->GetEntries();
  for (Long64_t i = 0; i < nEntries; ++i) {
    tree->GetEntry(i);
    std::cout << "Event " << i << ": " << photonDataVector->size() << " photons detected." << std::endl;

    bool escapeEvent = false;
    for(const auto& gamma : *gammaDataVector)
    {
      std::cout << "  Gamma track ID: " << gamma.trackID << ", energy deposited: " 
      << gamma.eDep << " keV, creator process: " << DecodeProcess(gamma.creatorProcess) 
      << " " << DecodeProcess(gamma.interactionProcess)<< " " 
      << DecodeVolume(gamma.endVolume) << std::endl;
      if(gamma.endVolume == 5) // Reached World
      {
        escapeEvent = true;
      }
    }
    if(escapeEvent)
    {
      photonCountEscaped->Fill(photonDataVector->size());
    }

    if(vetoDataVector->size() > 0)
    {
      // std::cout << "  Veto triggered with " << vetoDataVector->size() << " entries." << std::endl;
      photonCountVetoed->Fill(photonDataVector->size());
      photonCount->Fill(photonDataVector->size());
    }else{
      // std::cout << "  No veto triggered." << std::endl;
      photonCountWithShield->Fill(photonDataVector->size());
      photonCount->Fill(photonDataVector->size());
    }

    
  }
  std::cout << "Average number of photons per event: " << photonCountWithShield->GetMean() << std::endl;
  

  canvas->cd();
  photonCountWithShield->SetLineColor(kBlack);
  photonCountWithShield->SetLineWidth(2);
  photonCountWithShield->Draw();
  photonCountVetoed->SetLineColor(kRed);
  photonCountVetoed->SetLineWidth(2);
  photonCountVetoed->Draw("SAME");
  photonCount->SetLineColor(kGreen);
  photonCount->SetLineWidth(2);
  photonCount->Draw("SAME");
  photonCountEscaped->SetLineColor(kBlue);
  photonCountEscaped->SetLineWidth(2);
  photonCountEscaped->Draw("SAME");
  canvas->BuildLegend();
  

  // file->Close();
  return 0;
}

std::string DecodeProcess(int code)
{
    switch(code)
    {
        case 0:  return "compt";
        case 1:  return "phot";
        case 2:  return "conv";
        case 3:  return "eIoni";
        case 4:  return "hIoni";
        case 5:  return "msc";
        case 6:  return "Scintillation";
        case 7:  return "Cerenkov";
        case 8:  return "eBrem";
        case 9:  return "Rayl";
        case 10: return "Transportation";
        case 11: return "unknown";
        default: return "invalid";
    }
}

std::string DecodeVolume(int id)
{
    switch (id)
    {
        case 0: return "Physi_LaBr3";
        case 1: return "reflectorface";
        case 2: return "GlassWindow";
        case 3: return "Physi_SiPM";
        case 4: return "BGO";
        case 5: return "World";
        case 6: return "BGOW";
        case -3: return "Null";
        default: return "UNKNOWN";
    }
}