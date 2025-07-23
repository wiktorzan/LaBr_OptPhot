#include "DetectorConstruction.hh"

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
#include "G4LogicalSkinSurface.hh"

using namespace CLHEP;

DetectorConstruction::DetectorConstruction()
{}

DetectorConstruction::~DetectorConstruction()
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4double pressure = 3.e-18*pascal;
  G4double temperature = 2.73*kelvin;
  G4double density = 1.e-25*g/cm3;

  G4NistManager* man = G4NistManager::Instance();
  G4Material* vacuum = man->FindOrBuildMaterial("G4_Galactic");

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

  G4Material* AluR = new G4Material("AluRef", 13., 26.98*g/mole, 2.700*g/cm3);
  G4Material* MgO = new G4Material("MgO", 2.0*g/cm3, 2);
  MgO->AddElement(Mg, 1);
  MgO->AddElement(O, 1);

  G4Material* LaBr3 = new G4Material("LaBr3", 5.08*g/cm3, 4);
  LaBr3->AddElement(Br, 3);
  LaBr3->AddElement(La, 1);
  LaBr3->AddElement(Ce, 1);
  LaBr3->AddElement(Sr, 1);

  G4Material* Quartz = new G4Material("Quartz", 2.20*g/cm3, 2);
  Quartz->AddElement(Si, 1);
  Quartz->AddElement(O, 2);

  G4Material* Optmat = new G4Material("optgelmat", 1.06*g/cm3, 1);
  Optmat->AddElement(Si, 1);

  // Photocathode Material
  // (Bialkali K2CsSb,  Density=?, Thickness=25.*nm?)
  G4Material* K2CsSb = new G4Material("K2CsSb", 2.00*g/cm3, 3);
  K2CsSb->AddElement(K, 2);
  K2CsSb->AddElement(Cs, 1);
  K2CsSb->AddElement(Sb, 1);

  //Methyl Phenyl Siloxane or optical grease
  G4Material* Optgrease = new G4Material("Optical_grease", 1.12*g/cm3, 4);
  Optgrease->AddElement(C, 7);
  Optgrease->AddElement(H, 8);
  Optgrease->AddElement(O, 1);
  Optgrease->AddElement(Si, 1);

  G4NistManager* manager = G4NistManager::Instance();

  G4Material* BGOmat = manager->FindOrBuildMaterial("G4_BGO");
  G4Material* Tefmat = manager->FindOrBuildMaterial("G4_TEFLON");

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
  LaBr3->SetMaterialPropertiesTable(LaBr3MPT);

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
  BGOmat->SetMaterialPropertiesTable(BGO_MPT);

  G4double QuartzRefractionIndex[nEntries] = {1.462, 1.465, 1.47, 1.48}; //https://refractiveindex.info/?shelf=main&book=SiO2&page=Malitson
  G4double QuartzAbsorptionLength[nEntries] = {1E7*cm, 1E7*cm, 1E7*cm, 1E7*cm};

  G4MaterialPropertiesTable* QuartzMPT = new G4MaterialPropertiesTable();
  QuartzMPT->AddProperty("RINDEX", PhotonEnergy, QuartzRefractionIndex, nEntries);
  QuartzMPT->AddProperty("ABSLENGTH", PhotonEnergy, QuartzAbsorptionLength, nEntries);
  Quartz->SetMaterialPropertiesTable(QuartzMPT);

  G4double Optgrease_RI[nEntries] = {1.46, 1.46, 1.46, 1.46};
  G4double Optgrease_AL[nEntries] = {400.0*cm, 400.0*cm, 400.0*cm, 400.0*cm};

  G4MaterialPropertiesTable* Optgrease_MPT = new G4MaterialPropertiesTable();
  Optgrease_MPT->AddProperty("RINDEX", PhotonEnergy, Optgrease_RI, nEntries);
  Optgrease_MPT->AddProperty("ABSLENGTH", PhotonEnergy, Optgrease_AL, nEntries);
  Optgrease->SetMaterialPropertiesTable(Optgrease_MPT);

  G4double SiPM_RI[nEntries] = {4.32, 5.57, 6.5, 5.1};
  G4double SiPM_AL[nEntries] = {0.000001*cm, 0.000001*cm, 0.000001*cm, 0.000001*cm};

  G4MaterialPropertiesTable* SiPM_MPT = new G4MaterialPropertiesTable();
  SiPM_MPT->AddProperty("RINDEX", PhotonEnergy, SiPM_RI, nEntries);
  SiPM_MPT->AddProperty("ABSLENGTH", PhotonEnergy, SiPM_AL, nEntries);
  Optmat->SetMaterialPropertiesTable(SiPM_MPT);

  G4double vacRefractionIndex[nEntries] = {1.0, 1.0, 1.0, 1.0};

  G4MaterialPropertiesTable* vacMPT = new G4MaterialPropertiesTable();
  vacMPT->AddProperty("RINDEX",PhotonEnergy,vacRefractionIndex, nEntries);
  vacuum->SetMaterialPropertiesTable(vacMPT);

//------------------------------------------------------
// Detector geometry
//------------------------------------------------------
  WorldSize = 30.*cm;

  G4Box* solidWorld = new G4Box("World", WorldSize/2, WorldSize/2, WorldSize/2);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, vacuum, "World");
  G4VPhysicalVolume* physiWorld = new G4PVPlacement(0, G4ThreeVector(), "World", logicWorld, NULL, false, 0);

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
  G4LogicalVolume* lreflector_al = new G4LogicalVolume(reflector_al, Tefmat, "Reflector");
  G4VPhysicalVolume* physireflector_al = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, 0.*cm), "reflector", lreflector_al, physiWorld, false, 0);

  G4Tubs* reflector_alface = new G4Tubs("Reflectorface", 0.0*cm, Reflector_Rmax, (0.1*cm)/2, StartPhi, DeltaPhi);
  G4LogicalVolume* lreflector_alface = new G4LogicalVolume(reflector_alface, Tefmat, "Reflectorface");
  G4VPhysicalVolume* physireflector_alface = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -(Reflector_Z/2) + ((0.1*cm)/2)), "reflectorface",
                                 lreflector_alface, physiWorld, false, 0);

//Housing

  G4Tubs* housing_al = new G4Tubs("housing", Alhos_Rmin, Alhos_Rmax, Alhos_Z/2, StartPhi, DeltaPhi);
  G4LogicalVolume* lhousing_al = new G4LogicalVolume(housing_al, AluR, "lhousing");
  G4VPhysicalVolume* physihousing_al = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, 0.*cm), "physichousing", lhousing_al, physiWorld, false, 0);

  G4Tubs* housing_alface = new G4Tubs("housingface", 0.0*cm, Alhos_Rmax, (0.05*cm)/2, StartPhi, DeltaPhi);
  G4LogicalVolume* lhousing_alface = new G4LogicalVolume(housing_alface, AluR, "lhousingface");
  G4VPhysicalVolume* physihousing_alface = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -(Reflector_Z/2) + 0.1*cm + ((0.05*cm)/2)),
                                 "reflectorface", lreflector_alface, physiWorld, false, 0);

//LaBr3 crystal

  LaBr3->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
  G4Tubs* SLaBr3 = new G4Tubs("LaBr3", LaBr3Rmin, LaBr3Rmax, LaBr3Z/2, StartPhi, DeltaPhi);
  G4LogicalVolume* lLaBr3 = new G4LogicalVolume(SLaBr3, LaBr3, "lLaBr3");
  G4VPhysicalVolume* physiLaBr3  = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -(Reflector_Z/2) + 0.05*cm + LaBr3Z/2), "Physi_LaBr3",
                             lLaBr3, physiWorld, false, 0);

//Teflon

  G4Tubs* Teflon = new G4Tubs("Teflon", LaBr3Rmax, LaBr3Rmax + 0.05*cm, Reflector_Z/2, StartPhi, DeltaPhi);
  G4LogicalVolume* lteflon = new G4LogicalVolume(Teflon, Tefmat, "Teflon");
  G4VPhysicalVolume* physitef = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, 0.*cm), "teflon", lteflon, physiWorld, false, 0);

//Window

  G4Tubs* Glass_window = new G4Tubs("Glass_window", Glass_Rmin, Glass_Rmax, Glass_Z/2, StartPhi, DeltaPhi);
  G4LogicalVolume* lglassWindow = new G4LogicalVolume(Glass_window, Quartz, "Glass_window");
  G4VPhysicalVolume* physiglassWindow = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -(Reflector_Z/2) + 0.05*cm + LaBr3Z + (Glass_Z/2)),
                              "Glass_window", lglassWindow, physiWorld, false, 0);

//Optgel

  G4Tubs* Optgel = new G4Tubs("optgel", LaBr3Rmin, LaBr3Rmax, LaBr3Z/1000, StartPhi, DeltaPhi);
  G4LogicalVolume* lOptgel = new G4LogicalVolume(Optgel, Optgrease, "optgel");
  G4VPhysicalVolume* physiOptgel = new G4PVPlacement(0, G4ThreeVector(0.*cm, 0.*cm, -(Reflector_Z/2) + 0.05*cm + LaBr3Z + (Glass_Z) + (LaBr3Z/1000)),
                             "optgel", lOptgel, physiWorld, false, 0);

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
  G4LogicalVolume* lSipm = new G4LogicalVolume(Sipm, Optmat, "SiPM");
  G4VPhysicalVolume* physiSiPM[52];

  for(G4int i=0; i<52; i++) {
    new G4PVPlacement(0, G4ThreeVector(SP_X[i], SP_Y[i], -(Reflector_Z/2) + 0.05*cm + LaBr3Z + (Glass_Z) + (2*LaBr3Z/1000) + (SiPM_Z/2)),
              "Physi_SiPM", lSipm, physiWorld, true, i);
  }


//BGO Wrapping

  G4Box* BGO_barW = new G4Box("BGOW", BGOW_X/2, BGOW_Y/2, BGOW_Z/2);
  G4LogicalVolume* lBGOW = new G4LogicalVolume(BGO_barW,AluR,"BGOW");
  G4VPhysicalVolume* physiBGOW;

  for(G4int j=0;j<28;j++) {
    G4RotationMatrix* rotationMatrix = new G4RotationMatrix();
    rotationMatrix->rotateZ((j)*12.857*deg);
    physiBGOW = new G4PVPlacement(rotationMatrix, G4ThreeVector(std::sin((j)*12.857*deg)*31.25*mm, std::cos((j)*12.857*deg)*31.25*mm, 0.*cm),
                    "BGOW", lBGOW, physiWorld, false, j);
  }

//BGO

  G4Box* BGO_bar = new G4Box("BGO", BGO_X/2, BGO_Y/2, BGO_Z/2);
  G4LogicalVolume* lBGO = new G4LogicalVolume(BGO_bar, BGOmat, "BGO");
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
  BGOAL->SetType(dielectric_metal);
  BGOAL->SetModel(unified);
  BGOAL->SetFinish(groundfrontpainted);
  G4double reflectivity_metal[2] = {0.95, 0.95}; // np. 95% odbicia
  G4double efficiency_metal[2] = {0.0, 0.0};     // 0 = nie jest detektorem
  G4MaterialPropertiesTable* metalMPT = new G4MaterialPropertiesTable();
  metalMPT->AddProperty("REFLECTIVITY", energy, reflectivity_metal, 2);
  metalMPT->AddProperty("EFFICIENCY", energy, efficiency_metal, 2);
  BGOAL->SetMaterialPropertiesTable(metalMPT);
  new G4LogicalSkinSurface("BGO_Al", lBGO, BGOAL);

  // // BGOAL->SetModel(unified);
  // // BGOAL->SetFinish(groundfrontpainted);
  // G4LogicalBorderSurface* BGOALSurface = new G4LogicalBorderSurface("BGO_Al",physiBGO,physiBGOW,BGOAL);
  // BGOAL->SetMaterialPropertiesTable(SMPT);

//------------------------------------------------------
// visualization attributes
//------------------------------------------------------

  logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

  G4VisAttributes* Att1= new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  lLaBr3->SetVisAttributes(Att1);
  G4VisAttributes* Att2= new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
  lreflector_al->SetVisAttributes(Att2);
  lreflector_alface->SetVisAttributes(Att2);
  G4VisAttributes* Att3= new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
  lglassWindow->SetVisAttributes(Att3);
  G4VisAttributes* Att4= new G4VisAttributes(G4Colour(1.0, 0.0, 1.0));
  lSipm->SetVisAttributes(Att4);

  return physiWorld;
}
