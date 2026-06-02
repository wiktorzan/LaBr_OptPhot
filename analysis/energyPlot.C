#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TH1I.h"
#include "TCanvas.h"


std::string DecodeProcess(int code);
std::string DecodeVolume(int id);
double SmearEnergy(double energy);

int energyPlot()
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
  double totalEnergy = 0.;
  int eventID = 0;

  tree->SetBranchAddress("EventNr", &eventID);
  tree->SetBranchAddress("TotalEnergy", &totalEnergy);
  tree->SetBranchAddress("GammaTrack", &gammaDataVector);
  tree->SetBranchAddress("Photons", &photonDataVector);
  tree->SetBranchAddress("VetoNr", &vetoDataVector);

  TCanvas* canvas = new TCanvas("Canvas", "Photon Analysis", 800, 600);
  TH1D* EnergyWithShield = new TH1D("EnergyWithShield", "Energy per event with active veto; Energy[keV]; Counts", 100, 0., 520);
  TH1D* EnergyVetoed = new TH1D("EnergyVetoed", "Energy per event VetoEnergy[keV]; Counts", 100, 0., 520);
  TH1D* Energy = new TH1D("Energy", "Energy per event Energy[keV]; Counts", 100, 0., 520);
  TH1D* EnergyEscaped = new TH1D("EnergyEscaped", "Energy per event where gamma escaped Energy[keV]; Counts", 100, 0., 520);
  TH1D* EnergyNotEscaped = new TH1D("EnergyNotEscaped", "Energy per event where gamma did not escape Energy[keV]; Counts", 100, 0., 520);

  Long64_t nEntries = tree->GetEntries();
  for (Long64_t i = 0; i < nEntries; ++i) {
    tree->GetEntry(i);
    std::cout << "Event " << eventID << ": " << photonDataVector->size() << " photons detected." << std::endl;
    double ene = SmearEnergy(totalEnergy);

    bool escapeEvent = false;
    for(const auto& gamma : *gammaDataVector)
    {
      // std::cout << "  Gamma track ID: " << gamma.trackID << ", energy deposited: " 
      // << gamma.eDep << " keV, creator process: " << DecodeProcess(gamma.creatorProcess) 
      // << " " << DecodeProcess(gamma.interactionProcess)<< " " 
      // << DecodeVolume(gamma.endVolume) << std::endl;
      if(gamma.endVolume == 5) // Reached World
      {
        escapeEvent = true;
      }
    }
    if(escapeEvent)
    {
      EnergyEscaped->Fill(ene);
    }else
    {
      EnergyNotEscaped->Fill(ene);
    }

    if(vetoDataVector->size() > 0)
    {
      // std::cout << "  Veto triggered with " << vetoDataVector->size() << " entries." << std::endl;
      EnergyVetoed->Fill(ene);
      Energy->Fill(ene);
    }else{
      // std::cout << "  No veto triggered." << std::endl;
      EnergyWithShield->Fill(ene);
      Energy->Fill(ene);
    }

    
  }
  std::cout << "Average energy per event: " << EnergyWithShield->GetMean() << std::endl;
  

  canvas->cd();
  EnergyWithShield->SetLineColor(kBlack);
  EnergyWithShield->SetLineWidth(2);
  EnergyWithShield->Draw();
  EnergyVetoed->SetLineColor(kRed);
  EnergyVetoed->SetLineWidth(2);
  EnergyVetoed->Draw("SAME");
  Energy->SetLineColor(kGreen);
  Energy->SetLineWidth(2);
  Energy->Draw("SAME");
  EnergyEscaped->SetLineColor(kBlue);
  EnergyEscaped->SetLineWidth(2);
  EnergyEscaped->Draw("SAME");
  EnergyNotEscaped->SetLineColor(kCyan);
  EnergyNotEscaped->SetLineWidth(2);
  EnergyNotEscaped->Draw("SAME");
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


double SmearEnergy(double energy)
{
  double a = 2.0*pow(10, -4); // in MeV
  double b = 2.22*pow(10, -2);
  double c = 0.5;

  double EngSmr = gRandom->Gaus(0,1)*(a + b*sqrt(energy + c*pow(energy, 2)))/(2.35482004503);//original

  return energy + EngSmr;
}