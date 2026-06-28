#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIcmdWithADoubleAndUnit;


class DetectorMessenger : public G4UImessenger
{
  public:
    DetectorMessenger(DetectorConstruction*);
    ~DetectorMessenger();

    virtual void SetNewValue(G4UIcommand*, G4String);

  private:
    DetectorConstruction* fDetector;

    G4UIdirectory* fDetDirectory;
    G4UIcmdWithADoubleAndUnit* fBGO_YCmd = nullptr;
};




#endif