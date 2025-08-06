
#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "DetectorMessenger.hh"

class G4VPhysicalVolume;
class ICRP110PhantomMaterial_Female;
class ICRP110PhantomMaterial_Male;
class G4Material;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  explicit DetectorConstruction();
  ~DetectorConstruction();

  void SetDetectorPosition(const G4ThreeVector& pos);
  void SetDetectorRotation(const G4ThreeVector& rot);
  void PointDetectorTo(const G4ThreeVector& targetPos);

  G4VPhysicalVolume* Construct()override;
  void ConstructMaterials();
  void ConstructDet();
  void ConstructPhantom(const G4ThreeVector& posCentreVoxels);

  G4VPhysicalVolume* GetPhantomContainer() {return fPhantomContainer;}
  inline G4double GetVoxelHalfDimX(){return fVoxelHalfDimX;};
  inline G4double GetVoxelHalfDimY() {return fVoxelHalfDimY;};
  inline G4double GetVoxelHalfDimZ() {return fVoxelHalfDimZ;};

  
private:
  DetectorMessenger* fDetectorMessenger;

  //World parameters
  G4LogicalVolume* flogicWorld;
  G4VPhysicalVolume* fphysiWorld;

  G4double WorldSize;

  //Detector parameters
  G4VPhysicalVolume* fDetContainer;

  G4ThreeVector fDetectorPosition;
  G4RotationMatrix* fDetectorRotation;
  G4ThreeVector fDetectorTargetPosition;
  G4bool fDetectorTargetSet;


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

  //Phantom parameters
  G4String fSex;
  G4String fSection;

  ICRP110PhantomMaterial_Female* fMaterial_Female;
  ICRP110PhantomMaterial_Male* fMaterial_Male;

  void ReadPhantomData(const G4String& sex, const G4String& section);
  void ReadPhantomDataFile(const G4String& sex, const G4String& fname, G4int);

  G4int fNVoxelX;
  G4int fNVoxelY; 
  G4int fNVoxelZ;
  G4double fVoxelHalfDimX;
  G4double fVoxelHalfDimY;
  G4double fVoxelHalfDimZ;

  G4int fNoFiles;
  G4int fNVoxels;
  size_t* fMateIDs; // index of material of each voxel

  G4VPhysicalVolume* fPhantomContainer;

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

