
#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction();
  ~DetectorConstruction();

  G4VPhysicalVolume* Construct();
  
private:
  G4double WorldSize;
  G4double LaBr3Rmin;
  G4double LaBr3Rmax;
  G4double LaBr3Z;

  G4double Glass_Rmin;
  G4double Glass_Rmax;
  G4double Glass_Z;

  G4double ReflectorThickness;
  G4double Reflector_Rmin;
  G4double Reflector_Rmax;
  G4double Reflector_Z;

  G4double StartPhi;
  G4double DeltaPhi;

  G4double SiPM_X;
  G4double SiPM_Y;
  G4double SiPM_Z;

  G4double BGO_X;
  G4double BGO_Y;
  G4double BGO_Z;
  G4double SP_X[52], SP_Y[52], SP_Z[52];
};

#endif

