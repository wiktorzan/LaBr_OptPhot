#include "DetectorConstruction.hh"
#include "ICRP110PhantomMaterial_Female.hh"
#include "ICRP110PhantomMaterial_Male.hh"
#include "ICRP110PhantomNestedParameterisation.hh"

#include "G4PVParameterised.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4MaterialTable.hh"
#include "G4VisAttributes.hh"
#include "G4ElementTable.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Material.hh"
#include "G4Polycone.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"

using namespace CLHEP;

DetectorConstruction::DetectorConstruction()
{
  fMaterial_Female = new ICRP110PhantomMaterial_Female();
  fMaterial_Male = new ICRP110PhantomMaterial_Male();
  fSex = "female"; // Female phantom is the default option
  fSection = "head"; // Head partial phantom is the default option
}

DetectorConstruction::~DetectorConstruction()
{
  delete fMaterial_Female;
  delete fMaterial_Male;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  ConstructMaterials();

  //World construction
  WorldSize = 100.*cm;

  G4Box* solidWorld = new G4Box("World", WorldSize/2, WorldSize/2, WorldSize/2);
  flogicWorld = new G4LogicalVolume(solidWorld, fVacuum, "World");
  fphysiWorld = new G4PVPlacement(0, G4ThreeVector(), "World", flogicWorld, NULL, false, 0);

  

  //Construct the detector
  ConstructDet();
  ConstructPhantom();


  flogicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());
  return fphysiWorld;
}

void DetectorConstruction::ConstructMaterials()
{
  G4double pressure = 3.e-18*pascal;
  G4double temperature = 2.73*kelvin;
  G4double density = 1.e-25*g/cm3;

  G4NistManager* man = G4NistManager::Instance();
  fVacuum = man->FindOrBuildMaterial("G4_Galactic");

  G4Element* H = new G4Element("Hydrogen", "H", 1., 1.0078*g/mole);
  G4Element* C = new G4Element("Carbon", "C", 6., 12.011*g/mole);
  G4Element* O = new G4Element("Oxygen", "O", 8., 15.999*g/mole);
  G4Element* Mg = new G4Element("Magnesium", "Mg", 12., 24.305*g/mole);
  G4Element* Si = new G4Element("Silicon", "Si", 14., 28.0855*g/mole);
  G4Element* Cl = new G4Element("Chlorine", "Cl", 17., 35.453*g/mole);
  G4Element* K = new G4Element("Potassium", "K", 19., 39.0983*g/mole);
  G4Element* Br = new G4Element("Bromine", "Br", 35., 79.904*g/mole);
  G4Element* Sr = new G4Element("Strontium", "Sr", 38., 87.62*g/mole);
  G4Element* Sb = new G4Element("Antimony", "Sb", 51., 121.76*g/mole);
  G4Element* Cs = new G4Element("Cesium", "Cs", 55., 132.905*g/mole);
  G4Element* La = new G4Element("Lanthanum", "La", 57., 138.905*g/mole);
  G4Element* Ce = new G4Element("Cerium", "Ce", 58., 140.116*g/mole);

  fAluR = new G4Material("AluRef", 13., 26.98*g/mole, 2.700*g/cm3);
  G4Material* MgO = new G4Material("MgO", 2.0*g/cm3, 2);
  MgO->AddElement(Mg, 1);
  MgO->AddElement(O, 1);

  fLaBr3 = new G4Material("LaBr3", 5.08*g/cm3, 4);
  fLaBr3->AddElement(Br, 3);
  fLaBr3->AddElement(La, 1);
  fLaBr3->AddElement(Ce, 1);
  fLaBr3->AddElement(Sr, 1);

  fQuartz = new G4Material("Quartz", 2.20*g/cm3, 2);
  fQuartz->AddElement(Si, 1);
  fQuartz->AddElement(O, 2);

  fOptMat = new G4Material("optgelmat", 1.06*g/cm3, 1);
  fOptMat->AddElement(Si, 1);

  // Photocathode Material
  // (Bialkali K2CsSb,  Density=?, Thickness=25.*nm?)
  G4Material* K2CsSb = new G4Material("K2CsSb", 2.00*g/cm3, 3);
  K2CsSb->AddElement(K, 2);
  K2CsSb->AddElement(Cs, 1);
  K2CsSb->AddElement(Sb, 1);

  //Methyl Phenyl Siloxane or optical grease
  fOptGrease = new G4Material("Optical_grease", 1.12*g/cm3, 4);
  fOptGrease->AddElement(C, 7);
  fOptGrease->AddElement(H, 8);
  fOptGrease->AddElement(O, 1);
  fOptGrease->AddElement(Si, 1);

  G4NistManager* manager = G4NistManager::Instance();

  fBGOMat = manager->FindOrBuildMaterial("G4_BGO");
  fTefMat = manager->FindOrBuildMaterial("G4_TEFLON");

  //------------------------------------------------------
  // Optical properties
  //------------------------------------------------------
  const G4int nEntries = 4;
  G4double PhotonEnergy[nEntries] = {2.49*eV, 2.75*eV, 3.44*eV, 3.76*eV};
  G4double MgORefractionIndex[nEntries] = {1.74, 1.75, 1.77, 1.78};
  G4double MgOAbsorptionLength[nEntries] = {8.0*cm, 7.5*cm, 6.25*cm, 5.0*cm}; // From abs coeff 0.2 -> 0.05 cm-1

  G4MaterialPropertiesTable* MgOMPT = new G4MaterialPropertiesTable();
  MgOMPT->AddProperty("RINDEX", PhotonEnergy, MgORefractionIndex, nEntries);
  MgOMPT->AddProperty("ABSLENGTH", PhotonEnergy, MgOAbsorptionLength, nEntries);
  MgO->SetMaterialPropertiesTable(MgOMPT);

  G4double LaBr3RefractionIndex[nEntries] = {2.1, 2.27, 2.3, 2.4}; // 10.1109/TNS.2012.2193597
  G4double LaBr3AbsorptionLength[nEntries] = {19.*cm, 16.*cm, 10.*cm, 0.1*cm}; // 10.1109/TNS.2012.2193597
  G4double ScintFast[nEntries] = {1.00, 1.00, 0.99, 0.97};

  G4MaterialPropertiesTable* LaBr3MPT = new G4MaterialPropertiesTable();
  LaBr3MPT->AddProperty("RINDEX", PhotonEnergy, LaBr3RefractionIndex, nEntries);
  LaBr3MPT->AddProperty("ABSLENGTH", PhotonEnergy, LaBr3AbsorptionLength, nEntries);
  LaBr3MPT->AddProperty("SCINTILLATIONCOMPONENT1", PhotonEnergy, ScintFast, nEntries);
  LaBr3MPT->AddConstProperty("SCINTILLATIONYIELD", 73./keV);
  LaBr3MPT->AddConstProperty("RESOLUTIONSCALE", 1.);
  LaBr3MPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 25.0*ns);
  LaBr3MPT->AddConstProperty("SCINTILLATIONYIELD1", 1.);
  fLaBr3->SetMaterialPropertiesTable(LaBr3MPT);

  G4double BGO_RI[nEntries] = {2.14, 2.17, 2.27, 2.33}; // 10.1364/AO.35.003562
  G4double BGO_AL[nEntries] = {10.*cm, 8.5*cm, 7.8*cm, 3.0*cm};
  G4double BGO_Fast[nEntries] = {1.00, 0.99, 0.97, 0.95};

  G4MaterialPropertiesTable* BGO_MPT = new G4MaterialPropertiesTable();
  BGO_MPT->AddProperty("RINDEX", PhotonEnergy, BGO_RI, nEntries);
  BGO_MPT->AddProperty("ABSLENGTH", PhotonEnergy, BGO_AL, nEntries);
  BGO_MPT->AddProperty("SCINTILLATIONCOMPONENT1", PhotonEnergy, BGO_Fast, nEntries);
  BGO_MPT->AddConstProperty("SCINTILLATIONYIELD", 10.0/keV);
  BGO_MPT->AddConstProperty("RESOLUTIONSCALE", 1.);
  BGO_MPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 20.*ns);
  BGO_MPT->AddConstProperty("SCINTILLATIONYIELD1", 1.);
  fBGOMat->SetMaterialPropertiesTable(BGO_MPT);

  G4double QuartzRefractionIndex[nEntries] = {1.462, 1.465, 1.47, 1.48}; //https://refractiveindex.info/?shelf=main&book=SiO2&page=Malitson
  G4double QuartzAbsorptionLength[nEntries] = {1E7*cm, 1E7*cm, 1E7*cm, 1E7*cm};

  G4MaterialPropertiesTable* QuartzMPT = new G4MaterialPropertiesTable();
  QuartzMPT->AddProperty("RINDEX", PhotonEnergy, QuartzRefractionIndex, nEntries);
  QuartzMPT->AddProperty("ABSLENGTH", PhotonEnergy, QuartzAbsorptionLength, nEntries);
  fQuartz->SetMaterialPropertiesTable(QuartzMPT);

  G4double Optgrease_RI[nEntries] = {1.46, 1.46, 1.46, 1.46};
  G4double Optgrease_AL[nEntries] = {400.0*cm, 400.0*cm, 400.0*cm, 400.0*cm};

  G4MaterialPropertiesTable* Optgrease_MPT = new G4MaterialPropertiesTable();
  Optgrease_MPT->AddProperty("RINDEX", PhotonEnergy, Optgrease_RI, nEntries);
  Optgrease_MPT->AddProperty("ABSLENGTH", PhotonEnergy, Optgrease_AL, nEntries);
  fOptGrease->SetMaterialPropertiesTable(Optgrease_MPT);

  G4double SiPM_RI[nEntries] = {4.32, 5.57, 6.5, 5.1};
  G4double SiPM_AL[nEntries] = {0.000001*cm, 0.000001*cm, 0.000001*cm, 0.000001*cm};

  G4MaterialPropertiesTable* SiPM_MPT = new G4MaterialPropertiesTable();
  SiPM_MPT->AddProperty("RINDEX", PhotonEnergy, SiPM_RI, nEntries);
  SiPM_MPT->AddProperty("ABSLENGTH", PhotonEnergy, SiPM_AL, nEntries);
  fOptMat->SetMaterialPropertiesTable(SiPM_MPT);

  G4double vacRefractionIndex[nEntries] = {1.0, 1.0, 1.0, 1.0};

  G4MaterialPropertiesTable* vacMPT = new G4MaterialPropertiesTable();
  vacMPT->AddProperty("RINDEX",PhotonEnergy,vacRefractionIndex, nEntries);
  fVacuum->SetMaterialPropertiesTable(vacMPT);
  
}

void DetectorConstruction::ConstructDet()
{
//------------------------------------------------------
// Detector geometry
//------------------------------------------------------

//Detector
LaBr3Rmin = 0.00*cm;
LaBr3Rmax = 2.54*cm;
LaBr3Z = 5.08*cm;

Glass_Rmin = LaBr3Rmin;
Glass_Rmax = LaBr3Rmax;
Glass_Z = 0.5*cm;

ReflectorThickness = 0.05*cm;
Reflector_Rmin = LaBr3Rmax;
Reflector_Rmax = Reflector_Rmin + 0.15*cm;
Reflector_Z = LaBr3Z + Glass_Z;

G4double Alhos_Rmin = Reflector_Rmax;
G4double Alhos_Rmax = Alhos_Rmin + 0.05*cm;
G4double Alhos_Z = Reflector_Z;

StartPhi = 0.*deg;
DeltaPhi = 360.*deg;

SiPM_X = 6.0*mm;
SiPM_Y = 6.0*mm;
SiPM_Z = 0.5*mm;

BGO_X = 6.0*mm;
BGO_Y = 6.0*mm;
BGO_Z = 60.0*mm;

G4double BGOW_X = 6.1*mm;
G4double BGOW_Y = 6.1*mm;
G4double BGOW_Z = 60.1*mm;

//Reflector

G4Tubs* reflector_al = new G4Tubs("Reflector", Reflector_Rmin, Reflector_Rmax,Reflector_Z/2, StartPhi, DeltaPhi);
G4LogicalVolume* lreflector_al = new G4LogicalVolume(reflector_al, fTefMat, "Reflector");
G4VPhysicalVolume* physireflector_al = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, 0.*cm), "reflector", lreflector_al, fphysiWorld, false, 0);

G4Tubs* reflector_alface = new G4Tubs("Reflectorface", 0.0*cm, Reflector_Rmax, (0.1*cm)/2, StartPhi, DeltaPhi);
G4LogicalVolume* lreflector_alface = new G4LogicalVolume(reflector_alface, fTefMat, "Reflectorface");
G4VPhysicalVolume* physireflector_alface = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -(Reflector_Z/2) + ((0.1*cm)/2)), "reflectorface",
                               lreflector_alface, fphysiWorld, false, 0);

//Housing

G4Tubs* housing_al = new G4Tubs("housing", Alhos_Rmin, Alhos_Rmax, Alhos_Z/2, StartPhi, DeltaPhi);
G4LogicalVolume* lhousing_al = new G4LogicalVolume(housing_al, fAluR, "lhousing");
G4VPhysicalVolume* physihousing_al = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, 0.*cm), "physichousing", lhousing_al, fphysiWorld, false, 0);

G4Tubs* housing_alface = new G4Tubs("housingface", 0.0*cm, Alhos_Rmax, (0.05*cm)/2, StartPhi, DeltaPhi);
G4LogicalVolume* lhousing_alface = new G4LogicalVolume(housing_alface, fAluR, "lhousingface");
G4VPhysicalVolume* physihousing_alface = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -(Reflector_Z/2) + 0.1*cm + ((0.05*cm)/2)),
                               "reflectorface", lreflector_alface, fphysiWorld, false, 0);

//LaBr3 crystal

fLaBr3->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
G4Tubs* SLaBr3 = new G4Tubs("LaBr3", LaBr3Rmin, LaBr3Rmax, LaBr3Z/2, StartPhi, DeltaPhi);
G4LogicalVolume* lLaBr3 = new G4LogicalVolume(SLaBr3, fLaBr3, "lLaBr3");
G4VPhysicalVolume* physiLaBr3  = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -(Reflector_Z/2) + 0.05*cm + LaBr3Z/2), "Physi_LaBr3",
                           lLaBr3, fphysiWorld, false, 0);

//Teflon

G4Tubs* Teflon = new G4Tubs("Teflon", LaBr3Rmax, LaBr3Rmax + 0.05*cm, Reflector_Z/2, StartPhi, DeltaPhi);
G4LogicalVolume* lteflon = new G4LogicalVolume(Teflon, fTefMat, "Teflon");
G4VPhysicalVolume* physitef = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, 0.*cm), "teflon", lteflon, fphysiWorld, false, 0);

//Window

G4Tubs* Glass_window = new G4Tubs("Glass_window", Glass_Rmin, Glass_Rmax, Glass_Z/2, StartPhi, DeltaPhi);
G4LogicalVolume* lglassWindow = new G4LogicalVolume(Glass_window, fQuartz, "Glass_window");
G4VPhysicalVolume* physiglassWindow = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -(Reflector_Z/2) + 0.05*cm + LaBr3Z + (Glass_Z/2)),
                            "Glass_window", lglassWindow, fphysiWorld, false, 0);

//Optgel

G4Tubs* Optgel = new G4Tubs("optgel", LaBr3Rmin, LaBr3Rmax, LaBr3Z/1000, StartPhi, DeltaPhi);
G4LogicalVolume* lOptgel = new G4LogicalVolume(Optgel, fOptGrease, "optgel");
G4VPhysicalVolume* physiOptgel = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -(Reflector_Z/2) + 0.05*cm + LaBr3Z + (Glass_Z) + (LaBr3Z/1000)),
                           "optgel", lOptgel, fphysiWorld, false, 0);

//SiPm positions

SP_X[0] = -9.48*mm;           SP_Y[0] = 22.12*mm;          SP_Z[0] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[1] = SP_X[0]+6.32*mm;    SP_Y[1] = SP_Y[0];           SP_Z[1] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[2] = SP_X[1]+6.32*mm;    SP_Y[2] = SP_Y[0];           SP_Z[2] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[3] = SP_X[2]+6.32*mm;    SP_Y[3] = SP_Y[0];           SP_Z[3] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[4] = -15.80*mm;          SP_Y[4] = SP_Y[0]-6.32*mm;   SP_Z[4] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[5] = SP_X[4]+6.32*mm;    SP_Y[5] = SP_Y[4];           SP_Z[5] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[6] = SP_X[5]+6.32*mm;    SP_Y[6] = SP_Y[4];           SP_Z[6] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[7] = SP_X[6]+6.32*mm;    SP_Y[7] = SP_Y[4];           SP_Z[7] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[8] = SP_X[7]+6.32*mm;    SP_Y[8] = SP_Y[4];           SP_Z[8] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[9] = SP_X[8]+6.32*mm;    SP_Y[9] = SP_Y[4];           SP_Z[9] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[10] = -22.12*mm;         SP_Y[10] = SP_Y[4]-6.32*mm;  SP_Z[10] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[11] = SP_X[10]+6.32*mm;  SP_Y[11] = SP_Y[10];         SP_Z[11] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[12] = SP_X[11]+6.32*mm;  SP_Y[12] = SP_Y[10];         SP_Z[12] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[13] = SP_X[12]+6.32*mm;  SP_Y[13] = SP_Y[10];         SP_Z[13] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[14] = SP_X[13]+6.32*mm;  SP_Y[14] = SP_Y[10];         SP_Z[14] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[15] = SP_X[14]+6.32*mm;  SP_Y[15] = SP_Y[10];         SP_Z[15] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[16] = SP_X[15]+6.32*mm;  SP_Y[16] = SP_Y[10];         SP_Z[16] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[17] = SP_X[16]+6.32*mm;  SP_Y[17] = SP_Y[10];         SP_Z[17] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[18] = -22.12*mm;         SP_Y[18] = SP_Y[10]-6.32*mm; SP_Z[18] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[19] = SP_X[18]+6.32*mm;  SP_Y[19] = SP_Y[18];         SP_Z[19] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[20] = SP_X[19]+6.32*mm;  SP_Y[20] = SP_Y[18];         SP_Z[20] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[21] = SP_X[20]+6.32*mm;  SP_Y[21] = SP_Y[18];         SP_Z[21] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[22] = SP_X[21]+6.32*mm;  SP_Y[22] = SP_Y[18];         SP_Z[22] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[23] = SP_X[22]+6.32*mm;  SP_Y[23] = SP_Y[18];         SP_Z[23] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[24] = SP_X[23]+6.32*mm;  SP_Y[24] = SP_Y[18];         SP_Z[24] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[25] = SP_X[24]+6.32*mm;  SP_Y[25] = SP_Y[18];         SP_Z[25] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[26] = -22.12*mm;         SP_Y[26] = SP_Y[18]-6.32*mm; SP_Z[26] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[27] = SP_X[26]+6.32*mm;  SP_Y[27] = SP_Y[26];         SP_Z[27] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[28] = SP_X[27]+6.32*mm;  SP_Y[28] = SP_Y[26];         SP_Z[28] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[29] = SP_X[28]+6.32*mm;  SP_Y[29] = SP_Y[26];         SP_Z[29] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[30] = SP_X[29]+6.32*mm;  SP_Y[30] = SP_Y[26];         SP_Z[30] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[31] = SP_X[30]+6.32*mm;  SP_Y[31] = SP_Y[26];         SP_Z[31] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[32] = SP_X[31]+6.32*mm;  SP_Y[32] = SP_Y[26];         SP_Z[32] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[33] = SP_X[32]+6.32*mm;  SP_Y[33] = SP_Y[26];         SP_Z[33] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[34] = -22.12*mm;         SP_Y[34] = SP_Y[26]-6.32*mm; SP_Z[34] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[35] = SP_X[34]+6.32*mm;  SP_Y[35] = SP_Y[34];         SP_Z[35] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[36] = SP_X[35]+6.32*mm;  SP_Y[36] = SP_Y[34];         SP_Z[36] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[37] = SP_X[36]+6.32*mm;  SP_Y[37] = SP_Y[34];         SP_Z[37] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[38] = SP_X[37]+6.32*mm;  SP_Y[38] = SP_Y[34];         SP_Z[38] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[39] = SP_X[38]+6.32*mm;  SP_Y[39] = SP_Y[34];         SP_Z[39] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[40] = SP_X[39]+6.32*mm;  SP_Y[40] = SP_Y[34];         SP_Z[40] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[41] = SP_X[40]+6.32*mm;  SP_Y[41] = SP_Y[34];         SP_Z[41] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[42] = -15.80*mm;         SP_Y[42] = SP_Y[34]-6.32*mm; SP_Z[42] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[43] = SP_X[42]+6.32*mm;  SP_Y[43] = SP_Y[42];         SP_Z[43] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[44] = SP_X[43]+6.32*mm;  SP_Y[44] = SP_Y[42];         SP_Z[44] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[45] = SP_X[44]+6.32*mm;  SP_Y[45] = SP_Y[42];         SP_Z[45] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[46] = SP_X[45]+6.32*mm;  SP_Y[46] = SP_Y[42];         SP_Z[46] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[47] = SP_X[46]+6.32*mm;  SP_Y[47] = SP_Y[42];         SP_Z[47] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[48] = -9.48*mm;          SP_Y[48] = SP_Y[42]-6.32*mm; SP_Z[48] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[49] = SP_X[48]+6.32*mm;  SP_Y[49] = SP_Y[48];         SP_Z[49] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[50] = SP_X[49]+6.32*mm;  SP_Y[50] = SP_Y[48];         SP_Z[50] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);
SP_X[51] = SP_X[50]+6.32*mm;  SP_Y[51] = SP_Y[48];         SP_Z[51] = -((LaBr3Z/2)+(2*LaBr3Z/1000)+SiPM_Z/2);

G4Box* Sipm = new G4Box("SiPM", SiPM_X/2, SiPM_Y/2, SiPM_Z/2);
G4LogicalVolume* lSipm = new G4LogicalVolume(Sipm, fOptMat, "SiPM");
G4VPhysicalVolume* physiSiPM[52];

for(G4int i=0; i<52; i++) {
  new G4PVPlacement(0, G4ThreeVector(SP_X[i], SP_Y[i], -(Reflector_Z/2) + 0.05*cm + LaBr3Z + (Glass_Z) + (2*LaBr3Z/1000) + (SiPM_Z/2)),
            "Physi_SiPM", lSipm, fphysiWorld, true, i);
}


//BGO Wrapping

G4Box* BGO_barW = new G4Box("BGOW", BGOW_X/2, BGOW_Y/2, BGOW_Z/2);
G4LogicalVolume* lBGOW = new G4LogicalVolume(BGO_barW,fAluR,"BGOW");
G4VPhysicalVolume* physiBGOW;

for(G4int j=0;j<28;j++) {
  G4RotationMatrix* rotationMatrix = new G4RotationMatrix();
  rotationMatrix->rotateZ((j)*12.857*deg);
  physiBGOW = new G4PVPlacement(rotationMatrix, G4ThreeVector(std::sin((j)*12.857*deg)*31.25*mm, std::cos((j)*12.857*deg)*31.25*mm, 0.*cm),
                  "BGOW", lBGOW, fphysiWorld, false, j);
}

//BGO

G4Box* BGO_bar = new G4Box("BGO", BGO_X/2, BGO_Y/2, BGO_Z/2);
G4LogicalVolume* lBGO = new G4LogicalVolume(BGO_bar, fBGOMat, "BGO");
G4VPhysicalVolume* physiBGO = new G4PVPlacement(0, G4ThreeVector(std::sin((0)*12.857*deg)*0.*mm, std::cos((0)*12.857*deg)*0.*mm, 0.*cm),
                        "BGO", lBGO, physiBGOW, false, 0);

//------------------------------------------------------
// Surfaces and boundary processes
//------------------------------------------------------

// Reflector - sintillator surface

G4OpticalSurface* OpRefCrySurface =  new G4OpticalSurface("LaBr3_tef");
OpRefCrySurface->SetType(dielectric_dielectric);
OpRefCrySurface->SetModel(unified);
OpRefCrySurface->SetFinish(groundfrontpainted);
G4LogicalBorderSurface* RefCrySurface = new G4LogicalBorderSurface("LaBr3_tef",physiLaBr3,physitef,OpRefCrySurface);

G4double energy[2] = {3.0995*eV, 4.9592*eV};
G4double rindex_teflon[2] = {1.3570, 1.4530};
G4double reflectivity[2] = {0.90, 0.90};

G4MaterialPropertiesTable* SMPT = new G4MaterialPropertiesTable();
SMPT->AddProperty("RINDEX", energy, rindex_teflon, 2);
SMPT->AddProperty("REFLECTIVITY", energy, reflectivity, 2);
OpRefCrySurface->SetMaterialPropertiesTable(SMPT);

G4OpticalSurface* faceLaBr = new G4OpticalSurface("LaBr3_Al");
faceLaBr->SetType(dielectric_metal);

G4LogicalBorderSurface* RefCryfaceSurface = new G4LogicalBorderSurface("RefCryfaceSurface",physiLaBr3,physireflector_alface,faceLaBr );
G4OpticalSurface* BGOAL =  new G4OpticalSurface("BGO_Al");
BGOAL->SetType(dielectric_dielectric);

//------------------------------------------------------
// visualization attributes
//------------------------------------------------------



G4VisAttributes* Att1= new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
lLaBr3->SetVisAttributes(Att1);
G4VisAttributes* Att2= new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
lreflector_al->SetVisAttributes(Att2);
lreflector_alface->SetVisAttributes(Att2);
G4VisAttributes* Att3= new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
lglassWindow->SetVisAttributes(Att3);
G4VisAttributes* Att4= new G4VisAttributes(G4Colour(1.0, 0.0, 1.0));
lSipm->SetVisAttributes(Att4);

}

void DetectorConstruction::ConstructPhantom()
{
  //------------------------------------------------------
  // Phantom geometry
  //------------------------------------------------------

  //Phantom is in air

  // Define Material Air
  G4double A;  // atomic mass
  G4double Z;  // atomic number
  G4double d;  // density

  A = 14.01*g/mole;
  auto elN = new G4Element("Nitrogen","N",Z = 7.,A);
  A = 16.00*g/mole;
  auto elO = new G4Element("Oxygen","O",Z = 8.,A);

  d = 0.001 *g/cm3;
  auto matAir = new G4Material("Air",d,2);
  matAir -> AddElement(elN,0.8);
  matAir -> AddElement(elO,0.2); 

  std::vector<G4Material*> pMaterials;

  if(fSex == "female"){

    fMaterial_Female -> DefineMaterials();
  //----- Store materials in a vector
      pMaterials.push_back(matAir); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("teeth")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("bone"));
      pMaterials.push_back(fMaterial_Female -> GetMaterial("humeri_upper")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("humeri_lower")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("arm_lower")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("hand")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("clavicle")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("cranium")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("femora_upper")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("femora_lower")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("leg_lower")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("foot")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("mandible")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("pelvis")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("ribs")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("scapulae")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("spine_cervical")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("spine_thoratic")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("spine_lumbar")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("sacrum")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("sternum")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("hf_upper")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("hf_lower")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("med_lowerarm")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("med_lowerleg")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("cartilage")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("skin")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("blood")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("muscle")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("liver")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("pancreas")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("brain"));
      pMaterials.push_back(fMaterial_Female -> GetMaterial("heart")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("eye")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("kidney")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("stomach")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("intestine_sml")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("intestine_lrg")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("spleen")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("thyroid")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("bladder")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("ovaries_testes")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("adrenals")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("oesophagus")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("misc")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("uterus_prostate"));
      pMaterials.push_back(fMaterial_Female -> GetMaterial("lymph")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("breast_glandular")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("breast_adipose")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("lung")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("gastro_content")); 
      pMaterials.push_back(fMaterial_Female -> GetMaterial("urine"));     
  }
  else if (fSex == "male"){
  // MATT do the same here
      fMaterial_Male -> DefineMaterials();

  //----- Store materials in a vector
      pMaterials.push_back(matAir); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("teeth")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("bone"));
      pMaterials.push_back(fMaterial_Male -> GetMaterial("humeri_upper")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("humeri_lower")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("arm_lower")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("hand")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("clavicle")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("cranium")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("femora_upper")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("femora_lower")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("leg_lower")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("foot")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("mandible")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("pelvis")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("ribs")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("scapulae")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("spine_cervical")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("spine_thoratic")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("spine_lumbar")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("sacrum")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("sternum")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("hf_upper")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("hf_lower")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("med_lowerarm")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("med_lowerleg")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("cartilage")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("skin")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("blood")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("muscle")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("liver")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("pancreas")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("brain"));
      pMaterials.push_back(fMaterial_Male -> GetMaterial("heart")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("eye")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("kidney")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("stomach")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("intestine_sml")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("intestine_lrg")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("spleen")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("thyroid")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("bladder")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("ovaries_testes")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("adrenals")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("oesophagus")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("misc")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("uterus_prostate"));
      pMaterials.push_back(fMaterial_Male -> GetMaterial("lymph")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("breast_glandular")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("breast_adipose")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("lung")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("gastro_content")); 
      pMaterials.push_back(fMaterial_Male -> GetMaterial("urine")); 
  
  }
    
  ReadPhantomData(fSex, fSection);
  G4cout << "Number of X,Y,Z voxels = " << fNVoxelX << ", " << fNVoxelY << ", " << fNVoxelZ << G4endl;

//----- Define the volume that contains all the voxels
  G4Box* fContainer_solid = new G4Box("phantomContainer",fNVoxelX*fVoxelHalfDimX*mm,
    fNVoxelY*fVoxelHalfDimY*mm,
    fNVoxelZ*fVoxelHalfDimZ*mm);

  auto fContainer_logic = new G4LogicalVolume( fContainer_solid,
                                  matAir,
                                  "phantomContainer",
                                    nullptr, nullptr, nullptr);                                                        

  // fMaxX = fNVoxelX*fVoxelHalfDimX*mm; // Max X along X axis of the voxelised geometry 
  // fMaxY = fNVoxelY*fVoxelHalfDimY*mm; // Max Y
  // fMaxZ = fNVoxelZ*fVoxelHalfDimZ*mm; // Max Z

  // fMinX = -fNVoxelX*fVoxelHalfDimX*mm;// Min X 
  // fMinY = -fNVoxelY*fVoxelHalfDimY*mm;// Min Y
  // fMinZ = -fNVoxelZ*fVoxelHalfDimZ*mm;// Min Z

  // G4ThreeVector posCentreVoxels((fMinX+fMaxX)/2.,(fMinY+fMaxY)/2.,(fMinZ+fMaxZ)/2.);
  G4ThreeVector posCentreVoxels(0., 0., -20.*cm); // Center of the phantom container

  G4cout << " placing voxel container volume at " << posCentreVoxels << G4endl;

  fPhantomContainer
  = new G4PVPlacement(nullptr,                     // rotation
                      posCentreVoxels,
                      fContainer_logic,     // The logic volume
                      "phantomContainer",  // Name
                      flogicWorld,         // Mother
                      false,            // No op. bool.
                      1);              // Copy number
  
  fContainer_logic -> SetVisAttributes(new G4VisAttributes(G4Colour(1.,0.,0.,0.5)));

// Define the voxelised phantom here
// Replication of air Phantom Volume.

//--- Slice the phantom along Y axis
  G4String yRepName("RepY");
  G4VSolid* solYRep = new G4Box(yRepName,fNVoxelX*fVoxelHalfDimX,
                                fVoxelHalfDimY, fNVoxelZ*fVoxelHalfDimZ);
  auto logYRep = new G4LogicalVolume(solYRep,matAir,yRepName);
  new G4PVReplica(yRepName,logYRep,fContainer_logic,kYAxis, fNVoxelY,fVoxelHalfDimY*2.);

  logYRep -> SetVisAttributes(new G4VisAttributes(G4VisAttributes::GetInvisible()));   

//--- Slice the phantom along X axis 
  G4String xRepName("RepX");
  G4VSolid* solXRep = new G4Box(xRepName,fVoxelHalfDimX,fVoxelHalfDimY,
                                fNVoxelZ*fVoxelHalfDimZ);
  auto logXRep = new G4LogicalVolume(solXRep,matAir,xRepName);
  new G4PVReplica(xRepName,logXRep,logYRep,kXAxis,fNVoxelX,fVoxelHalfDimX*2.);

  logXRep -> SetVisAttributes(new G4VisAttributes(G4VisAttributes::GetInvisible()));

//----- Voxel solid and logical volumes
//--- Slice along Z axis 
  G4VSolid* solidVoxel = new G4Box("phantom",fVoxelHalfDimX, fVoxelHalfDimY,fVoxelHalfDimZ);
  auto logicVoxel = new G4LogicalVolume(solidVoxel,matAir,"phantom");

  logicVoxel -> SetVisAttributes(new G4VisAttributes(G4VisAttributes::GetInvisible()));

  // Parameterisation to define the material of each voxel
  G4ThreeVector halfVoxelSize(fVoxelHalfDimX,fVoxelHalfDimY,fVoxelHalfDimZ);
    
  auto param =  new ICRP110PhantomNestedParameterisation(halfVoxelSize, pMaterials);

  new G4PVParameterised("phantom",    // their name
    logicVoxel, // their logical volume
    logXRep,      // Mother logical volume
    kZAxis,       // Are placed along this axis
    fNVoxelZ,      // Number of cells
    param);       // Parameterisation

  param -> SetMaterialIndices(fMateIDs); // fMateIDs is  the vector with Material ID associated to each voxel, from ASCII input data files.
  param -> SetNoVoxel(fNVoxelX,fNVoxelY,fNVoxelZ);
}

void DetectorConstruction::ReadPhantomData(const G4String& sex, const G4String& section)
{

  // This method reads the information of ICRPdata/FemaleData.dat or
  // ICRPdata/MaleData.data depending on the sex of the chosen phantom

  fSex = sex;
  fSection = section;
  
  G4String dataFile;
  
      if (fSex == "female")
      {
          if (fSection == "head")
          {
            dataFile = "ICRPdata/FemaleHead.dat";
          }
          else if (fSection == "trunk")
          {
            dataFile = "ICRPdata/FemaleTrunk.dat";
          }
          else if (fSection == "full")
          {
            dataFile = "ICRPdata/FemaleData.dat"; 
          }
      }
      if (fSex == "male")
      {
          if (fSection == "head")
          {
            dataFile = "ICRPdata/MaleHead.dat";
          }
          else if (fSection == "trunk")
          {
            dataFile = "ICRPdata/MaleTrunk.dat";
          }
          else if (fSection == "full")
          {
            dataFile = "ICRPdata/MaleData.dat"; 
          }
      }

          
  G4cout << "Data file " << dataFile << " is read by Detector Construction." << G4endl;
  
  // The data.dat file in directory/build/ICRPdata/ contains the information 
  // to build the phantoms. For more details look in the README file.
  
    //input file named finDF which consists of dataFile as a string object
    std::ifstream finDF(dataFile.c_str()); 
    
    
    G4String fname;
  
  if(finDF.good() != 1 ) //check that the file is good and working
    { 
    G4String descript = "Problem reading data file: "+dataFile;
    G4Exception(" HumanPhantomConstruction::ReadPhantomData"," ", 
      FatalException,descript);
    }

  finDF >> fNoFiles;
  G4cout << "Number of files = " << fNoFiles << G4endl;
  finDF >> fNVoxelX;      //Inputs number of X-Voxels
  finDF >> fNVoxelY;      //Y-Voxels
  fNVoxelZ = fNoFiles;    //Z-Voxels (equal to number of slice files built/read)
  finDF >> fVoxelHalfDimX;
  finDF >> fVoxelHalfDimY;
  finDF >> fVoxelHalfDimZ;
  G4cout << "Number of X,Y,Z voxels = " << fNVoxelX << ", " << fNVoxelY << ", " << fNVoxelZ <<G4endl;
  
  fNVoxels = fNVoxelX*fNVoxelY*fNVoxelZ; 
  G4cout << "Total Number of Voxels = " << fNVoxels << G4endl;
  
  G4int nMaterials;
  finDF >> nMaterials;
  G4String mateName;
  G4int nmate;

//-----Read materials and associate with material ID number------//
  
  for( G4int ii = 0; ii < nMaterials; ii++ ){
    finDF >> nmate;
    finDF >> mateName;
    
    // This allows to skip empty spaces and tabs in the string 
    if( mateName[0] == '"' && mateName[mateName.length()-1] == '"' ) 
    {
      mateName = mateName.substr(1,mateName.length()-2); 
    }
 
    // To uncomment for eventual debugging
    /* G4cout << "GmReadPhantomG4Geometry::ReadPhantomData reading nmate " 
           << ii << " = " << nmate 
           << " mate " << mateName << G4endl;*/
 
    if( ii != nmate ) {
    G4Exception("GmReadPhantomG4Geometry::ReadPhantomData",
                "Wrong argument",
                FatalErrorInArgument,
                "Material number should be in increasing order:wrong material number");
                }
      }
  
fMateIDs = new size_t[fNVoxels]; //Array with Material ID for each voxel

G4cout << "ICRP110PhantomConstruction::ReadPhantomDataFile is openining the following phantom files: " << G4endl;
  
for(G4int i = 0; i < fNoFiles; i++ )
  {
    finDF >> fname;
    ReadPhantomDataFile(fSex, fname, i); 
  }

finDF.close();
}

void DetectorConstruction::ReadPhantomDataFile(const G4String& sex, const G4String& fileName, G4int numberFile)
{

G4cout << fileName << G4endl;
         
fSex = sex;

G4String slice;

    if (fSex == "female")
    {
      slice = "ICRPdata/ICRP110_g4dat/AF/"+fileName;
    }
    if (fSex == "male")
    {
      slice = "ICRPdata/ICRP110_g4dat/AM/"+fileName;
    }  

    std::ifstream fin(slice.c_str(), std::ios_base::in);
  
    if( !fin.is_open() ) {
      G4Exception("HumanPhantomConstruction::ReadPhantomDataFile",
                  "",
                  FatalErrorInArgument,
                  G4String("File not found " + fileName ).c_str());
    }

  for( G4int iy = 0; iy < fNVoxelY; iy++ ) {
    for( G4int ix = 0; ix < fNVoxelX; ix++ ) {
      if (ix == 0 && iy == 0)
        {
          G4int dudX,dudY,dudZ;      
          fin >> dudX >> dudY >> dudZ ;
          // Dummy method to skip the first three lines of the files
          // which are not used here
        }
    
      G4int nnew = ix + (iy)*fNVoxelX + numberFile*fNVoxelX*fNVoxelY;
      G4int OrgID;
      fin >> OrgID; 

      G4int mateID_out;

      // The code below associates organ ID numbers (called here mateID) from ASCII slice
      // files with material ID numbers (called here mateID_out) as defined in ICRP110PhantomMaterials
      // Material and Organ IDs are associated as stated in AM_organs.dat and FM_organs.dat depending on
      // the sex of the phantom (male and female, respctively)

      if (OrgID==128)
      {
      mateID_out=1;
      }

      else if (OrgID==13 || OrgID==16 || OrgID==19 || OrgID==22 || OrgID==24 || OrgID==26 || OrgID==28 || OrgID==31 || OrgID==34 || OrgID==37 || OrgID==39 || OrgID==41 || OrgID==43 || OrgID==45 || OrgID==47 || OrgID==49 || OrgID==51 || OrgID==53 || OrgID==55)
      {
      mateID_out=2;
      }

      else if (OrgID==14)
      {
      mateID_out=3;
      }

      else if (OrgID==17)
      {
      mateID_out=4;
      }

      else if (OrgID==20)
      {
      mateID_out=5;
      }

      else if (OrgID==23)
      {
      mateID_out=6;
      }

      else if (OrgID==25)
      {
      mateID_out=7;
      }

      else if (OrgID==27)
      {
      mateID_out=8;
      }

      else if (OrgID==29)
      {
      mateID_out=9;
      }

      else if (OrgID==32)
      {
      mateID_out=10;
      }

      else if (OrgID==35)
      {
      mateID_out=11;
      }
      
      else if (OrgID==38)
      {
      mateID_out=12;
      }

      else if (OrgID==40)
      {
      mateID_out=13;
      }

      else if (OrgID==42)
      {
      mateID_out=14;
      }

      else if (OrgID==44)
      {	
      mateID_out=15;
      }

      else if (OrgID==46)
      {
      mateID_out=16;
      }

      else if (OrgID==48)
      {
      mateID_out=17;
      }

      else if (OrgID==50)
      {
      mateID_out=18;
      }

      else if (OrgID==52)
      {
      mateID_out=19;
      }

      else if (OrgID==54)
      {
      mateID_out=20;
      }

      else if (OrgID==56)
      {
      mateID_out=21;
      }

      else if (OrgID==15 || OrgID==30)
      {
      mateID_out=22;
      }

      else if (OrgID==18 || OrgID==33)
      {
      mateID_out=23;
      }

      else if (OrgID==21)
      {
      mateID_out=24;
      }

      else if (OrgID==36)
      {
      mateID_out=25;
      }

      else if (OrgID==57 || OrgID==58 || OrgID==59 || OrgID==60)	
      {
      mateID_out=26;
      }

      else if (OrgID==122 || OrgID==123 || OrgID==124 || OrgID==125 || OrgID==141 )		
      {
      mateID_out=27;
      }

      else if (OrgID==9 || OrgID==10 || OrgID==11 || OrgID==12 || OrgID==88 || OrgID==96 || OrgID==98)
      {
      mateID_out=28;
      }

      else if (OrgID==5 || OrgID==6 || OrgID==106 || OrgID==107 || OrgID==108 || OrgID==109 || OrgID==133)
      {
      mateID_out=29;
      }

      else if (OrgID==95)
      {
      mateID_out=30;
      }

      else if (OrgID==113)
      {
      mateID_out=31;
      }

      else if (OrgID==61)
      {
      mateID_out=32;
      }

      else if (OrgID==87)
      {
      mateID_out=33;
      }

      else if (OrgID==66 || OrgID==67 || OrgID==68 || OrgID==69)
      {
      mateID_out=34;
      }

      else if (OrgID==89 || OrgID==90 || OrgID==91 || OrgID==92 || OrgID==93 || OrgID==94)
      {
      mateID_out=35;
      }

      else if (OrgID==72)
      {
      mateID_out=36;
      }

      else if (OrgID==74)
      {
      mateID_out=37;
      }

      else if (OrgID==76 || OrgID==78 || OrgID==80 || OrgID==82 || OrgID==84 || OrgID==86)
      {
      mateID_out=38;
      }

      else if (OrgID==127)
      {
      mateID_out=39;
      }

      else if (OrgID==132)
      {
      mateID_out=40;
      }

      else if (OrgID==137)
      {
      mateID_out=41;
      }

      else if (OrgID==111 || OrgID==112 || OrgID==129 || OrgID==130)
      {
      mateID_out=42;
      }

      else if (OrgID==1 || OrgID==2)
      {
      mateID_out=43;
      }

      else if (OrgID==110)
      {
      mateID_out=44;
      }

      else if (OrgID==3 || OrgID==4 || OrgID==7 || OrgID==8 || OrgID==70 || OrgID==71 || OrgID==114 || OrgID==120 || OrgID==121 || OrgID==126 || OrgID==131 || OrgID==134 || OrgID==135 || OrgID == 136)
      {
      mateID_out=45;
      }

      else if (OrgID==115 || OrgID==139)
      {
      mateID_out=46;
      }

      else if (OrgID==100 || OrgID==101 || OrgID==102 || OrgID==103 || OrgID==104 || OrgID==105)
      {
      mateID_out=47;
      }

      else if (OrgID==63 || OrgID==65)
      {
      mateID_out=48;
      }

      else if (OrgID==62 || OrgID==64 || OrgID==116 || OrgID==117 || OrgID==118 || OrgID==119)
      {
      mateID_out=49;
      }

      else if (OrgID==97 || OrgID==99)
      {
      mateID_out=50;
      }

      else if (OrgID==73 || OrgID==75 || OrgID==77 || OrgID==79 || OrgID==81 || OrgID==83 || OrgID==85)
      {
      mateID_out=51;
      }

      else if (OrgID==138)
      {
      mateID_out=52;
      }

      else if (OrgID==0 || OrgID==140)
      {
      mateID_out=0;
      }

      else 
      {
      mateID_out=OrgID;
      }

      G4int nMaterials = 53;
      if( mateID_out < 0 || mateID_out >= nMaterials ) {
        G4Exception("GmReadPhantomG4Geometry::ReadPhantomData",
                    "Wrong index in phantom file",
                    FatalException,
                    G4String("It should be between 0 and 53"
                            ).c_str());
      
      //-------------Store Material IDs and position/reference number within phantom in vector---------------//
      }
      fMateIDs[nnew] = mateID_out;
    }
  }
}