#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TH1I.h"
#include "TCanvas.h"
#include "TLegend.h"


std::string DecodeProcess(int code);
std::string DecodeVolume(int id);

int timePlot(TString fileName = "histomanager_out2.root")
{
  // TFile* file = new TFile("100k_out2.root", "READ");
  TFile* file = new TFile(fileName, "READ");
  if (!file || file->IsZombie()) {
    std::cerr << "Error opening file!" << std::endl;
    return -1;
  }
  TString outfilename  = fileName(0, fileName.Length()-5);
  TFile* outFile = new TFile("timePlots" + outfilename + ".root", "RECREATE");

  TTree* tree = (TTree*)(file->Get("EventTree0"));
  if (!tree) {
    std::cerr << "Error getting tree!" << std::endl;
    file->Close();
    return -1;
  }



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

  TCanvas* canvas = new TCanvas("Canvas", "Timing Analysis", 800, 600);
  TH1D* BGOTime = new TH1D("BGOTime", "Time difference between first interaction in LaBr3 and BGO; Time [ns]; Counts", 200, -1.2, 1.2);

  double timeDiff =-1;
  double timeLa = -1;
  double timeVe = 0;

  Long64_t nEntries = tree->GetEntries();
  for (Long64_t i = 0; i < nEntries; ++i) {
    tree->GetEntry(i);

    for(const auto& veto : *vetoDataVector)
    {
      timeVe = veto.time;
      break; //only first interaction 
    }
    for(const auto& gam : *gammaDataVector)
    {
       	if(gam.endVolume == 0){
	timeLa = gam.time;
        break;
	}
    }
    if(timeVe!=0){
      BGOTime->Fill(timeVe-timeLa);
    }
    timeVe=0;
    timeLa=-10;

    

  }
  

  canvas->cd();
  gStyle->SetOptStat(1111111);
  BGOTime->Draw();

  // canvas->SetLogy();
  TLegend* legend = new TLegend(0.4, 0.7, 0.7, 0.9);
  legend->AddEntry(BGOTime, "BGO Time", "l");
  legend->Draw();
  canvas->SaveAs("timePlots"+outfilename +  ".pdf");

  outFile->cd();
  canvas->Write();

  outFile->Save();
  // outFile->Close();
  

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
