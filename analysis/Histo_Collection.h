#ifndef Histo_Collection_h
#define Histo_Collection_h

#include "TVector3.h"
#include "TLatex.h"
#include "TAxis.h"
#include "TFile.h"
#include "TLine.h"
#include "TF1.h"
#include "TF2.h"
#include "TF3.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

enum HistoLabel {
  cAll, cGZH_XY, TGlobal, TTrigger, all, single
};

class HistCollection
{
public:
  HistCollection() {;};
  ~HistCollection() {;};

  void CreateTimingHistos(double minTime, double maxTime, double binSize)
  {
    double_t binSizeZ = 1;
    double_t minZ = -30;
    double maxZ = 30;

    ToAvsSiPMNoGlobal = new TH2D("ToAvsSiPMNoGlobal","SiPM number vs Time of Arrival; ToA [ns]; SiPM number",
                           (maxTime-minTime)/binSize, minTime-0.5*binSize, maxTime-0.5*binSize,60, 0, 60);
    ToAvsSiPMNoTrigger = new TH2D("ToAvsSiPMNoTrigger","SiPM number vs Time of Arrival with trigger; ToA [ns]; SiPM number",
                           (maxTime-minTime)/binSize, minTime-0.5*binSize, maxTime-0.5*binSize,60, 0, 60);
    ToAvsZallSiPM = new TH2D("ToAvsZallSiPM","Interactions depth vs Time of Arrival with trigger; ToA [ns]; Depth",
                           (maxTime-minTime)/binSize, minTime-0.5*binSize, maxTime-0.5*binSize,(maxZ-minZ)/binSizeZ, minZ-0.5*binSizeZ, maxZ - 0.5*binSizeZ);
    ToAvsZSingleSiPM = new TH2D("ToAvsZSingleSiPM","Interactions depth vs Time of Arrival with trigger; ToA [ns]; Depth",
                           (maxTime-minTime)/binSize, minTime-0.5*binSize, maxTime-0.5*binSize,(maxZ-minZ)/binSizeZ, minZ-0.5*binSizeZ, maxZ - 0.5*binSizeZ);
    
  }

  void CreatePositionHistos(double minMaxDim, double binSize)
  {
    PositionsAll = new TH3D("PositionsAll","Positions; X [mm]; Y [mm]; Z [mm]", 2*minMaxDim/binSize, -minMaxDim-0.5*binSize, minMaxDim-0.5*binSize,
                            2*minMaxDim/binSize, -minMaxDim-0.5*binSize, minMaxDim-0.5*binSize,
                            2*minMaxDim/binSize, -minMaxDim-0.5*binSize, minMaxDim-0.5*binSize);
    PositionsGammaZDetXY = new TH3D("PositionsGammaZDetXY","Positions; X of hit [mm]; Y of hit [mm]; Z of gamma [mm]",
                                    2*minMaxDim/binSize, -minMaxDim-0.5*binSize, minMaxDim-0.5*binSize,
                                    2*minMaxDim/binSize, -minMaxDim-0.5*binSize, minMaxDim-0.5*binSize,
                                    2*minMaxDim/binSize, -minMaxDim-0.5*binSize, minMaxDim-0.5*binSize);
  }

  void FillTimingHisto(double ToA, int SiPMNo, HistoLabel label) {
    switch (label) {
      case HistoLabel::TGlobal:
        ToAvsSiPMNoGlobal->Fill(ToA, SiPMNo);
        break;
      case HistoLabel::TTrigger:
        ToAvsSiPMNoTrigger->Fill(ToA, SiPMNo);
        break;
    }
  }

  void FillTimevsDepth(double Toa, double z, HistoLabel label)
  {
    switch (label)
    {
    case all:
      ToAvsZallSiPM->Fill(Toa, z);
      break;
    
    case single:
      ToAvsZSingleSiPM->Fill(Toa, z);

    break;
    }

  }

  void FillPositionHisto(TVector3 vector, HistoLabel label) {
    switch (label) {
      case HistoLabel::cAll:
        PositionsAll->Fill(vector.X(), vector.Y(), vector.Z());
        break;
      case HistoLabel::cGZH_XY:
        PositionsGammaZDetXY->Fill(vector.X(), vector.Y(), vector.Z());
        break;
    }
  }

  void SaveHistos(TString output)
  {
    std::cout << "Saving histos to " << output << std::endl;
    TFile* outfile = new TFile(output, "RECREATE");
    outfile->cd();
    PositionsAll->Write("PositionsAll");
    PositionsGammaZDetXY->Write("PositionsGammaZDetXY");
    ToAvsSiPMNoGlobal->Write("ToAvsSiPMNoGlobal");
    ToAvsSiPMNoTrigger->Write("ToAvsSiPMNoTrigger");

    ToAvsZallSiPM->Write();
    ToAvsZSingleSiPM->Write();
    outfile->Close();
  }

private:
  TH3D* PositionsAll;
  TH3D* PositionsGammaZDetXY;
  TH2D* ToAvsSiPMNoGlobal;
  TH2D* ToAvsSiPMNoTrigger;
  TH2D* ToAvsZallSiPM;
  TH2D* ToAvsZSingleSiPM;
};

#endif
