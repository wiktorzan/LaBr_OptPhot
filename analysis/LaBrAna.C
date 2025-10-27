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
void FindTriggerTimes(std::string NameOfFile, std::map<Int_t, Double_t> *triggerTimes);
void AnalyzeFile(std::string NameOfFile, HistCollection histo);

int main(int argc, char* argv[])
{
//--------------------------------------------------------
//Reading parameters
  if (argc < 2) {
    std::cout << "Not enough arguments. More than two needed: name of the file | name of the second file ..." << std::endl;
    return 0;
  }

  Double_t minMaxDim = 30, binSizeSmall = 1;// in mm
  Double_t minTime = 0, maxTime = 50, binSize = 0.2; // in ns
  HistCollection hist;
  hist.CreatePositionHistos(minMaxDim, binSizeSmall);
  hist.CreateTimingHistos(minTime, maxTime, binSize);
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
  std::map<Int_t, Double_t>* triggerTimes = new std::map<Int_t, Double_t>();
  std::cout << " Looking for triggers in  " << NameOfFile << std::endl;
  FindTriggerTimes(NameOfFile, triggerTimes);

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

  Int_t currentEvent = -1;
  Double_t gammaZ = -35;
  for (Int_t i=0; i<nentries; i++) {
    ntuple->GetEntry(i);
    
    // if(evNr == 940) {
    //   // std::cout << "Event 4105: pType " << pType << " pName " << pName << " KE " << KE << " Edep " << Edep
    //   //           << " posX " << posX << " posY " << posY << " posZ " << posZ
    //   //           << " Det " << Det << " CopyNo " << CopyNo << " time " << time
    //   //           << " momX " << momX << " momY " << momY << " momZ " << momZ << std::endl;
    //   std::cout << "Event 4105: pType " << pType << " pName " << pName 
    //             << " Det " << Det << " CopyNo " << CopyNo << " time " << time
    //              << std::endl;
    // }else continue;

    if (pType == 0) {
      gammaZ = posZ;
    }

    if (evNr == currentEvent) {
      TVector3 pos(posX, posY, posZ);
      hist.FillPositionHisto(pos, HistoLabel::cAll);
      if (Det == 22) {
        TVector3 pos2(posX, posY, gammaZ);
        hist.FillPositionHisto(pos2, HistoLabel::cGZH_XY);
        Double_t ToA = time - (*triggerTimes)[evNr];
        if (ToA < 0) std::cout << "Warning! Negative ToA = " << ToA << " for event " << evNr << " in file " << NameOfFile << std::endl;
        hist.FillTimingHisto(time, CopyNo, HistoLabel::TGlobal);
        // std::cout << "Time " << time << std::endl;
        hist.FillTimingHisto(ToA, CopyNo, HistoLabel::TTrigger);
        hist.FillTimevsDepth(ToA, gammaZ, HistoLabel::all);
        if (CopyNo == 21) {hist.FillTimevsDepth(ToA, gammaZ, HistoLabel::single);}
      }
    } else {
      currentEvent = evNr;
      // std::cout << " Trigger time for event " << evNr << " is " << (*triggerTimes)[evNr] << " ns" << std::endl;
    }
  }
}

bool FileCheck(const std::string& NameOfFile)
{
    struct stat buffer;
    return (stat(NameOfFile.c_str(), &buffer) == 0);
}

void FindTriggerTimes(std::string NameOfFile, std::map<Int_t, Double_t>* triggerTimes)
{
  TString fileName = NameOfFile;
  TFile* hfile = new TFile(fileName, "READ");
  TTree *ntuple = (TTree *) hfile->Get("T");

  Int_t evNr, pType, pName, Det, CopyNo;
  Double_t time;
  ntuple->SetBranchAddress("evNr", &evNr);
  ntuple->SetBranchAddress("pType", &pType);
  ntuple->SetBranchAddress("pName", &pName);
  ntuple->SetBranchAddress("Det", &Det);
  ntuple->SetBranchAddress("CopyNo", &CopyNo);
  ntuple->SetBranchAddress("Gtime", &time);


  Int_t nentries = (Int_t)ntuple->GetEntries();

  Int_t currentEvent = -1;
  Double_t triggerTime = 1000;
  for (Int_t i=0; i<nentries; i++) {
    ntuple->GetEntry(i);

    if (evNr != currentEvent) {
      

      if(triggerTime < 1000) {
        (*triggerTimes)[currentEvent] = triggerTime;
        // std::cout << " Trigger time for event " << evNr << " is " << (*triggerTimes)[evNr] << " ns" << std::endl;
      }
      currentEvent = evNr;
      triggerTime = 1000;
    }

    if(Det == 22) {
      if(triggerTime > time) {triggerTime = time;}
    }
  }
  if(triggerTime < 1000) {
    (*triggerTimes)[currentEvent] = triggerTime;
  }

  hfile->Close();
}