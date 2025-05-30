#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#include "TProfile.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"

#include <fstream>
#include <iostream>

using namespace std;

class EventAction : public G4UserEventAction
{
public:
  EventAction(G4int* evN);
  ~EventAction();

  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  void addEdepTotal0(G4double Edep) {TotalEnergyDeposit[0]+= Edep;};
  void addEdepTotal1(G4double Edep) {TotalEnergyDeposit[1]+= Edep;};
  void addEdepTotal2(G4double Edep) {TotalEnergyDeposit[2]+= Edep;};
  void addEdepTotal3(G4double Edep) {TotalEnergyDeposit[3]+= Edep;};
  void addEdepTotal4(G4double Edep) {TotalEnergyDeposit[4]+= Edep;};
  void addEdepTotal5(G4double Edep) {TotalEnergyDeposit[5]+= Edep;};
  void addEdepTotal6(G4double Edep) {TotalEnergyDeposit[6]+= Edep;};
  void addEdepTotal7(G4double Edep) {TotalEnergyDeposit[7]+= Edep;};
  void addEdepTotal8(G4double Edep) {TotalEnergyDeposit[8]+= Edep;};
  void addEdepTotal9(G4double Edep) {TotalEnergyDeposit[9]+= Edep;};
  void addEdepTotal10(G4double Edep) {TotalEnergyDeposit[10]+= Edep;};
  void addEdepTotal11(G4double Edep) {TotalEnergyDeposit[11]+= Edep;};
  void addEdepTotal12(G4double Edep) {TotalEnergyDeposit[12]+= Edep;};
  void addEdepTotal13(G4double Edep) {TotalEnergyDeposit[13]+= Edep;};
  void addEdepTotal14(G4double Edep) {TotalEnergyDeposit[14]+= Edep;};
  void addEdepTotal15(G4double Edep) {TotalEnergyDeposit[15]+= Edep;};
  void addEdepTotal16(G4double Edep) {TotalEnergyDeposit[16]+= Edep;};
  void addEdepTotal17(G4double Edep) {TotalEnergyDeposit[17]+= Edep;};
  void addEdepTotal18(G4double Edep) {TotalEnergyDeposit[18]+= Edep;};
  void addEdepTotal19(G4double Edep) {TotalEnergyDeposit[19]+= Edep;};
  void addEdepTotal20(G4double Edep) {TotalEnergyDeposit[20]+= Edep;};
  void addEdepTotal21(G4double Edep) {TotalEnergyDeposit[21]+= Edep;};
  void addEdepTotal22(G4double Edep) {TotalEnergyDeposit[22]+= Edep;};
  void addEdepTotal23(G4double Edep) {TotalEnergyDeposit[23]+= Edep;};

  TTree* GetListMode() {return ListMode;}
  void SetBranchListMode() {
    ListMode->Branch("TotalEnergyDeposit1", &TotalEnergyDeposit[0], "TotalEnergyDeposit0/D");
    ListMode->Branch("TotalEnergyDeposit2", &TotalEnergyDeposit[1], "TotalEnergyDeposit1/D");
    ListMode->Branch("TotalEnergyDeposit3", &TotalEnergyDeposit[2], "TotalEnergyDeposit2/D");
    ListMode->Branch("TotalEnergyDeposit4", &TotalEnergyDeposit[3], "TotalEnergyDeposit3/D");
    ListMode->Branch("TotalEnergyDeposit5", &TotalEnergyDeposit[4], "TotalEnergyDeposit4/D");
    ListMode->Branch("TotalEnergyDeposit6", &TotalEnergyDeposit[5], "TotalEnergyDeposit5/D");
    ListMode->Branch("TotalEnergyDeposit7", &TotalEnergyDeposit[6], "TotalEnergyDeposit6/D");
    ListMode->Branch("TotalEnergyDeposit8", &TotalEnergyDeposit[7], "TotalEnergyDeposit7/D");
    ListMode->Branch("TotalEnergyDeposit9", &TotalEnergyDeposit[8], "TotalEnergyDeposit8/D");
    ListMode->Branch("TotalEnergyDeposit10",&TotalEnergyDeposit[9], "TotalEnergyDeposit9/D");
    ListMode->Branch("TotalEnergyDeposit11",&TotalEnergyDeposit[10],"TotalEnergyDeposit10/D");
    ListMode->Branch("TotalEnergyDeposit12",&TotalEnergyDeposit[11],"TotalEnergyDeposit11/D");
    ListMode->Branch("TotalEnergyDeposit13",&TotalEnergyDeposit[12],"TotalEnergyDeposit12/D");
    ListMode->Branch("TotalEnergyDeposit14",&TotalEnergyDeposit[13],"TotalEnergyDeposit13/D");
    ListMode->Branch("TotalEnergyDeposit15",&TotalEnergyDeposit[14],"TotalEnergyDeposit14/D");
    ListMode->Branch("TotalEnergyDeposit16",&TotalEnergyDeposit[15],"TotalEnergyDeposit15/D");
    ListMode->Branch("TotalEnergyDeposit17",&TotalEnergyDeposit[16],"TotalEnergyDeposit16/D");
    ListMode->Branch("TotalEnergyDeposit18",&TotalEnergyDeposit[17],"TotalEnergyDeposit17/D");
    ListMode->Branch("TotalEnergyDeposit19",&TotalEnergyDeposit[18],"TotalEnergyDeposit18/D");
    ListMode->Branch("TotalEnergyDeposit20",&TotalEnergyDeposit[19],"TotalEnergyDeposit19/D");
    ListMode->Branch("TotalEnergyDeposit21",&TotalEnergyDeposit[20],"TotalEnergyDeposit20/D");
    ListMode->Branch("TotalEnergyDeposit22",&TotalEnergyDeposit[21],"TotalEnergyDeposit21/D");
    ListMode->Branch("TotalEnergyDeposit23",&TotalEnergyDeposit[22],"TotalEnergyDeposit22/D");
    ListMode->Branch("TotalEnergyDeposit24",&TotalEnergyDeposit[23],"TotalEnergyDeposit23/D");
  } 
   
private:
  G4int PrintModulo;
  G4int *evNr;
  G4double TotalEnergyDeposit[24];
  TH1F* histogram1;
  TTree* ListMode;
  TFile* fileOut;
  G4double  k_primary;
};

#endif
