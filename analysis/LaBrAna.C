#include <sys/stat.h>
#include <functional>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <sstream>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>

#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TRandom3.h"
#include "TVector3.h"
#include "TDatime.h"
#include "TNtuple.h"
#include "TObject.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TStyle.h"
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"

#include "Histo_Collection.h"

bool FileCheck(const std::string& NameOfFile);
void AnalyzeFile(std::string NameOfFile, HistCollection histo);
void SavePhotonCount(TH1* hist, std::ofstream& csv, std::string fileName);

int main(int argc, char* argv[])
{
//--------------------------------------------------------
//Reading parameters
  if (argc < 2) {
    std::cout << "Not enough arguments. More than two needed: name of the file | name of the second file ..." << std::endl;
    return 0;
  }

  Double_t minMaxDim = 30, binSizeSmall = 1;// in mm
  HistCollection hist;
  hist.CreatePositionHistos(minMaxDim, binSizeSmall);
  TString outputName = "";
  std::string fileOrPattern = argv[1];

//Analysis
  std::vector<std::string> filesToAnalyze;

  if (argc > 2) {
    outputName = "Out_LastFile_" + fileOrPattern;
    filesToAnalyze.push_back(fileOrPattern);
    for (unsigned i=2; i<argc; i++) {
      fileOrPattern = argv[i];
      filesToAnalyze.push_back(fileOrPattern);
    }
    for (unsigned fileNo = 0; fileNo < filesToAnalyze.size(); fileNo++) {
      AnalyzeFile(filesToAnalyze.at(fileNo), hist);
    }
  } else {
    TString root_file = fileOrPattern;
    TString star = "*";
    TString slash = "/";
    std::size_t starPlace = fileOrPattern.rfind(star);
    std::size_t slashPlace = fileOrPattern.rfind(slash);

    if (root_file[strlen(root_file) - 1] == star) {
      std::string pattern;
      std::string dirName;
      if (slashPlace > 0) {
        dirName = fileOrPattern.substr(0, slashPlace+1);
        pattern = fileOrPattern.substr(slashPlace+1, starPlace-slashPlace-1);
      } else {
        dirName = "";
        pattern = fileOrPattern.substr(0, starPlace);
      }
      std::cout << "Getting files from directory: " << dirName << " and pattern " << pattern << std::endl;
      TString dirNameRoot = dirName;
      if (dirNameRoot == "")
        dirNameRoot = ".";
      TSystemDirectory dir(dirNameRoot, dirNameRoot);
      TList *files = dir.GetListOfFiles();

      if (files) {
        TSystemFile *file;
        TString fname;
        TIter next(files);
        while ((file = (TSystemFile*)next())) {
          fname = file->GetName();

          if (!file->IsDirectory() && fname.EndsWith(".root") && fname.BeginsWith(pattern)) {
            std::cout << "Adding file to the analysis queue " << fname.Data() << std::endl;
            filesToAnalyze.push_back(dirName + fname.Data());
          }
        }
      }
      outputName = "Out_Patt_" + pattern + ".root";
    } else {
      filesToAnalyze.push_back(fileOrPattern);
      outputName = "Out_" + fileOrPattern;
    }

    for (unsigned fileNo = 0; fileNo < filesToAnalyze.size(); fileNo++) {
      AnalyzeFile(filesToAnalyze.at(fileNo), hist);
    }
  }
  hist.SaveHistos(outputName);

  return 0;
}

void AnalyzeFile(std::string NameOfFile, HistCollection hist)
{
  TString fileName = NameOfFile;
  std::cout << " Reading file " << fileName << std::endl;
  TFile* hfile = new TFile(fileName, "READ");
  TTree *ntuple = (TTree *) hfile->Get("T");

  Int_t evNr, pType, pName, Det, CopyNo;
  Double_t KE, Edep, posX, posY, posZ, time, momX, momY, momZ;
  ntuple->SetBranchAddress("evNr", &evNr);
  ntuple->SetBranchAddress("pType", &pType);
  ntuple->SetBranchAddress("pName", &pName);
  ntuple->SetBranchAddress("KE", &KE);
  ntuple->SetBranchAddress("Edep", &Edep);
  ntuple->SetBranchAddress("postPosX", &posX);
  ntuple->SetBranchAddress("postPosY", &posY);
  ntuple->SetBranchAddress("postPosZ", &posZ);
  ntuple->SetBranchAddress("Det", &Det);
  ntuple->SetBranchAddress("CopyNo", &CopyNo);
  ntuple->SetBranchAddress("Gtime", &time);
  ntuple->SetBranchAddress("momentumX", &momX);
  ntuple->SetBranchAddress("momentumY", &momY);
  ntuple->SetBranchAddress("momentumZ", &momZ);;

  Int_t nentries = (Int_t)ntuple->GetEntries();

  //photon count per SiPM
  TH1I hOpt_count("OpticalCount", "Optical photon count per SiPM", 52, 0, 52);
  FILE *file = fopen("OpticalCount.csv", "a");
  std::ofstream csv("OpticalCount.csv",std::ios_base::app);

  Int_t currentEvent = -1;
  Double_t gammaZ = -35;
  for (Int_t i=0; i<nentries; i++) {
    ntuple->GetEntry(i);

    if(Det == 22) //if photon hit the SiPM
      hOpt_count.Fill(CopyNo);

    if (pType == 0) {
      gammaZ = posZ;
    }

    if (evNr == currentEvent) {
      TVector3 pos(posX, posY, posZ);
      hist.FillPositionHisto(pos, HistoLabel::cAll);
      if (Det == 22) {
        TVector3 pos2(posX, posY, gammaZ);
        hist.FillPositionHisto(pos2, HistoLabel::cGZH_XY);
      }
    } else {
      currentEvent = evNr;
      //if current event changed - save histo and reset it for the next event
      if(hOpt_count.GetEntries() > 0) 
        SavePhotonCount(&hOpt_count, csv, NameOfFile);
      hOpt_count.Reset();
    }
  }
  csv.close();
}

bool FileCheck(const std::string& NameOfFile)
{
    struct stat buffer;
    return (stat(NameOfFile.c_str(), &buffer) == 0);
}

void SavePhotonCount(TH1* hist, std::ofstream& csv, std::string fileName)
{
  int bins = hist->GetNbinsX();
  csv << "0;0;";
  for (int i=1; i<5; i++)
    csv << hist->GetBinContent(i) << ";";
  csv << "0;0;" << "0;";
  for (int i=5; i<11; i++)
    csv << hist->GetBinContent(i) << ";";
  csv << "0;";
  for (int i=11; i<43; i++)
    csv << hist->GetBinContent(i) << ";";
  csv << "0;";
  for (int i=43; i<49; i++)
    csv << hist->GetBinContent(i) << ";";
  csv << "0;"<< "0;0;";
  for (int i=49; i<=bins; i++) 
    csv << hist->GetBinContent(i) << ";";
  csv << "0;0;";


  // int shift = 0;
  // if(fileName[5] == '-'){
  //   csv << fileName.substr(5, 5) << ";";
  //   shift = 1;
  // } else {
  //   csv << fileName.substr(5, 4) << ";";
  //   shift = 0;
  // }

  // if(fileName[10 + shift] == '-')
  //   csv << fileName.substr(10 + shift, 5);
  // else
  //   csv << fileName.substr(10 + shift, 4);

  csv << fileName.substr(12,3) << ";";
  csv << fileName.substr(16,3);

  csv << std::endl;
}