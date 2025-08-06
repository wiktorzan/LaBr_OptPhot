#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction* detectorConstruction)
    : fDetectorConstruction(detectorConstruction)
{
    fDetectorDir = new G4UIdirectory("/LaBr3/det/");
    fDetectorDir->SetGuidance("Commands for detector construction.");

    fSetDetectorPositionCmd = new G4UIcmdWith3VectorAndUnit("/LaBr3/det/setPosition", this);
    fSetDetectorPositionCmd->SetGuidance("Set the position of the detector.");
    fSetDetectorPositionCmd->SetParameterName("x", "y", "z", false);
    fSetDetectorPositionCmd->SetUnitCategory("Length");
    fSetDetectorPositionCmd->SetDefaultUnit("cm");

    fSetDetectorRotationCmd = new G4UIcmdWith3VectorAndUnit("/LaBr3/det/setRotation", this);
    fSetDetectorRotationCmd->SetGuidance("Set the rotation of the detector.");
    fSetDetectorRotationCmd->SetParameterName("xaxis", "yaxis", "zaxis", false);
    fSetDetectorRotationCmd->SetUnitCategory("Angle");
    fSetDetectorRotationCmd->SetDefaultUnit("deg");

    fSetDetectorTargetCmd = new G4UIcmdWith3VectorAndUnit("/LaBr3/det/setTarget", this);
    fSetDetectorTargetCmd->SetGuidance("Point the detector towards a target position.");
    fSetDetectorTargetCmd->SetParameterName("x", "y", "z", false);
    fSetDetectorTargetCmd->SetUnitCategory("Length");
    fSetDetectorTargetCmd->SetDefaultUnit("cm");

}

DetectorMessenger::~DetectorMessenger()
{
    delete fSetDetectorPositionCmd;
    delete fSetDetectorRotationCmd;
    delete fDetectorDir;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == fSetDetectorPositionCmd) {
        fDetectorConstruction->SetDetectorPosition(
            fSetDetectorPositionCmd->GetNew3VectorValue(newValue));
    } else if (command == fSetDetectorRotationCmd) {
        fDetectorConstruction->SetDetectorRotation(
            fSetDetectorRotationCmd->GetNew3VectorValue(newValue));
    } else if (command == fSetDetectorTargetCmd) {
        fDetectorConstruction->PointDetectorTo(
            fSetDetectorTargetCmd->GetNew3VectorValue(newValue));
    } else {
        G4cout << "Unknown command: " << command->GetCommandName() << G4endl;
    }
}