#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TH1I.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TRandom.h"



std::string DecodeProcess(int code);
std::string DecodeVolume(int id);
double SmearEnergy(double energy);
void energyPlot(TString filename, double maxEnergy, bool smearEnergy, bool plotGraphs);

struct GammaData{
  int trackID;
  double eDep;
  double time;
  int creatorProcess;
  int interactionProcess;
  int endVolume;
  double posX;
  double posY;
  double posZ;

  ClassDef(GammaData, 6);
};

struct PhotonData{
  int trackID;
  int process;
  double posX;
  double posY;
  double posZ;
  int copyNo;

  ClassDef(PhotonData, 2);
};

struct VetoData{
  int trackID;
  int copyNo;
  double time;
  ClassDef(VetoData, 3);
};


int main(int argc, char** argv)
{

  double maxEnergy = -1;
  bool plotGraphs = false;
  bool smearEnergy = false;
  bool includeBackVeto = false;
  std::vector<std::string> files;


  for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "-e" && i + 1 < argc) {
          maxEnergy = std::atof(argv[++i]);
      } else if (arg == "-g") {
          plotGraphs = true;
      } else if (arg == "-s") {
          smearEnergy = true;
      } else if (arg == "-b") {
          includeBackVeto = true;
      } else {
          files.push_back(arg);
      }
  }

  if (files.empty()) {
    std::cerr << "Nie podano zadnych plikow .root!" << std::endl;
    return 1;
  }

  for (size_t i = 0; i < files.size(); ++i) {
    std::cout << "Processing file: " << files[i] << std::endl;
    energyPlot(files[i], maxEnergy, smearEnergy, plotGraphs);
  }


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

double SmearEnergy(double energy)
{

  double a = 2.0*pow(10, -4); // in MeV
  double b = 2.22*pow(10, -2);
  double c = 0.5;

  double EngSmr = gRandom->Gaus(0,1)*(a + b*sqrt(energy + c*pow(energy, 2)))/(2.35482004503);//original

  return energy + EngSmr;
}

void energyPlot(TString filename, double maxEnergy, bool smearEnergy, bool plotGraphs)
{
  TFile* file = new TFile(filename, "READ");
  if (!file || file->IsZombie()) {
    std::cerr << "Error opening file!" << std::endl;
    return;
  }
  TString outfilename = filename(0, filename.Length()-5);
  if(smearEnergy)
    outfilename = outfilename + "_sm";
  TFile* outFile = new TFile("energyPlots"+outfilename+".root", "RECREATE");

  TTree* tree = (TTree*)(file->Get("EventTree0"));
  if (!tree) {
    std::cerr << "Error getting tree!" << std::endl;
    file->Close();
    return;
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

  TH1D* EnergyWithShield = new TH1D("EnergyWithShield", "Energy per event with active veto; Energy[keV]; Counts", 100, 0., maxEnergy);
  TH1D* EnergyWithShieldAndBack = new TH1D("EnergyWithShieldAndBack", "Energy per event with active veto and back; Energy[keV]; Counts", 100, 0., maxEnergy);
  TH1D* EnergyVetoed = new TH1D("EnergyVetoed", "Energy per event Veto; Energy[keV]; Counts", 100, 0., maxEnergy);
  TH1D* EnergyVetoedBack = new TH1D("EnergyVetoedBack", "Energy per event Veto back; Energy[keV]; Counts", 100, 0., maxEnergy);
  TH1D* Energy = new TH1D("Energy", "Energy per event; Energy[keV]; Counts", 100, 0., maxEnergy);
  TH1D* EnergyEscaped = new TH1D("EnergyEscaped", "Energy per event where gamma escaped; Energy[keV]; Counts", 100, 0., maxEnergy);
  TH1D* EnergyNotEscaped = new TH1D("EnergyNotEscaped", "Energy per event where gamma did not escape; Energy[keV]; Counts", 100, 0., maxEnergy);

  Long64_t nEntries = tree->GetEntries();
  for (Long64_t i = 0; i < nEntries; ++i) {
    tree->GetEntry(i);
    double ene = totalEnergy;
    if(ene <= 0) continue;
    if(smearEnergy){ene = SmearEnergy(totalEnergy);}


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
  

    Energy->Fill(ene);


    bool vetoBack = false;
    bool vetoSide = false;
    for(const auto& veto : *vetoDataVector)
    {
      if(veto.copyNo == -1) // BGO_back
      {
        vetoBack = true;
      }
      if(veto.copyNo != -1) // BGO
      {
        vetoSide = true;
      }
    }

    if(vetoBack)
    {
      EnergyVetoedBack->Fill(ene);
    }
    if(vetoSide)
    {
      EnergyVetoed->Fill(ene);
    }



    if(!vetoSide)
    {
      EnergyWithShield->Fill(ene);
    }

    if(!vetoSide && !vetoBack)
    {
      EnergyWithShieldAndBack->Fill(ene);
    }
    
  }
  std::cout << "Average energy per event: " << EnergyWithShield->GetMean() << std::endl;
  



    
  TCanvas* canvas = new TCanvas("ShieldCanvas", "Spectrum after veto", 800, 600);
  TCanvas* canvas2 = new TCanvas("Canvas2", "Escaped gamma Analysis", 800, 600);
  TCanvas* canvas3 = new TCanvas("VetoedCanvas", "Vetoed gamma Analysis", 800, 600);


  canvas->cd();
  Energy->SetLineColor(kBlack);
  Energy->SetLineWidth(2);
  Energy->SetMinimum(1);
  Energy->SetStats(0);
  Energy->Draw();
  EnergyWithShield->SetLineColor(kGreen);
  EnergyWithShield->SetLineWidth(2);
  EnergyWithShield->SetMinimum(1);
  EnergyWithShield->Draw("SAME");
  EnergyWithShieldAndBack->SetLineColor(kMagenta);
  EnergyWithShieldAndBack->SetLineWidth(2);
  EnergyWithShieldAndBack->SetMinimum(1);
  EnergyWithShieldAndBack->Draw("SAME");

  canvas->SetLogy();
  TLegend* legend = new TLegend(0.2, 0.7, 0.7, 0.9);
  legend->AddEntry(Energy, "All Events", "l");
  legend->AddEntry(EnergyWithShield, "Events with Active Shield", "l");
  legend->AddEntry(EnergyWithShieldAndBack, "Events with Active Shield and Back", "l");
  legend->Draw();
  

  canvas3->cd();
  Energy->SetLineColor(kBlack);
  Energy->SetLineWidth(2);
  Energy->SetMinimum(1);
  Energy->SetStats(0);
  Energy->Draw();
  EnergyVetoed->SetLineColor(kRed);
  EnergyVetoed->SetLineWidth(2);
  EnergyVetoed->SetMinimum(1);
  EnergyVetoed->Draw("SAME");
  EnergyVetoedBack->SetLineColor(kBlue);
  EnergyVetoedBack->SetLineWidth(2);
  EnergyVetoedBack->SetMinimum(1);
  EnergyVetoedBack->Draw("SAME");

  canvas3->SetLogy();
  TLegend* legend3 = new TLegend(0.2, 0.7, 0.7, 0.9);
  legend3->AddEntry(Energy, "All Events", "l");
  legend3->AddEntry(EnergyVetoed, "Vetoed Events", "l");
  legend3->AddEntry(EnergyVetoedBack, "Vetoed Events Back", "l");
  legend3->Draw();


  canvas2->cd();
  Energy->SetLineColor(kBlack);
  Energy->SetLineWidth(2);
  Energy->Draw();
  Energy->SetMinimum(1);
  EnergyEscaped->SetLineColor(kBlue);
  EnergyEscaped->SetLineWidth(2);
  EnergyEscaped->SetMinimum(1);
  EnergyEscaped->Draw("SAME");
  EnergyNotEscaped->SetLineColor(kCyan);
  EnergyNotEscaped->SetLineWidth(2);
  EnergyNotEscaped->SetMinimum(1);
  EnergyNotEscaped->Draw("SAME");
  canvas2->SetLogy();
  TLegend* legend2 = new TLegend(0.2, 0.7, 0.7, 0.9);
  legend2->AddEntry(Energy, "All Events", "l");
  legend2->AddEntry(EnergyEscaped, "Events with gamma escaped", "l");
  legend2->AddEntry(EnergyNotEscaped, "Events where gamma did not escape LaBr3", "l");
  legend2->Draw();

  if(plotGraphs)
  {
    canvas->SaveAs("energyPlots_"+outfilename+".pdf");
    canvas2->SaveAs("energyPlots2_"+outfilename+".pdf");
  }



  outFile->cd();
  canvas->Write();
  canvas2->Write();
  canvas3->Write();
  EnergyWithShield->Write();
  EnergyWithShieldAndBack->Write();
  EnergyVetoed->Write();
  EnergyVetoedBack->Write();
  Energy->Write();
  EnergyEscaped->Write();
  EnergyNotEscaped->Write();
  outFile->Save();

  outFile->Close();
  file->Close();

}