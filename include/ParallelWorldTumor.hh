#ifndef ParallelWorldTumor_hh
#define ParallelWorldTumor_hh 1

#include "globals.hh"
#include "G4VUserParallelWorld.hh"

class ParallelWorldTumor : public G4VUserParallelWorld
{
    public:
        ParallelWorldTumor(const G4String& worldName);
        virtual ~ParallelWorldTumor();

        virtual void Construct();
};

#endif