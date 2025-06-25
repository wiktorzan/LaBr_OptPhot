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

bool FileCheck(const std::string& NameOfFile);
void AnalyzeFile(std::string NameOfFile);

int main(int argc, char* argv[])
{
//--------------------------------------------------------
//Reading parameters
  if (argc < 2) {
    std::cout << "Not enough arguments. More than two needed: name of the file | name of the second file ..." << std::endl;
    return 0;
  }

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

  return 0;
}

void AnalyzeFile(std::string NameOfFile)
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

  int currentEvent = -1;
  for (Int_t i=0; i<nentries; i++) {
    ntuple->GetEntry(i);
  }
}

bool FileCheck(const std::string& NameOfFile)
{
    struct stat buffer;
    return (stat(NameOfFile.c_str(), &buffer) == 0);
}
