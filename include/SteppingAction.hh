#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4String.hh"
#include "globals.hh"

#include "TGraph.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TH3I.h"

class EventAction;

class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction(G4int*);
  ~SteppingAction();

  void UserSteppingAction(const G4Step*);
  void InitVar();
  void InitOutput();

  void SetOutputFileName(G4String val) {foutName=val;}

  G4double  k_primary;
  TFile *fout;
  TTree *tout;
private:
  EventAction* eventAction;
  G4int currentEv;
  G4int *evNr;
   
  G4int eventNr;
  G4int pType;
  G4int pName;
  G4double KE;
  G4String foutName;
  G4int nAbsPhotons;
  G4int Det;
  G4double Gtime;
  G4double EdepInSiPM;
  G4double Edep;

  G4double prePosX;
  G4double prePosY;
  G4double prePosZ;

  G4double postPosX;
  G4double postPosY;
  G4double postPosZ;
  G4int CopyNo;
    
  G4int counter;
};

#endif
