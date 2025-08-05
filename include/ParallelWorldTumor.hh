#ifndef ParallelWorldTumor_hh
#define ParallelWorldTumor_hh 1

#include "globals.hh"
#include "G4Ellipsoid.hh"
#include "G4VUserParallelWorld.hh"
#include "TumorMessenger.hh"

class ParallelWorldTumor : public G4VUserParallelWorld
{
    public:
        ParallelWorldTumor(const G4String& worldName);
        virtual ~ParallelWorldTumor();

        virtual void Construct();

        void SetTumorPosition(const G4ThreeVector& position);
        void SetTumorSize(const G4ThreeVector& size);
    private:
        TumorMessenger* fTumorMessenger;

        G4ThreeVector fTumorPosition; 
        G4ThreeVector fTumorSize; 
        G4Ellipsoid * fTumorShape;
        G4VPhysicalVolume* fTumorPV = nullptr;
};

#endif