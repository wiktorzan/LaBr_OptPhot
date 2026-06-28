#include <iostream>
#include <vector>

#include "TH1I.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

void AnalyzeFile(std::string NameOfFile, TString histName);

void AnalyzeFile2(std::string NameOfFile, TString histName);

TFile* outputFile = new TFile("PhotonCountPerEvent.root", "UPDATE");

int main(int argc, char* argv[])
{
    
    if (argc < 2) {
        std::cout << "Not enough arguments. More than two needed: 1: energy, 2:name of the file " << std::endl;
        return 0;
    }

    std::string energy = argv[1];

    //add files to queue (for * it works automatically zby bash)
    std::vector<std::string> filesToAnalyze;
    std::string fileOrPattern;
    for (unsigned i=2; i<argc; i++) {
        fileOrPattern = argv[i];
        filesToAnalyze.push_back(fileOrPattern);
    }

    TString histName = "PhotonCnt_" + energy;
    
    

    for (unsigned fileNo = 0; fileNo < filesToAnalyze.size(); fileNo++) {
        if(filesToAnalyze.at(fileNo)[0] != 't' && filesToAnalyze.at(fileNo)[0] != 's') {
            AnalyzeFile2(filesToAnalyze.at(fileNo), histName);
        } else
        AnalyzeFile(filesToAnalyze.at(fileNo), histName);
    }
    std::cout << "Saving histogram to file " << outputFile->GetName() << std::endl;

    
    
    outputFile->Close();
}

void AnalyzeFile(std::string NameOfFile, TString name)
{
    std::cout << " Reading file " << NameOfFile << std::endl;
    

    TFile* hfile = new TFile(NameOfFile.c_str(), "READ");

    Int_t evNr, creatorProcess;
    TTree* ntuple = nullptr;
    TString histName;
    if(NameOfFile[0] == 's') {
        ntuple = (TTree *) hfile->Get("steps");
        histName = name + "_steps_ab";
        ntuple->SetBranchAddress("Process", &creatorProcess);
    } else if(NameOfFile[0] == 't') {
        ntuple = (TTree *) hfile->Get("tracks");
        histName = name + "_tracks";
    } else {
        ntuple = (TTree *) hfile->Get("T");
        histName = name + "_T";
    }
    TH1I hPhotCountPerEvent(histName, "Photon count per gamma", 500, 0, 160000);
    TH1I hPhotCountPerEvent_tr(histName + "_tr", "Photon count per gamma", 500, 0, 60000);
    
    

    ntuple->SetBranchAddress("EventID", &evNr);

    Int_t nentries = (Int_t)ntuple->GetEntries();

    Int_t currentEvent = -1;
    Int_t photonCount = 0;
    Int_t trPhotonCount = 0;
    for(Int_t i=0; i<nentries; i++) {
        ntuple->GetEntry(i);

        if(NameOfFile[0] ==  's') { //if it's a step tree and the process is absorption, count the photon
            if(creatorProcess == 1) {
                photonCount++;
            }else if(creatorProcess == 2) {
                trPhotonCount++;
            }
        } else photonCount++;

        if(currentEvent != evNr) {
            if(currentEvent != -1){ //if not the first event, save the count for the previous one
                if(NameOfFile[0] == 's') {
                    hPhotCountPerEvent.Fill(photonCount);
                    hPhotCountPerEvent_tr.Fill(trPhotonCount);
                } else {
                hPhotCountPerEvent.Fill(photonCount);
                }}
            std::cout << "Event " << evNr << ": " << photonCount << " photons" << std::endl;
            currentEvent = evNr;
            photonCount = 0; //reset count for the new event
            trPhotonCount = 0;
            
        }

        
    }

    outputFile->cd();
    hPhotCountPerEvent.Write();
    if(NameOfFile[0] == 's') {
        hPhotCountPerEvent_tr.Write();
    }
    std::cout << "Entries in histogram: " << hPhotCountPerEvent.GetEntries() << std::endl;
}

void AnalyzeFile2(std::string NameOfFile, TString name)
{
    std::cout << " Reading file " << NameOfFile << std::endl;
    

    TFile* hfile = new TFile(NameOfFile.c_str(), "READ");

    TTree* ntuple = (TTree*)hfile->Get("T");
    TString histName = name + "_out";

    TH1I hPhotCountPerEvent(histName, "Photon count per gamma", 500, 0, 60000);


    Int_t evNr, pName, Det;
    ntuple->SetBranchAddress("evNr", &evNr);
    ntuple->SetBranchAddress("pName", &pName);
    ntuple->SetBranchAddress("Det", &Det);

    Int_t nentries = (Int_t)ntuple->GetEntries();

    Int_t currentEvent = -1;
    Int_t photonCount = 0;

    for(Int_t i=0; i<nentries; i++) {
        ntuple->GetEntry(i);

            if(Det == 22) { //if it's an optical photon, count it
                photonCount++;
            }

        if(currentEvent != evNr) {
            if(currentEvent != -1){ //if not the first event, save the count for the previous one
                hPhotCountPerEvent.Fill(photonCount);
            }
            std::cout << "Event " << evNr << ": " << photonCount << " photons" << std::endl;
            currentEvent = evNr;
            photonCount = 0; //reset count for the new event


        }


    }

    outputFile->cd();
    hPhotCountPerEvent.Write();
    std::cout << "Entries in histogram: " << hPhotCountPerEvent.GetEntries() << std::endl;
}
