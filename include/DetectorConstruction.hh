
#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4Material;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction();
  ~DetectorConstruction();

  G4VPhysicalVolume* Construct();
  void ConstructMaterials();
  void ConstructDet();
  
private:
  G4VPhysicalVolume* physiWorld;

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

  //Materials
  G4Material* fVacuum;
  G4Material* fTefMat;
  G4Material* fAluR;
  G4Material* fQuartz;
  G4Material* fOptGrease;
  G4Material* fOptMat;
  G4Material* fBGOMat;
  G4Material* fLaBr3;

  


};

#endif

