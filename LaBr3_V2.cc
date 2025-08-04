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

#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "SteppingAction.hh"
#include "EventAction.hh"
#include "PhysicsList.hh"
#include "RunAction.hh"
#include "ICRP110PhantomVisAction.hh"
#include "ParallelWorldTumor.hh"
#include "G4ParallelWorldPhysics.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4UIterminal.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UItcsh.hh"

#include "Randomize.hh"

#include <sstream>
#include <unistd.h>
#include <chrono>

int main(int argc,char** argv) {
 
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }

  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  int mask = 01001010;
  using namespace std::chrono;
  system_clock::time_point now = system_clock::now();
  long seed = (unsigned int)(system_clock::to_time_t(now)) * 677 * ::getpid();
  G4Random::setTheSeed(seed);

  std::time_t tt = std::chrono::system_clock::to_time_t(now);
  std::tm tm = *std::gmtime(&tt); //GMT (UTC)
  std::stringstream ss;
  const std::string& format = "%m%d%H%M%S";
  ss << std::put_time( &tm, format.c_str() );

  std::string seedAndTime = ss.str() + "_" + std::to_string(seed);
  
  G4RunManager * runManager = new G4RunManager;

  auto userConstruction = new DetectorConstruction;
  G4String worldName = "TumorWorld";
  G4bool overwriteMaterial = true;
  userConstruction->RegisterParallelWorld(new ParallelWorldTumor(worldName));
  runManager->SetUserInitialization(userConstruction);

  G4VModularPhysicsList * physicsList = new PhysicsList;
  physicsList->RegisterPhysics(new G4ParallelWorldPhysics(worldName, overwriteMaterial));
  runManager->SetUserInitialization(physicsList);

  G4VisManager* visManager = nullptr;
  visManager = new G4VisExecutive;
  visManager->RegisterRunDurationUserVisAction
  ("phantom", new ICRP110PhantomVisAction(userConstruction));
  visManager->Initialize();
  
  G4int evNumber(0); 

  runManager->SetUserAction(new PrimaryGeneratorAction);
  runManager->SetUserAction(new RunAction);
  runManager->SetUserAction(new EventAction(&evNumber));
  runManager->SetUserAction(new SteppingAction(&evNumber, seedAndTime));
  
  runManager->Initialize(); 
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  
  if (ui) {

    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  } else {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
 
  delete visManager;
  delete runManager;

  return 0;
}
