#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "SteppingAction.hh"
#include "EventAction.hh"

#include "G4GeneralParticleSource.hh"
#include "G4ParticleDefinition.hh"
#include "G4SteppingManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleTypes.hh"
#include "G4SystemOfUnits.hh"
#include "G4TrackStatus.hh"
#include "G4RunManager.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4Step.hh"

#include "TH2F.h"

#include <fstream>
#include <iomanip>

using namespace std;	 

SteppingAction::SteppingAction(G4int *evN, G4String nameAdd) : G4UserSteppingAction(), fout(nullptr), tout(nullptr), foutName("Output_" + nameAdd + ".root")
{ 
  evNr = evN;
  counter = 0;
}

void SteppingAction::InitOutput()
{
  fout = new TFile(foutName.c_str(),"RECREATE");
  tout = new TTree("T","Stepping action Event Tree");
  tout->Branch("evNr", &eventNr, "evNr/I");
  tout->Branch("pType", &pType, "pType/I");
  tout->Branch("pName", &pName, "pName/I");
  tout->Branch("KE", &KE, "KE/D");
  tout->Branch("Edep", &Edep, "Edep/D");
  tout->Branch("postPosX", &postPosX, "postPosX/D");
  tout->Branch("postPosY", &postPosY, "postPosY/D");
  tout->Branch("postPosZ", &postPosZ, "postPosZ/D");
  tout->Branch("Det", &Det, "Det/I");
  tout->Branch("CopyNo", &CopyNo, "CopyNo/I");
  tout->Branch("Gtime", &Gtime, "Gtime/D");
  tout->Branch("momentumX", &momentumX, "momentumX/D");
  tout->Branch("momentumY", &momentumY, "momentumY/D");
  tout->Branch("momentumZ", &momentumZ, "momentumZ/D");
}

SteppingAction::~SteppingAction()
{
  if (fout && tout) {
    fout->cd();
    tout->Write();
    fout->Close();
    G4cout << "End of Stepping Action (file written)" << G4endl;
  } else {
    G4cerr << "Warning: Output ROOT file or tree was not initialized!" << G4endl;
  }
}

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
  if (fout == NULL) {
    InitOutput();
  }

  InitVar();
  eventNr=*evNr;

  if (currentEv != eventNr) {
    currentEv=eventNr;
  }

  const G4String currentPhysicalName = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
  G4StepPoint* thePrePoint = aStep->GetPreStepPoint();
  G4StepPoint* thePostPoint = aStep->GetPostStepPoint();
  G4TouchableHandle theTouchable = thePrePoint->GetTouchableHandle();

  const G4String processName = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  const G4String particleName = aStep->GetTrack()->GetDefinition()->GetParticleName();
  G4double EdepStep = aStep->GetTotalEnergyDeposit();

  G4ThreeVector momentumVec = aStep->GetTrack()->GetMomentum();

  G4bool fillTree =0;
  if (currentPhysicalName == "Physi_LaBr3" && particleName != "opticalphoton") {
    if (particleName == "gamma") {pType = 0;}
    if (particleName == "e-") {pType = 1;}
    if (particleName == "e+") {pType = 2;}

    if (processName == "compt") {pName = 0;}
    if (processName == "phot") {pName = 1;}
    if (processName == "conv") {pName = 2;}
    if (processName == "eIoni") {pName = 3;}
    if (processName == "hIoni") {pName = 4;}
    if (processName == "msc") {pName = 5;}
    if (processName == "Scintillation") {pName = 6;}
    if (processName == "Cerenkov") {pName = 7;}
    if (processName == "eBrem") {pName = 8;}
    if (processName == "Rayl") {pName = 9;}
    if (processName == "Transportation") {pName = 10;}

    postPosX = thePostPoint->GetPosition().getX();
    postPosY = thePostPoint->GetPosition().getY();
    postPosZ = thePostPoint->GetPosition().getZ();
    Det = 11;
    Edep = EdepStep;
    KE = aStep->GetTrack()->GetVertexKineticEnergy();
    CopyNo = theTouchable->GetCopyNumber();
    Gtime = thePostPoint->GetGlobalTime();
    momentumX = momentumVec.getX();
    momentumY = momentumVec.getY();
    momentumZ = momentumVec.getZ();
    fillTree=1;
  } else if (currentPhysicalName == "Physi_SiPM" && processName == "OpAbsorption") {
    pType = 10;
    pName = 20;
    postPosX = thePostPoint->GetPosition().getX();
    postPosY = thePostPoint->GetPosition().getY();
    postPosZ = thePostPoint->GetPosition().getZ();
    Det = 22;
    Edep = EdepStep;
    KE = aStep->GetTrack()->GetVertexKineticEnergy();
    CopyNo =  theTouchable->GetCopyNumber();
    Gtime = thePostPoint->GetGlobalTime();
    fillTree=1;
  } else
    fillTree=0;

  if (fillTree) {
    tout->Fill();
  }
  counter++;
}

void SteppingAction::InitVar()
{
  eventNr = -999;
  pType = -9;
  pName = -9;
  Edep = -999;
  CopyNo = -9;
  nAbsPhotons = -9;
  EdepInSiPM = -9;
  prePosX = -999;
  prePosY = -999;
  prePosZ = -999;
  postPosX = -999;
  postPosY = -999;
  postPosZ = -999;
  Gtime = -9;
  counter = 0;
  momentumX = -9;
  momentumY = -9;
  momentumZ = -9;
}
