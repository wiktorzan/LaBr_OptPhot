#include "ParallelWorldTumor.hh"
#include "ICRP110PhantomMaterial_Female.hh"
#include "ICRP110PhantomMaterial_Male.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4RunManager.hh"

#include "G4Ellipsoid.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"



ParallelWorldTumor::ParallelWorldTumor(const G4String& worldName)
: G4VUserParallelWorld(worldName)
{
    fTumorMessenger = new TumorMessenger(this);

    fTumorPosition = G4ThreeVector(0.,  12.*cm, -13.*cm );
    fTumorSize = G4ThreeVector(1.*cm, 2.*cm, 1.*cm); // Default size of the tumor (halfaxis)
}

ParallelWorldTumor::~ParallelWorldTumor()
{;}

void ParallelWorldTumor::Construct()
{
    G4double A;  // atomic mass
    G4double Z;  // atomic number
    G4double d;  // density
   
    // General elements
   
    A = 1.01*g/mole;
    auto elH = new G4Element ("Hydrogen","H",Z = 1.,A);
  
    A = 12.011*g/mole;
    auto elC = new G4Element("Carbon","C",Z = 6.,A);  
  
    A = 14.01*g/mole;
    auto elN = new G4Element("Nitrogen","N",Z = 7.,A);
  
    A = 16.00*g/mole;
    auto elO = new G4Element("Oxygen","O",Z = 8.,A);
  
    A = 22.99*g/mole;
    auto elNa = new G4Element("Sodium","Na",Z = 11.,A);
  
    A = 30.974*g/mole;
    auto elP = new G4Element("Phosphorus","P",Z = 15.,A);
   
    A = 32.064*g/mole;
    auto elS = new G4Element("Sulfur","S",Z = 16.,A);
   
    A = 35.453*g/mole;
    auto elCl = new G4Element("Chlorine","Cl",Z = 17.,A);
   
    A = 39.098*g/mole;
    auto elK = new G4Element("Potassium","K",Z = 19.,A);
  

    //Brain -- copied from ICRP110PhantomMaterial_Female 
    //male and female material differs a little bit - here ignored
    d = 1.050 *g/cm3;
    G4Material* BrainMaterial = new G4Material("brain",d,9);
    BrainMaterial -> AddElement(elH,0.107);
    BrainMaterial -> AddElement(elC,0.144);
    BrainMaterial -> AddElement(elN,0.022);
    BrainMaterial -> AddElement(elO,0.713);
    BrainMaterial -> AddElement(elNa,0.002);
    BrainMaterial -> AddElement(elP,0.004);
    BrainMaterial -> AddElement(elS,0.002);
    BrainMaterial -> AddElement(elCl,0.003);
    BrainMaterial -> AddElement(elK,0.003);

    G4VPhysicalVolume *ghostWorld = GetWorld();
    G4LogicalVolume *worldLogical = ghostWorld->GetLogicalVolume();


    fTumorShape = new G4Ellipsoid("Tumor", fTumorSize.x(), fTumorSize.y(), fTumorSize.z());  
    G4LogicalVolume * lTumor = new G4LogicalVolume(fTumorShape, BrainMaterial, "TumorLogical", 0, 0, 0);
   
    fTumorPV = new G4PVPlacement(0, fTumorPosition, lTumor, "TumorPhysical", worldLogical, 0, 0);


    G4VisAttributes *TumorVisAttributes= new G4VisAttributes(G4Colour(0., 0.0, 1.0, 0.5)); 
    TumorVisAttributes->SetForceSolid(true); 
    lTumor->SetVisAttributes(TumorVisAttributes);

}

void ParallelWorldTumor::SetTumorPosition(const G4ThreeVector& position)
{
    G4cout << "Setting tumor position to: " << position << G4endl;
    fTumorPosition = position;

    if(fTumorPV) {
        fTumorPV->SetTranslation(fTumorPosition);
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
    } 
}

void ParallelWorldTumor::SetTumorSize(const G4ThreeVector& size)
{
    G4cout << "Setting tumor size to: " << size << G4endl;
    fTumorSize = size;

    if(fTumorShape) {
        fTumorShape->SetSemiAxis(fTumorSize.x(), fTumorSize.y(), fTumorSize.z());
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
    }
}