//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************

#ifndef HistoManager_h
#define HistoManager_h 1

#

#include "Analysis.hh"

#include "globals.hh"

#include "G4Track.hh"
#include "G4Run.hh"

#include "TH1I.h"
#include "TFile.h"
#include "TTree.h"

enum HistoId {
  ElectronEnergyHist = 0,
  GammaDepositedEnergyHist,
  PhotonEnergyHist,
  GammaElectronEnergy,
  GammaPhotonEnergy,
  PhotonInSIPMCount
};

struct GammaData{
  int trackID;
  double eDep;
  double time;
  int creatorProcess;
  int interactionProcess;
  int endVolume;
  double posX;
  double posY;
  double posZ;

  ClassDef(GammaData, 6);
};

struct PhotonData{
  int trackID;
  int process;
  double posX;
  double posY;
  double posZ;
  int copyNo;

  ClassDef(PhotonData, 2);
};

struct VetoData{
  int trackID;
  int copyNo;
  double time;
  ClassDef(VetoData, 3);
};

class HistoManager
{
public:
  static HistoManager* GetPointer();
  ~HistoManager();

  void BeginOfRun(const G4Run* run);
  void EndOfRun();
  void BeginOfEvent(const G4Event* evt);
  void EndOfEvent(const G4Event* evt);
  void UserSteppingAction(const G4Step* step);

  void Initialize();
  void Close();
  void FillEHisto(HistoId histId, G4double value);


  void TrackingAction(const G4Track* track);
  void PostUserTrackingAction(const G4Track* track);

  G4int EncodeProcess(const G4String& processName);
  G4int EncodeVolume(const G4String& volumeName);
  void SetSeedAndTime(std::string str){
    seedAndTime = str;
  }

private:
  std::string seedAndTime;
  HistoManager();
  static HistoManager* fManager;

  double fGammaDepositedEnergy;
  double fGammaElectronEnergy;
  double fGammaPhotonEnergy;
  double fPhotonInSIPMCount;

  TH1I* fPhotonEndingVolume;
  TFile* fOutputFile;

  //Event tree variables
  TTree* fOutputTree;
  G4int fTreeEventID;
  G4double fTreeTotalEnergy;
  G4double fTreeBGOTotalEnergy;
  std::vector<struct GammaData> fTreeGammaTrack;
  std::vector<struct PhotonData> fTreePhotons;
  std::vector<struct VetoData> fTreeVetoNr;
};

#endif

