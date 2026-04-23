#include "RunAction.hh"
#include "Analysis.hh"

#include "HistoManager.hh"

#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "G4Run.hh"

#include <iomanip>

RunAction::RunAction() : G4UserRunAction()
{}

RunAction::~RunAction()
{
  HistoManager::GetPointer()->Close();
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
  HistoManager::GetPointer()->BeginOfRun(run);
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4cout << "#### Run  " << run->GetRunID() << " stop." << G4endl;
  HistoManager::GetPointer()->EndOfRun();
}
