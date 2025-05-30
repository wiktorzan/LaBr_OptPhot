#include "PhysicsList.hh"

#include "G4EmLivermorePolarizedPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmStandardPhysics.hh"
#include "G4OpticalParameters.hh"
#include "G4OpticalPhysics.hh"
#include "G4ParticleTypes.hh"
#include "G4SystemOfUnits.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
  defaultCutValue = 0.01*mm;

  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  auto opticalParams = G4OpticalParameters::Instance();
  opticalParams->SetScintTrackSecondariesFirst(true);
  RegisterPhysics(opticalPhysics);
  RegisterPhysics(new G4EmLivermorePolarizedPhysics());
}


PhysicsList::~PhysicsList()
{}

void PhysicsList::SetCuts()
{
  SetCutsWithDefault();
}
