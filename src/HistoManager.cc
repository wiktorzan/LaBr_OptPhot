#include "HistoManager.hh"
#include "G4AnalysisManager.hh" 

#include "G4SystemOfUnits.hh"

#include "G4Electron.hh"
#include "G4VProcess.hh"

HistoManager* HistoManager::fManager = nullptr;





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
  Initialize();
}

HistoManager::~HistoManager()
{
}

void HistoManager::Initialize()
{
  fGammaDepositedEnergy = 0.;


  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->CreateH1("ElectronEnergy", "Energy of electrons; E[keV]; Counts", 100, 0., 1100. ); //keV
  analysisManager->CreateH1("GammaDepositedEnergy", "Energy deposited by gammas; E[keV]; Counts", 100, 0., 1100. ); //keV
}


void HistoManager::Close()
{
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->CloseFile();
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
}

void HistoManager::EndOfRun()
{
  G4cout << "HISTOMANAGER  ### Run ended" << G4endl;
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
}

void HistoManager::BeginOfEvent()
{
  // G4cout << "HISTOMANAGER  ### Event started" << G4endl;
}

void HistoManager::EndOfEvent()
{
  // G4cout << "HISTOMANAGER  ### Event ended" << G4endl;
    FillEHisto(GammaDepositedEnergyHist, fGammaDepositedEnergy);
    fGammaDepositedEnergy = 0.;
}

void HistoManager::TrackingAction(const G4Track* track)
{ 
  bool isElectron = track->GetDefinition() == G4Electron::ElectronDefinition();
  bool createdByGamma = track->GetParentID() == 1;
  bool isFirstStep = track->GetCurrentStepNumber() == 0;

  if(createdByGamma && isFirstStep)
  {
    double energy = track->GetKineticEnergy() / keV;
    fGammaDepositedEnergy += energy;

    if(isElectron)
    {
      // G4cout << "TrackID :" << track->GetTrackID() << " Particle: " << track->GetDefinition()->GetParticleName() 
      //        << " Energy: " << track->GetKineticEnergy() / keV << " keV" 
      //        << " Creator: " << (track->GetCreatorProcess() ? track->GetCreatorProcess()->GetProcessName() : "unknown") 
      //        << G4endl;
      // //parent particle 
      // G4cout << "ParentID :" << track->GetParentID() << G4endl;
      FillEHisto(ElectronEnergyHist, energy);
    }

  }

}