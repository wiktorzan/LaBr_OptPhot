#ifndef MyTrackingAction_h
#define MyTrackingAction_h

#include "G4UserTrackingAction.hh"
#include "G4Types.hh"
#include "TFile.h"
#include "TTree.h"


class TrackingAction : public G4UserTrackingAction
{
public:
    TrackingAction();
    ~TrackingAction() override;

    void PreUserTrackingAction(const G4Track* track) override;
    
    
private:
    TFile* ftracksFile = nullptr;
    TTree* ftracksTree = nullptr;

    G4int fCreatorProcess;
    G4double fPhotonEnergy;
    G4int fEventID;
    G4int fparticletype;
};

#endif