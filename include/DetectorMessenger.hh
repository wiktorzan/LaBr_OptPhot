#ifndef DetectorMessenger_hh
#define DetectorMessenger_hh

#include "G4UImessenger.hh"
#include "globals.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWith3VectorAndUnit;

class DetectorMessenger : public G4UImessenger
{
public:
    DetectorMessenger(DetectorConstruction*);
    virtual ~DetectorMessenger();

    void SetNewValue(G4UIcommand* command, G4String newValue);
private:
    DetectorConstruction* fDetectorConstruction;

    G4UIdirectory* fDetectorDir;  
    G4UIcmdWith3VectorAndUnit * fSetDetectorPositionCmd;
    G4UIcmdWith3VectorAndUnit * fSetDetectorRotationCmd;
    G4UIcmdWith3VectorAndUnit * fSetDetectorTargetCmd; 

};


#endif