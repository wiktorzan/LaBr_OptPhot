#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "RunAction.hh"
#include "Analysis.hh"

#include "G4GeneralParticleSource.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4Event.hh"
#include "globals.hh"

#include <iomanip>

using namespace std;
using namespace CLHEP;

EventAction::EventAction(G4int *evN) : G4UserEventAction(), PrintModulo(10000)
{
 evNr=evN;          
}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event* evt)
{
  G4int eventID = evt->GetEventID();
  *evNr = eventID;
  if (eventID %  PrintModulo == 0)
    G4cout << "\n---> Begin of Event: " << eventID << G4endl;
}

void EventAction::EndOfEventAction(const G4Event* evt)
{}
