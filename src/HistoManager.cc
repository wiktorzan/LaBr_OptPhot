#include "HistoManager.hh"
#include "G4AnalysisManager.hh" 

#include "G4SystemOfUnits.hh"

#include "G4Electron.hh"
#include "G4OpticalPhoton.hh"
#include "G4Gamma.hh"
#include "G4VProcess.hh"
#include "G4Event.hh"

HistoManager* HistoManager::fManager = nullptr;

ClassImp(GammaData);
ClassImp(PhotonData);
ClassImp(VetoData);



HistoManager* HistoManager::GetPointer()
{
  if (!fManager) {
    static HistoManager manager;
    fManager = &manager;
  }
  return fManager;
}

HistoManager::HistoManager()
{
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->OpenFile("histomanager_out.root");
  fOutputFile = new TFile("histomanager_out2.root", "RECREATE");
  Initialize();
}

HistoManager::~HistoManager()
{
}

void HistoManager::Initialize()
{
  fGammaDepositedEnergy = 0.;
  fGammaElectronEnergy = 0;
  fGammaPhotonEnergy = 0;
  fPhotonInSIPMCount = 0;


  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->CreateH1("ElectronEnergy", "Energy of electrons; E[keV]; Counts", 100, 0., 1100. ); //keV
  analysisManager->CreateH1("GammaDepositedEnergy", "Energy deposited by gammas; E[keV]; Counts", 100, 0., 1100. ); //keV
  analysisManager->CreateH1("PhotonEnergy", "Energy of photons; E[eV]; Counts", 100, 0., 10); //eV
  analysisManager->CreateH1("GammaElectronEnergy", "Energy of electrons created by gammas; E[keV]; Counts", 100, 0., 1100. ); //keV
  analysisManager->CreateH1("GammaPhotonEnergy", "Energy of photons created by gammas; E[eV]; Counts", 100, 0., 3000); //eV
  analysisManager->CreateH1("PhotonInSIPMCount", "Number of photons in SIPM; Count; Counts", 1000, 0., 20000); //count
  
  fPhotonEndingVolume = new TH1I("PhotonEndingVolume", "Ending volume of photons; Volume; Counts", 10, 0., 10);
  fPhotonEndingVolume->SetCanExtend(TH1::kAllAxes);



}


void HistoManager::Close()
{
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->CloseFile();
  fOutputFile->Close();
}

void HistoManager::FillEHisto(HistoId histId, G4double value)
{
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillH1(histId, value);
}

void HistoManager::BeginOfRun(const G4Run* run)
{
  G4cout << "HISTOMANAGER  ### Run started" << G4endl;
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Reset();

  fPhotonEndingVolume->Reset();
  std::string treeName = "EventTree" + std::to_string(run->GetRunID());
  fOutputTree = new TTree(treeName.c_str(), "Tree with event data");
  // fTreeGammaTrack = std::vector<struct GammaData>();
  // fTreePhotons = std::vector<struct PhotonData>();
  // fTreeVetoNr = std::vector<struct  VetoData>();

  fOutputTree->Branch("EventNr", &fTreeEventID, "EventNr/I");
  fOutputTree->Branch("TotalEnergy", &fTreeTotalEnergy, "TotalEnergy/D");
  fOutputTree->Branch("BGOTotalEnergy", &fTreeBGOTotalEnergy, "BGOTotalEnergy/D");
  fOutputTree->Branch("GammaTrack", &fTreeGammaTrack);
  fOutputTree->Branch("Photons", &fTreePhotons);
  fOutputTree->Branch("VetoNr", &fTreeVetoNr);
}

void HistoManager::EndOfRun()
{
  G4cout << "HISTOMANAGER  ### Run ended" << G4endl;
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  
  fOutputFile->cd();
  fPhotonEndingVolume->Write();  
  fOutputTree->Write();

  delete fOutputTree;
}

void HistoManager::BeginOfEvent(const G4Event* evt)
{
  fTreeEventID = evt->GetEventID();
  fTreeTotalEnergy = 0.;
  fTreeBGOTotalEnergy = 0.;
  fTreeGammaTrack.clear();
  fTreePhotons.clear();
  fTreeVetoNr.clear();
  // G4cout << "HISTOMANAGER  ### Event started" << G4endl;
}

void HistoManager::EndOfEvent(const G4Event* evt)
{
  // G4cout << "HISTOMANAGER  ### Event ended" << G4endl;
    FillEHisto(GammaDepositedEnergyHist, fGammaDepositedEnergy);
    FillEHisto(GammaElectronEnergy, fGammaElectronEnergy);
    FillEHisto(GammaPhotonEnergy, fGammaPhotonEnergy);
    FillEHisto(PhotonInSIPMCount, fPhotonInSIPMCount);
    fOutputTree->Fill();
    fGammaElectronEnergy = 0.;
    fGammaPhotonEnergy = 0.;
    fGammaDepositedEnergy = 0.;
    fPhotonInSIPMCount = 0.;
}

void HistoManager::TrackingAction(const G4Track* track)
{ 
  bool isElectron = track->GetDefinition() == G4Electron::ElectronDefinition();
  bool isPhoton = track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition();
  bool isGamma = track->GetDefinition() == G4Gamma::GammaDefinition();
  bool createdByGamma = track->GetParentID() == 1;
  bool isFirstStep = track->GetCurrentStepNumber() == 0;

  // if(isGamma && isFirstStep)
  // {
  //   double energy = track->GetKineticEnergy();
  //   struct GammaData gammaData;
  //   gammaData.trackID = track->GetTrackID();
  //   gammaData.energy = energy / keV;
  //   gammaData.creatorProcess = EncodeProcess(track->GetCreatorProcess() ? track->GetCreatorProcess()->GetProcessName() : "unknown");
  //   // TODO interactionProcess
  //   gammaData.posX = track->GetPosition().getX() / mm;
  //   gammaData.posY = track->GetPosition().getY() / mm;
  //   gammaData.posZ = track->GetPosition().getZ() / mm;
  //   fTreeGammaTrack.push_back(gammaData);
  // }


  if(createdByGamma && isFirstStep)
  {
    double energy = track->GetKineticEnergy();
    fGammaDepositedEnergy += energy /keV;

    if(isPhoton)
    {
      fGammaPhotonEnergy += energy / eV;
      FillEHisto(PhotonEnergyHist, energy /eV); 
    }

    if(isElectron)
    {
      // G4cout << "TrackID :" << track->GetTrackID() << " Particle: " << track->GetDefinition()->GetParticleName() 
      //        << " Energy: " << track->GetKineticEnergy() / keV << " keV" 
      //        << " Creator: " << (track->GetCreatorProcess() ? track->GetCreatorProcess()->GetProcessName() : "unknown") 
      //        << G4endl;
      // //parent particle 
      // G4cout << "ParentID :" << track->GetParentID() << G4endl;
      fGammaElectronEnergy += energy /keV;
      FillEHisto(ElectronEnergyHist, energy /keV);
    }

  }


}

void HistoManager::UserSteppingAction(const G4Step* step)
{
  bool isGamma = step->GetTrack()->GetDefinition() == G4Gamma::GammaDefinition();
  bool isOpticalPhoton = step->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition();
  bool isVeto = step->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "BGO";
  bool isLaBr3 = step->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "Physi_LaBr3";
  bool isBGO = step->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "BGO";

  if(isLaBr3 && !isOpticalPhoton)
  {
    fTreeTotalEnergy += step->GetTotalEnergyDeposit() / keV;
  }

  if(isBGO && !isOpticalPhoton)
  {
    fTreeBGOTotalEnergy += step->GetTotalEnergyDeposit() / keV;
  }

  if(isGamma)
  {
    auto track = step->GetTrack();

    double energy = step->GetTotalEnergyDeposit();
    struct GammaData gammaData;
    gammaData.trackID = track->GetTrackID();
    gammaData.eDep = energy / keV;
    gammaData.creatorProcess = EncodeProcess(track->GetCreatorProcess() ? track->GetCreatorProcess()->GetProcessName() : "unknown");
    gammaData.interactionProcess = EncodeProcess(step->GetPostStepPoint()->GetProcessDefinedStep() ? step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() : "unknown");
    gammaData.posX = step->GetPreStepPoint()->GetPosition().getX() / mm;
    gammaData.posY = step->GetPreStepPoint()->GetPosition().getY() / mm;
    gammaData.posZ = step->GetPreStepPoint()->GetPosition().getZ() / mm;
    gammaData.endVolume = step->GetPostStepPoint()->GetPhysicalVolume() ? EncodeVolume(step->GetPostStepPoint()->GetPhysicalVolume()->GetName()) : -3;
    fTreeGammaTrack.push_back(gammaData);
  }

  if(isGamma && isVeto && step->GetTotalEnergyDeposit() > 0)
  {
    auto trackID = step->GetTrack()->GetTrackID();
    auto copyNo = step->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();

    for(const auto& veto : fTreeVetoNr)
    {
      if(veto.trackID == trackID && veto.copyNo == copyNo)
      {
        return;
      }
    }
    struct VetoData vetoData;
    vetoData.trackID = step->GetTrack()->GetTrackID();
    vetoData.copyNo = step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(1);
    fTreeVetoNr.push_back(vetoData);
  }
}

void HistoManager::PostUserTrackingAction(const G4Track* track)
{
  bool isStopped = track->GetTrackStatus() == fStopAndKill;
  bool isPhoton = track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition();


  auto volumeName = track->GetVolume()->GetName();

  if(isStopped && isPhoton && volumeName == "Physi_SiPM")
  {
    struct PhotonData photonData;
    photonData.trackID = track->GetTrackID();
    photonData.process = EncodeProcess(track->GetCreatorProcess() ? track->GetCreatorProcess()->GetProcessName() : "unknown");
    photonData.posX = track->GetPosition().getX() / mm;
    photonData.posY = track->GetPosition().getY() / mm;
    photonData.posZ = track->GetPosition().getZ() / mm;
    photonData.copyNo = track->GetVolume()->GetCopyNo();
    fTreePhotons.push_back(photonData);
  }

  if( isStopped){
    if(isPhoton)
    {
      fPhotonEndingVolume->Fill(volumeName, 1);
      if(volumeName == "Physi_SiPM")
      {
        fPhotonInSIPMCount++;
      }
    }else{
      // G4cout << "Volume: " << volumeName
      // << " Particle: " << track->GetDefinition()->GetParticleName()
      // << " trackID: " << track->GetTrackID()
      // << " particleID: " << track->GetDefinition()->GetPDGEncoding()
      // << " process: " << (track->GetCreatorProcess() ? track->GetCreatorProcess()->GetProcessName() : "unknown")
      // << G4endl;
    }}
}

G4int HistoManager::EncodeProcess(const G4String& processName)
{
    if (processName == "compt") {return 0;}
    if (processName == "phot") {return 1;}
    if (processName == "conv") {return 2;}
    if (processName == "eIoni") {return 3;}
    if (processName == "hIoni") {return 4;}
    if (processName == "msc") {return 5;}
    if (processName == "Scintillation") {return 6;}
    if (processName == "Cerenkov") {return 7;}
    if (processName == "eBrem") {return 8;}
    if (processName == "Rayl") {return 9;}
    if (processName == "Transportation") {return 10;}
    if (processName == "unknown") {return 11;}
    return -1;
}

G4int HistoManager::EncodeVolume(const G4String& volumeName)
{
    if (volumeName == "Physi_LaBr3") {return 0;}
    if (volumeName == "reflectorface") {return 1;}
    if (volumeName == "GlassWindow") {return 2;}
    if (volumeName == "Physi_SiPM") {return 3;}
    if (volumeName == "BGO") {return 4;}
    if (volumeName == "World") {return 5;}
    if (volumeName == "BGOW") {return 6;}
    return -1;
}