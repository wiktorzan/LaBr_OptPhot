#include "RunAction.hh"
#include "Analysis.hh"

#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "G4Run.hh"

#include <iomanip>

RunAction::RunAction() : G4UserRunAction()
{}

RunAction::~RunAction()
{}

void RunAction::BeginOfRunAction(const G4Run*)
{}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4cout << "#### Run  " << run->GetRunID() << " stop." << G4endl;
}
