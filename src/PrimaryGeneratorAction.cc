#include "PrimaryGeneratorAction.hh"

#include "G4GeneralParticleSource.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "G4Event.hh"
#include "globals.hh"
#include "G4ios.hh"

#include <fstream>
#include <iomanip>

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  particleGun = new G4GeneralParticleSource();
  particleGun->SetCurrentSourceIntensity(1);
  particleGun->SetParticlePosition(G4ThreeVector());
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete particleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  particleGun->GeneratePrimaryVertex(anEvent);
}
