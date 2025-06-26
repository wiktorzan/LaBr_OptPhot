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
  cAll, cGZH_XY
};

class HistCollection
{
public:
  HistCollection() {;};
  ~HistCollection() {;};

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
    outfile->Close();
  }

private:
  TH3D* PositionsAll;
  TH3D* PositionsGammaZDetXY;
};

#endif
