#include <iostream>
#include <vector>

#include "TH1I.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

void AnalyzeFile(std::string NameOfFile, TH1I& hPhotCountPerEvent);

int main(int argc, char* argv[])
{
    
    if (argc < 2) {
        std::cout << "Not enough arguments. More than two needed: 1: energy, 2:name of the file " << std::endl;
        return 0;
    }

    std::string energy = argv[1];

    //add files to queue (for * it works automatically by bash)
    std::vector<std::string> filesToAnalyze;
    std::string fileOrPattern;
    for (unsigned i=2; i<argc; i++) {
        fileOrPattern = argv[i];
        filesToAnalyze.push_back(fileOrPattern);
    }

    TString histName = "PhotonCountPerEvent_" + energy;
    TFile* outputFile = new TFile("PhotonCountPerEvent.root", "UPDATE");
    TH1I hPhotCountPerEvent(histName, "Photon count per event", 2000, 0, 2000);

    for (unsigned fileNo = 0; fileNo < filesToAnalyze.size(); fileNo++) {
        AnalyzeFile(filesToAnalyze.at(fileNo), hPhotCountPerEvent);
    }
    std::cout << "Saving histogram to file " << outputFile->GetName() << std::endl;

    outputFile->cd();
    std::cout << "Entries in histogram: " << hPhotCountPerEvent.GetEntries() << std::endl;
    hPhotCountPerEvent.Write();
    outputFile->Close();
}

void AnalyzeFile(std::string NameOfFile, TH1I& hPhotCountPerEvent)
{
    std::cout << " Reading file " << NameOfFile << std::endl;
    

    TFile* hfile = new TFile(NameOfFile.c_str(), "READ");
    TTree *ntuple = (TTree *) hfile->Get("T");

    Int_t evNr, Det;

    ntuple->SetBranchAddress("evNr", &evNr);
    ntuple->SetBranchAddress("Det", &Det);

    Int_t nentries = (Int_t)ntuple->GetEntries();

    Int_t currentEvent = -1;
    Int_t photonCount = 0;
    for(Int_t i=0; i<nentries; i++) {
        ntuple->GetEntry(i);

        if(Det == 22) //if photon hit the SiPM
            photonCount++;

        if(currentEvent != evNr) {
            if(currentEvent != -1) //if not the first event, save the count for the previous one
                hPhotCountPerEvent.Fill(photonCount);
            currentEvent = evNr;
            photonCount = 0; //reset count for the new event
        }

        
    }
    
}