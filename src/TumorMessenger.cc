#include "TumorMessenger.hh"
#include "ParallelWorldTumor.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"




TumorMessenger::TumorMessenger(ParallelWorldTumor* tumorWorld)
    : fTumorWorld(tumorWorld)
{
    fTumorDir = new G4UIdirectory("/LaBr3/tumor/");
    fTumorDir->SetGuidance("Commands for tumor parallel world.");

    fSetTumorPositionCmd = new G4UIcmdWith3VectorAndUnit("/LaBr3/tumor/setPosition", this);
    fSetTumorPositionCmd->SetGuidance("Set the position of the tumor.");
    fSetTumorPositionCmd->SetParameterName("x", "y", "z", false);
    fSetTumorPositionCmd->SetUnitCategory("Length");
    fSetTumorPositionCmd->SetDefaultUnit("cm");

    fSetTumorSizeCmd = new G4UIcmdWith3VectorAndUnit("/LaBr3/tumor/setSize", this);
    fSetTumorSizeCmd->SetGuidance("Set the size of the tumor (half-axis).");
    fSetTumorSizeCmd->SetParameterName("x", "y", "z", false);
    fSetTumorSizeCmd->SetUnitCategory("Length");
    fSetTumorSizeCmd->SetDefaultUnit("cm");


}

TumorMessenger::~TumorMessenger()
{
    delete fSetTumorPositionCmd;
    delete fTumorDir;
}

void TumorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{    
    if (command == fSetTumorPositionCmd) {
        fTumorWorld->SetTumorPosition(
            fSetTumorPositionCmd->GetNew3VectorValue(newValue));
    } else if (command == fSetTumorSizeCmd) {
        fTumorWorld->SetTumorSize(
            fSetTumorSizeCmd->GetNew3VectorValue(newValue));
    } else {
        G4cout << "Unknown command: " << command->GetCommandName() << G4endl;
    }
}