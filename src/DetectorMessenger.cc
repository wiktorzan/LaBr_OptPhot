#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIdirectory.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction* detector) : fDetector(detector)
{
    fDetDirectory = new G4UIdirectory("/detector/");
    fDetDirectory->SetGuidance("Detector control");

    fBGO_YCmd = new G4UIcmdWithADoubleAndUnit("/detector/setBGO_Y", this);
    fBGO_YCmd->SetGuidance("Set the radial dimension of the BGO crystal.");
    fBGO_YCmd->SetParameterName("BGO_Y", false);
    fBGO_YCmd->SetUnitCategory("Length");
    fBGO_YCmd->SetDefaultUnit("mm");
    fBGO_YCmd->SetDefaultValue(6.0);

    fDetector->SetBGO_Y(20.0*mm); // Set the default value in the detector construction
}

DetectorMessenger::~DetectorMessenger()
{
    delete fBGO_YCmd;
    delete fDetDirectory;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == fBGO_YCmd) {
        G4double val = fBGO_YCmd->GetNewDoubleValue(newValue);
        fDetector->SetBGO_Y(val);

        G4RunManager::GetRunManager()->ReinitializeGeometry(true);
        G4UImanager* ui = G4UImanager::GetUIpointer();
        if(ui) {
            ui->ApplyCommand("/run/initialize");
            ui->ApplyCommand("/vis/scene/add/volume");
        }
        
    }
}