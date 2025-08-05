#ifndef TumorMessenger_hh
#define TumorMessenger_hh

#include "G4UImessenger.hh"
#include "globals.hh"

class ParallelWorldTumor;
class G4UIdirectory;
class G4UIcmdWith3VectorAndUnit;


class TumorMessenger : public G4UImessenger
{
public:
    TumorMessenger(ParallelWorldTumor*);
    virtual ~TumorMessenger();

    void SetNewValue(G4UIcommand* command, G4String newValue);  

private:
    ParallelWorldTumor* fTumorWorld;

    G4UIdirectory* fTumorDir;  
    G4UIcmdWith3VectorAndUnit * fSetTumorPositionCmd;
    G4UIcmdWith3VectorAndUnit * fSetTumorSizeCmd;
};

#endif