//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "G4PhysicalVolumeStore.hh"
// use of stepping action to set the accounting volume
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Polyhedra.hh"
#include "G4Polycone.hh"
#include "G4SubtractionSolid.hh"

#include "G4Torus.hh"
#include "G4UnionSolid.hh"

#include "G4String.hh"
#include "math.h"

#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

using namespace std;
//using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
const G4double DetectorConstruction::LambdaE = 2.0*TMath::Pi()*1.973269602e-16 * m * GeV;
//const G4double DetectorConstruction::LambdaE = 2.0*TMath::Pi()*1.973269602e-10 * mm * MeV;// in default Mev-mm units

const G4int numTPB =500;// 63;;

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction()
{
  //Debug bools
  checkOverlaps = true;
  GeDebug = false;//true;
  TpbDebug = false;
  LArDebug = false;
  CuDebug = false;//true;
  IronDebug = false;//true;
  VM2000Debug = false;//true;
  SdDebug = false;
  //***//
  //TPB//
  //***//    
  /*
  G4String pathFile = "External_data/tpbGhemann.root";
  TFile *tpbFile = new TFile(pathFile.data());
  if (!tpbFile ) 
    G4cout<<" DetectorConstruction ERROR:: file " << pathFile << " not found " << G4endl;
  else
    G4cout<<" DetectorConstruction INFO:: file " << pathFile << " opened " << G4endl;
  fTPBspec=NULL;
  tpbFile->GetObject("tpbGhemann",fTPBspec);
  if (!fTPBspec ) 
    G4cout<<" DetectorConstruction ERROR:: not graph tpbBhemann in file " << pathFile <<G4endl;
  else 
    G4cout<<" DetectorConstruction info tpbBhemann graph found " <<G4endl;
  */
  
  G4String pathFile = "External_data/TPB_Spec_Lehnert_thesis.root";
  TFile *tpbFile = new TFile(pathFile.data());
  if (!tpbFile ) 
    G4cout<<" DetectorConstruction ERROR:: file " << pathFile << " not found " << G4endl;
  else
    G4cout<<" DetectorConstruction INFO:: file " << pathFile << " opened " << G4endl;
  fTPBspec=NULL;
  tpbFile->GetObject("TPBSpec",fTPBspec);
  if (!fTPBspec ) 
    G4cout<<" DetectorConstruction ERROR:: no graph TPB_Spec_Lehnert_thesis.root in file " << pathFile <<G4endl;
  else 
    G4cout<<" DetectorConstruction info TPB_Spec_Lehnert_thesis.root graph found " <<G4endl;
  
  
  //Germanium Reflectivity
  pathFile = "External_data/Reflectivity_Ge.root";
  TFile *GeReflectivityFile = new TFile(pathFile.data());
  if(!GeReflectivityFile)
    G4cout<<" DetctorConstruction ERROR:: File "<<pathFile<<" NOT found "<<G4endl;
  else
    G4cout<<" DetectorConstruction INFO:: file "<< pathFile <<" opened "<<G4endl;

  fGeOpticalSpec=NULL;
  
  GeReflectivityFile->GetObject("Reflectivity_Ge",fGeOpticalSpec);//TODO Look at the name of the plot in the root file
  if(!fGeOpticalSpec)
    G4cout<<"DetectorConstruction ERROR:: no graph for GE!!!!!!!!!!"<<G4endl;
  else
    G4cout<<"DetectorConstruction INFO:: Germanim Reflections imported"<<G4endl;

  //Copper Reflectivity
   pathFile = "External_data/Reflectivity_Cu.root";
  TFile *CuReflectivityFile = new TFile(pathFile.data());
  if(!CuReflectivityFile)
    G4cout<<" DetctorConstruction ERROR:: File "<<pathFile<<" NOT found "<<G4endl;
  else
    G4cout<<" DetectorConstruction INFO:: file "<< pathFile <<" opened " <<G4endl;
  fCuOpticalSpec=NULL;
  
  CuReflectivityFile->GetObject("Reflectivity_Cu",fCuOpticalSpec);//TODO Look at the name of the plot in the root file
  if(!fCuOpticalSpec)
    G4cout<<"DetectorConstruction ERROR:: no graph for GE"<<G4endl;
  else
    G4cout<<"DetectorConstruction INFO:: Copper Reflections imported"<<G4endl;
   
  //Iron Reflectivity
  pathFile = "External_data/Iron_Reflectivity.root";
  TFile *IronReflectivityFile = new TFile(pathFile.data());
  if(!IronReflectivityFile)
    G4cout<<" DetctorConstruction ERROR:: File "<<pathFile<<" NOT found "<<G4endl;
  else
    G4cout<<" DetectorConstruction INFO:: file "<< pathFile <<" opened " <<G4endl;
  fIronOpticalSpec=NULL;
  
  IronReflectivityFile->GetObject("Iron_reflectivity",fIronOpticalSpec);//TODO Look at the name of the plot in the root file
  if(!fIronOpticalSpec)
    G4cout<<"DetectorConstruction ERROR:: no graph for Iron_Reflectivity"<<G4endl;
  else
    G4cout<<"DetectorConstruction INFO:: Iron Reflections imported"<<G4endl;


  // create directory 
  fDir = LegendAnalysis::Instance()->topHistDir()->mkdir("detec");
  fDir->cd();
  G4cout<<" DetectorAction working root directory  is  " << G4endl;  
  gDirectory->pwd();
  G4cout << " ... " << G4endl;
  G4double WLSHighE =LambdaE / (350*nm);//885.6013 2.4796*eV;//500 nm
  G4double WLSLowE = LambdaE / (650*nm);//100 nm
  G4double ArHighE = LambdaE / (115*nanometer);
  G4double ArLowE =  LambdaE / (300*nanometer);//650*nanometer);
  hWLSPhotonE = new TH1F("WLSPhotonE"," photon energy in WLS",100,WLSLowE,WLSHighE);
  hWLSPhotonWavelength = new TH1F("WLSPhotonWavelength"," photon Wavelength in WLS",100,LambdaE/WLSHighE,LambdaE/WLSLowE);
  hArPhotonE = new TH1F("ArPhotonE"," photon energy in LAr",100,ArLowE,ArHighE);
  hArPhotonWavelength = new TH1F("ArPhotonWavelength"," photon Wavelength in LAr",100,LambdaE/ArHighE,LambdaE/ArLowE);

  hWlsEmission   = new TH1F("WlsEmission"," WLS emission ",numTPB,0,numTPB);
  hWlsAbsorption = new TH1F("WlsAbsorption"," WLS absorption ",numTPB,0,numTPB);
  hWlsRefraction = new TH1F("WlsRefraction"," WLS refraction ",numTPB,0,numTPB);

  G4cout << " DetectorConstruction info lambda conversion factor =  " << LambdaE << " h_Planck*c_light = "  <<  h_Planck*c_light << " Mev-mm " << G4endl; 
 

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  nist = G4NistManager::Instance();
  innerVessel_FillMaterial = "ArgonLiquid";//"NitrogenGas";
  // used in this include 
	#include "LegendDetectorMaterials.icc"
  ArgonOpticalProperties();
  //mat_fill->GetMaterialPropertiesTable()->DumpTable();
  
  WLSOpticalProperties();
  
  GeOpticalProperties();

  CuOpticalProperties();
  
  //get ssteel from LegendDetectorMaterials.icc, then add iron optical properties to ssteel
  fSSteelMaterial = ssteel;
  IronOpticalProperties();
  fMaterialPMTGlass = quartz;
  PMTGlassOpticalProperties();
  //VM2000 defined in LegengDetectorMaterial.icc
  VM2000OpticalProperties();
  ////////////////////////////////////////////////////////////////////////////////////////
	//
  // World
  //
  ////////////////////////////////////////////////////////////////////////////////////////
  static const G4double inch = 2.54*cm;
  solidWorld = new G4Box("sol_World",50*m,50*m,50*m);
  logicalWorld = new G4LogicalVolume(solidWorld,mat_air,"log_World");
	logicalWorld->SetVisAttributes (G4VisAttributes::Invisible);
  physicalWorld = new G4PVPlacement(0,G4ThreeVector(),logicalWorld,"phy_World",0,false,0,checkOverlaps);

  G4double outerCanInnerRadius = (22.750 / 2.)*inch;
  G4double outerCanThickness = 0.25*inch;
  G4double outerCanOuterRadius = (outerCanInnerRadius+outerCanThickness);
  G4double outerCanLidThickness = 1.*inch;
  G4double outerCanHeight = ( (32.410*inch - 2.*outerCanLidThickness)/2. );
  
  solidOuterCan = new G4Tubs("sol_OuterCan",outerCanInnerRadius,outerCanOuterRadius,outerCanHeight,0, twopi);
  logicalOuterCan = new G4LogicalVolume(solidOuterCan,fSSteelMaterial,"log_OuterCan");
  logicalOuterCan->SetVisAttributes ( new G4VisAttributes(G4Colour(0.1,0.1,0.7,0.5) ) );
  physicalOuterCan = new G4PVPlacement(0,G4ThreeVector(),logicalOuterCan,"phys_OuterCan",logicalWorld,false,0,checkOverlaps);

  solidOuterCanLid = new G4Tubs("sol_OuterCanLid",0.,outerCanOuterRadius,outerCanLidThickness/2.,0,twopi);
  logicalOuterCanLid = new G4LogicalVolume(solidOuterCanLid,fSSteelMaterial,"log_OuterCanLid");
  logicalOuterCanLid->SetVisAttributes ( new G4VisAttributes(G4Colour(0.1,0.1,0.7,0.5) ) );
  physicalOuterCanLidTop = new G4PVPlacement(0,G4ThreeVector(0,0,outerCanHeight+outerCanLidThickness/2.),
      logicalOuterCanLid,"phys_OuterCanLidTop",logicalWorld,false,0,checkOverlaps);
  physicalOuterCanLidBottom = new G4PVPlacement(0,G4ThreeVector(0,0,-outerCanHeight-outerCanLidThickness/2.),
      logicalOuterCanLid,"phys_OuterCanLidBottom",logicalWorld,false,1,checkOverlaps);

  solidVacuum = new G4Tubs("sol_Vacuum",0.,outerCanInnerRadius,outerCanHeight,0,twopi);
  logicalVacuum = new G4LogicalVolume(solidVacuum,mat_vacuum,"log_vacuum");
  logicalVacuum->SetVisAttributes(G4VisAttributes::Invisible);
  physicalVacuum = new G4PVPlacement(0,G4ThreeVector(),logicalVacuum,"phs_vacuum",logicalWorld,false,0,checkOverlaps);

  G4double outerSourceVesselInnerRadius = (2.5/2.)*inch;
  G4double outerSourceVesselThickness = 0.25*inch;
  G4double outerSourceVesselOuterRadius = outerSourceVesselInnerRadius+outerSourceVesselThickness;
  G4double outerSourceVesselHeight = (6.0+7.025)*inch/2.;

  solidOuterSourceVessel = new G4Tubs("sol_OuterSourceVessel",outerSourceVesselInnerRadius,outerSourceVesselOuterRadius,outerSourceVesselHeight,0,twopi);
  logicalOuterSourceVessel = new G4LogicalVolume(solidOuterSourceVessel,fSSteelMaterial,"log_OuterSourceVessel");
  logicalOuterSourceVessel->SetVisAttributes ( new G4VisAttributes(G4Colour(0.1,0.1,0.7,0.5) ) );
  physicalOuterSourceVessel = new G4PVPlacement(0,G4ThreeVector(0,0,outerCanHeight-outerSourceVesselHeight),
      logicalOuterSourceVessel,"phys_OuterSourceVessel",logicalVacuum,false,0,checkOverlaps);

  solidOuterSourceAir = new G4Tubs("sol_OuterSourceAir",0,outerSourceVesselInnerRadius,outerSourceVesselHeight,0,twopi);
  logicalOuterSourceAir = new G4LogicalVolume(solidOuterSourceAir,mat_air,"log_OuterSourceAir");
  logicalOuterSourceAir->SetVisAttributes(new G4VisAttributes(G4Colour(0.1,0.1,0.3,0.5) ) );
  physicalOuterSourceAir = new G4PVPlacement(0,G4ThreeVector(0,0,outerCanHeight-outerSourceVesselHeight),
      logicalOuterSourceAir,"phys_OuterSourceAir",logicalVacuum,false,0,checkOverlaps);

  G4double innerCanInnerRadius = (19./2.)*inch;
  G4double innerCanThickness = 0.5*inch;
  G4double innerCanOuterRadius = innerCanInnerRadius+innerCanThickness;
  G4double innerCanHeight = (22.375/2.)*inch;
  G4double innerCanLidThickness = 1.0*inch;
  
  G4double innerSourceVesselInnerRadius = (5.0/2.)*inch;
  G4double innerSourceVesselThickness = 0.25*inch;
  G4double innerSourceVesselOutterRadius = innerSourceVesselInnerRadius + innerSourceVesselThickness;
  G4double innerSourceVesselHeight = (8./2.)*inch;

  G4double turnBuckleGap = 6.0*inch;
  G4double innerCanFullHeight = (23.875/2.)*inch;//This includes the top and bottom lid
  
  G4double innerSourceVessel_Z[6] = {innerSourceVesselHeight + innerCanLidThickness,
                                     innerSourceVesselHeight,
                                     innerSourceVesselHeight,
                                     -innerSourceVesselHeight+innerSourceVesselThickness,
                                     -innerSourceVesselHeight+innerSourceVesselThickness,
                                     -innerSourceVesselHeight};
  G4double innerSourceVessel_RMin[6] = {innerSourceVesselInnerRadius,
                                        innerSourceVesselInnerRadius,
                                        innerSourceVesselInnerRadius,
                                        innerSourceVesselInnerRadius,
                                        0,
                                        0};
  G4double innerSourceVessel_RMax[6] = {innerCanOuterRadius,
                                        innerCanOuterRadius,
                                        innerSourceVesselOutterRadius,
                                        innerSourceVesselOutterRadius,
                                        innerSourceVesselOutterRadius,
                                        innerSourceVesselOutterRadius};

  //might need G4Polycone class for inner vessel
  solidSourceInnerVessel = new G4Polycone("sol_SourceInnerVessel",0,twopi,6,innerSourceVessel_Z,innerSourceVessel_RMin,innerSourceVessel_RMax);
  logicalSourceInnerVessel = new G4LogicalVolume(solidSourceInnerVessel,fSSteelMaterial,"log_SourceInnerVessel");
  //had to palce by hand because it is not obvious where the center is because the object is not symetric
  physicalSourceInnerVessel = new G4PVPlacement(0,G4ThreeVector(0,0,outerCanHeight-turnBuckleGap-innerSourceVesselHeight-innerCanLidThickness/2-0.25*inch),
      logicalSourceInnerVessel,"phys_SourceInnerVessel",logicalVacuum,false,0,checkOverlaps);
  solidInnerCan = new G4Tubs("sol_InnerCan",innerCanInnerRadius,innerCanOuterRadius,innerCanHeight,0,twopi);
  logicalInnerCan = new G4LogicalVolume(solidInnerCan,fSSteelMaterial,"log_InnerCan");
  physicalInnerCan = new G4PVPlacement(0,G4ThreeVector(0,0,outerCanHeight-turnBuckleGap-innerCanFullHeight),
      logicalInnerCan,"phys_InnerCan",logicalVacuum,false,0,checkOverlaps);

  solidInnerCanBottomLid = new G4Tubs("sol_InnerCanBottomLid",0,innerCanOuterRadius,innerCanThickness/2.,0,twopi);
  logicalInnerCanBottomLid = new G4LogicalVolume(solidInnerCanBottomLid,fSSteelMaterial,"log_InnerCanBotomLid");
  physicalInnerCanBottomLid = new G4PVPlacement(0,G4ThreeVector(0,0,outerCanHeight-turnBuckleGap-2*innerCanFullHeight+innerCanThickness),
      logicalInnerCanBottomLid,"phys_InnerCanBottomLid",logicalVacuum,false,0,checkOverlaps);

  G4double Argon_Z   [4] = {innerCanHeight,
    innerCanHeight-2.*innerSourceVesselHeight,
    innerCanHeight-2.*innerSourceVesselHeight,
    -innerCanHeight};
  G4double Argon_RMin[4] = {innerSourceVesselOutterRadius,
    innerSourceVesselOutterRadius,
    0,
    0};
  G4double Argon_RMax[4] = {innerCanInnerRadius,
    innerCanInnerRadius,
    innerCanInnerRadius,
    innerCanInnerRadius};
  solidArgon = new G4Polycone("sol_Argon",0,twopi,4,Argon_Z,Argon_RMin,Argon_RMax);
  logicalArgon = new G4LogicalVolume(solidArgon,mat_fill,"log_Argon");
  logicalArgon->SetVisAttributes(new G4VisAttributes(G4Colour(0.1,0.9,0.1,0.5) ) );
  physicalArgon = new G4PVPlacement(0,G4ThreeVector(0,0,outerCanHeight-turnBuckleGap-innerCanFullHeight),
      logicalArgon,"phys_Argon",logicalVacuum,false,0,checkOverlaps);
  
  new G4LogicalBorderSurface("Phys_SourceInnerVessel_in",physicalSourceInnerVessel,physicalArgon,fIronOptSurface);
  new G4LogicalBorderSurface("Phys_SourceInnerVessel_out",physicalArgon,physicalSourceInnerVessel,fIronOptSurface);

  new G4LogicalBorderSurface("Phys_SourceInnerVessel_in",physicalInnerCan,physicalArgon,fIronOptSurface);
  new G4LogicalBorderSurface("Phys_SourceInnerVessel_out",physicalArgon,physicalInnerCan,fIronOptSurface);
  
  new G4LogicalBorderSurface("Phys_SourceInnerVessel_in",physicalInnerCanBottomLid,physicalArgon,fIronOptSurface);
  new G4LogicalBorderSurface("Phys_SourceInnerVessel_out",physicalArgon,physicalInnerCanBottomLid,fIronOptSurface);
  
  /////////////PMT coated in WLS/////////////
  G4double pmtRadius = (3.215/2.)*inch;
  WLSHalfThickness = 0.05*mm;
  G4double housingHalfThickness = 10*mm;  // half thickness
  G4double glassHalfThickness = 1*mm;  // half thickness
  G4double cathodeHalfThickness = 1*mm;
  G4double pmtZOffset = 2*(cathodeHalfThickness+glassHalfThickness+WLSHalfThickness)+housingHalfThickness;

  G4Tubs* PMTDiskTubs = new G4Tubs("PMTDiskTubs",0.,pmtRadius,housingHalfThickness,0,twopi);
  //Metal housing, Kovar is a Ni Co alloy
  G4Material* materialPMTHousing = G4Material::GetMaterial("Kovar");
  logicalPmtHousing = new G4LogicalVolume(PMTDiskTubs,materialPMTHousing,"logicalPmtHousing");  
  logicalPmtHousing->SetVisAttributes ( new G4VisAttributes(G4Colour::Green() ) );
  
  G4Tubs* PMTGlassTubs = new G4Tubs("PMTGlassTubs",0,pmtRadius,glassHalfThickness,0,twopi);
  //G4Material* materialPMTGlass = G4Material::GetMaterial("Quartz"); 
  logicalPmtGlass = new G4LogicalVolume(PMTGlassTubs,fMaterialPMTGlass,"logicalPmtGlass");            
  logicalPmtGlass->SetVisAttributes ( new G4VisAttributes(G4Colour::Yellow() ) );

  G4Tubs* PMTCathodeTubs = new G4Tubs("PMTCathodeTubs",0,pmtRadius,cathodeHalfThickness,0,twopi);
  logicalPmtCathode=  new G4LogicalVolume(PMTCathodeTubs,CathodeMetalAluminium,"logicalPmtCathode"); 
  logicalPmtCathode->SetVisAttributes ( new G4VisAttributes(G4Colour::Red() ) );
  
  G4Tubs* PMTWlsTubs = new G4Tubs("PMTWlsTubs",0,pmtRadius,WLSHalfThickness,0,twopi);
  logicalPMTWLS = new G4LogicalVolume(PMTWlsTubs,fTPB,"logicalPmtGlassWLS");   
  logicalPMTWLS->SetVisAttributes ( new G4VisAttributes(G4Colour::Blue() ) );
  //fPMTGlassOptSurface defined in LegendDetectorMaterials.icc
  new G4LogicalSkinSurface("PMTGlass_surf",logicalPmtGlass,fPMTGlassOptSurface);
  //G4ThreeVector pmtVec = G4ThreeVector(0,0,outerCanHeight-turnBuckleGap-2*innerCanFullHeight+innerCanThickness+pmtZOffset);
  G4ThreeVector pmtVec = G4ThreeVector(0,-3.*inch,-innerCanHeight+pmtZOffset+4.5*inch);
  PlacePMT(pmtVec,1,0);
  pmtVec = G4ThreeVector(0,3.*inch,-innerCanHeight+pmtZOffset+4.5*inch);
  PlacePMT(pmtVec,1,1);


  G4cout << "\t DetectorConstruction done constructing detector  " << G4endl;
  
  return physicalWorld;
}


void DetectorConstruction::PlacePMT(G4ThreeVector rhousing,double top_or_bot,int num)
{
  //PMT Housing 
  new G4PVPlacement(0,rhousing,logicalPmtHousing,"phys_PMTHousing_"+std::to_string(num),logicalArgon,false,num,checkOverlaps);
  // photocathode
  G4double zhalf =  top_or_bot*(dynamic_cast<G4Tubs*>(logicalPmtHousing->GetSolid())->GetZHalfLength() +  
    dynamic_cast<G4Tubs*>(logicalPmtCathode->GetSolid())->GetZHalfLength());
  G4ThreeVector rcathode = rhousing + G4ThreeVector(0,0,zhalf);
  new G4PVPlacement(0,rcathode,logicalPmtCathode,"phys_PMTCathode_"+std::to_string(num),logicalArgon,false,num,checkOverlaps);
  //PMT GLASS
  zhalf += top_or_bot*( dynamic_cast<G4Tubs*>(logicalPmtCathode->GetSolid())->GetZHalfLength()
      +dynamic_cast<G4Tubs*>(logicalPmtGlass->GetSolid())->GetZHalfLength());
  G4ThreeVector rglass = rhousing + G4ThreeVector(0,0,zhalf);
  new G4PVPlacement(0,rglass,logicalPmtGlass,"phys_PMTGlass_"+std::to_string(num),logicalArgon,false,num,checkOverlaps);
  //WLS PMT 
  zhalf += top_or_bot*( dynamic_cast<G4Tubs*>(logicalPmtGlass->GetSolid())->GetZHalfLength()
      +dynamic_cast<G4Tubs*>(logicalPMTWLS->GetSolid())->GetZHalfLength());
  G4ThreeVector rwls = rhousing + G4ThreeVector(0,0,zhalf);
  G4PVPlacement* phys_PMTWLS = new G4PVPlacement(0,rwls,logicalPMTWLS,"phys_PMTWLS_"+std::to_string(num),logicalArgon,false,num,checkOverlaps);
  // border between WLS and larPhysicsl crossing in both directions WLS is rough on both sides 
  G4double roughness = 0.5;
  G4OpticalSurface* WLSoptSurf = new G4OpticalSurface("WLS_rough_surf",glisur,ground,dielectric_dielectric,roughness);
  new G4LogicalBorderSurface("Phys_PMT_WLS_"+ std::to_string(num),physicalArgon,phys_PMTWLS,WLSoptSurf);
  new G4LogicalBorderSurface("Phys_PMT_WLS_"+ std::to_string(num),phys_PMTWLS,physicalArgon,WLSoptSurf);
}
///////////////////////////////////////////////
////////////////SD Manager/////////////////////
///////////////////////////////////////////////
void DetectorConstruction::ConstructSDandField()
{
  
  G4SDManager* SDman   =  G4SDManager::GetSDMpointer(); 

  // add PMTs
  PMTSD* sd = new PMTSD("PhotoCathode",1,"PhCathodeHC" );    
  SDman->AddNewDetector(sd); 
  logicalPmtCathode->SetSensitiveDetector(sd);
/*
  GermaniumSD* gesd = new GermaniumSD("GeDetector");    
  SDman->AddNewDetector(gesd); 
  logicalGeDet->SetSensitiveDetector(gesd);
*/

  G4PhysicalVolumeStore* theStore = G4PhysicalVolumeStore::GetInstance();
  G4cout << "\t DetectorConstruction::SDandField done   " << G4endl;
  if(SdDebug){
    for(G4int istore = 0; istore< theStore->size() ; ++istore ){
      G4VPhysicalVolume *pvol = theStore->at(istore);
      G4int nsense = 0;
      G4VSensitiveDetector* sdet = pvol->GetLogicalVolume()->GetSensitiveDetector();
      if(sdet) nsense = sdet->GetNumberOfCollections();
      if(nsense>0) G4cout << " SD phys vol  " << istore << 
        " name = " << pvol->GetName() << " logical " <<  pvol->GetLogicalVolume()->GetName() <<  " number of collections =  " << nsense << G4endl; 
    } 
  }
   
}

//
/// optical properties of lar in several places
void DetectorConstruction::ArgonOpticalProperties()
{
	//Taken from home/gold/MaGe-master/munichteststand/src/GEMPIKLArGe.cc
  G4int num = 500;
  static const G4double temp = 88.5*kelvin;
 // static const G4double LambdaE = twopi *1.973269602e-16 * m * GeV;
  G4double scint_yield = 40000./MeV; 

  G4int ji;
  G4double e;
  G4double ee;

  G4double PPCKOVHighE = LambdaE / (115*nanometer);
  G4double PPCKOVLowE = LambdaE / (650*nanometer); 
  G4double de = ((PPCKOVHighE - PPCKOVLowE) / ((G4double)(num-1)));
  // liquid argon (LAr)  
  G4double LAr_PPCK[(num)];
  G4double LAr_RIND[(num)];
  G4double LAr_RAYL[(num)];
  G4double LAr_ABSL[(num)];
  
  G4double lar_absl_xuv = 60*cm;
	G4double lar_absl_vis = 1000*m;
 
  for (ji = 0; ji < num; ji++) {
    e = PPCKOVLowE + ((G4double)ji) * de;
    LAr_PPCK[ji] = e;
    LAr_RIND[ji] = LArRefIndex((LambdaE / e));
    LAr_RAYL[ji] = LArRayLength((LambdaE / e), temp);

    if (((LambdaE / e)/nm) < 200.0) {
      LAr_ABSL[ji] = lar_absl_xuv;
    } else {
      LAr_ABSL[ji] = lar_absl_vis;
    }
  }

  G4double PPSCHighE = LambdaE /(115*nanometer);
  //G4double PPSCLowE = LambdaE /(650*nanometer);
  //Argon does not Scinillate at 650, so I arbitarly cut the value off at 300 nm
  G4double PPSCLowE = LambdaE/(300*nanometer);
  G4double dee = ((PPSCHighE - PPSCLowE) / ((G4double)(num-1)));
  G4double LAr_SCIN[num];
  G4double LAr_SCPP[num];
  for (ji = 0; ji < num; ji++) {
    ee=PPSCLowE+ ((G4double)ji) * dee;
    LAr_SCPP[ji]=ee;
    LAr_SCIN[ji]=ArScintillationSpectrum(ee);
    // plot spectrum
    hArPhotonE->SetBinContent(  hArPhotonE->FindBin(ee), LAr_SCIN[ji]);
    hArPhotonWavelength->SetBinContent(hArPhotonWavelength->FindBin(LambdaE/ee) ,LAr_SCIN[ji]);
    if(LArDebug){
      G4cout<<"DetectorConstruction::ArgonOpticalProperties()...LAr Energy Spec = "<<LAr_SCPP[ji]<<G4endl;
      G4cout<<"DetectorConstruction::ArgonOpticalProperties()...LAr Scint Spec = "<<LAr_SCIN[ji] <<G4endl;
    }
  }


  G4MaterialPropertiesTable* LAr_mt = new G4MaterialPropertiesTable();

  LAr_mt->AddProperty("RINDEX",        LAr_PPCK, LAr_RIND, num);
  LAr_mt->AddProperty("RAYLEIGH",      LAr_PPCK, LAr_RAYL, num);
  LAr_mt->AddProperty("ABSLENGTH",     LAr_PPCK, LAr_ABSL, num);
  if ( (LAr_SCPP[0] >= PPCKOVLowE) &&
       (LAr_SCPP[(sizeof(LAr_SCPP)/sizeof(G4double) - 1)] <= PPCKOVHighE) )
    {
      LAr_mt->AddProperty("FASTCOMPONENT",LAr_SCPP,LAr_SCIN,num);
      LAr_mt->AddProperty("SLOWCOMPONENT",LAr_SCPP,LAr_SCIN,num);
    }
  LAr_mt->AddConstProperty("SCINTILLATIONYIELD",scint_yield);
  LAr_mt->AddConstProperty("RESOLUTIONSCALE",1.0);
  LAr_mt->AddConstProperty("FASTTIMECONSTANT", 5.95*ns);//6.*ns);
  LAr_mt->AddConstProperty("SLOWTIMECONSTANT",922*ns);//1590.*ns);
  LAr_mt->AddConstProperty("YIELDRATIO",0.23);
  
  G4double fano = 0.11;
  LAr_mt->AddConstProperty("RESOLUTIONSCALE",fano); 
  mat_ArLiq->SetMaterialPropertiesTable(LAr_mt); // G4Material defined in Detector_Materials.icc
  mat_ArLiq->GetIonisation()->SetBirksConstant(5.1748e-4*cm/MeV);//0.0144*mm/MeV);
}

G4double DetectorConstruction::LArEpsilon(const G4double lambda)
{
  G4double epsilon;
  if (lambda < 110*nanometer) return 1.0e4; // lambda MUST be > 110.0 nm
  epsilon = lambda / micrometer; // switch to micrometers
  epsilon = 1.0 / (epsilon * epsilon); // 1 / (lambda)^2
  epsilon = 1.2055e-2 * ( 0.2075 / (91.012 - epsilon) +
                          0.0415 / (87.892 - epsilon) +
                          4.3330 / (214.02 - epsilon) );
  epsilon *= (8./12.); // Bideau-Sellmeier -> Clausius-Mossotti
  G4double LArRho = 1.396*g/cm3;
  G4double GArRho = 1.66e-03*g/cm3;
  epsilon *= (LArRho / GArRho); // density correction (Ar gas -> LAr liquid)
  if (epsilon < 0.0 || epsilon > 0.999999) return 4.0e6;
  epsilon = (1.0 + 2.0 * epsilon) / (1.0 - epsilon); // solve Clausius-Mossotti
  return epsilon;
}

G4double DetectorConstruction::LArRefIndex(const G4double lambda)
{
 //G4cout<< ( sqrt(LArEpsilon(lambda)))<<G4endl;
 return ( sqrt(LArEpsilon(lambda)) ); // square root of dielectric constant
}
G4double DetectorConstruction::LArRayLength(const G4double lambda,const
				   G4double temp)
{
  G4double dyne = 1.0e-5*newton;
  static const G4double LArKT = 2.18e-10 * cm2/dyne; // LAr isothermal compressibility
  static const G4double k = 1.380658e-23 * joule/kelvin; // the Boltzmann constant
  G4double h;
  h = LArEpsilon(lambda);
  if (h < 1.00000001) h = 1.00000001; // just a precaution
  h = (h - 1.0) * (h + 2.0); // the "dielectric constant" dependance
  h *= h; // take the square
  h *= LArKT * temp * k; // compressibility * temp * Boltzmann constant
  h /= lambda * lambda * lambda * lambda; // (lambda)^4
  h *= 9.18704494231105429; // (2 * Pi / 3)^3
  if ( h < (1.0 / (10.0 * km)) ) h = 1.0 / (10.0 * km); // just a precaution
  if ( h > (1.0 / (0.1 * nanometer)) ) h = 1.0 / (0.1 * nanometer); // just a precaution
  return ( 1.0 / h );
}
// arg is energy , returns probability 
//  FWHM at 80K from J Chem Phys vol 91 (1989) 1469 E Morikawa et al
G4double DetectorConstruction::ArScintillationSpectrum(const G4double ee)
{
  G4double meanWave = 128.0*nm; //nm
  G4double meanE = LambdaE/meanWave;
  G4double sigmaE =  (0.56/2.355)*eV;  // sigma(ruler) from FWHM and convert ruler to electron volts
  G4double x = (ee-meanE)/sigmaE;
  G4double emit = exp(-0.5*x*x);
  return emit;
}

void DetectorConstruction::WLSOpticalProperties()
{
  // add WLS 
  // -- WLS: TPB (Tetraphenyl butadiene)
  // --M.Gold from Gehmann et al plot

  fTPB = G4Material::GetMaterial("TPB", false);
  if (fTPB == 0) {
    G4Element* elementH = nist->FindOrBuildElement("H");
    G4Element* elementC = nist->FindOrBuildElement("C");
    fTPB= new G4Material("TPB", 1*g/cm3, 2, kStateSolid);
    fTPB->AddElement(elementH, 22);
    fTPB->AddElement(elementC, 28);
  }
   // Now attach the optical properties to it.
   // Build table with photon energies
   
   G4double HighETPB = LambdaE /(350*nanometer);
   G4double LowETPB = LambdaE /(650*nanometer);//(650*nanometer); //598
   G4double deeTPB = ((HighETPB - LowETPB) / ((G4double)(numTPB-1)));
   G4double LAr_SCPPTPB[numTPB];
   for (G4int ji = 0; ji < numTPB; ji++)  LAr_SCPPTPB[ji]=LowETPB+ ((G4double)ji) * deeTPB;
   
   G4double WLS_absorption[numTPB];
   G4double WLS_emission[numTPB];
   G4double WLS_refraction[numTPB];
   
   // make new table 
   tpbTable = new G4MaterialPropertiesTable();
   for (G4int ji=0;ji < numTPB; ji++) {
     WLS_refraction[ji] = 1.15; //Lehnert_Dresden_PhDThesis-GeDet.pdf page 180
     // Should the TPB shift the Cherenkov light?
     // This makes a tail starting at 128 until the visible.
     if (LAr_SCPPTPB[ji] > 3.31*eV){// < 374.57 nm 
       // For the moment set it to always absorb photons
       WLS_absorption[ji] = 0.001*nm; //absorbs UV (always)
     } else {
       // < 350 nm
       WLS_absorption[ji] = 10.5*m; //otherwise transparent
     }
     WLS_emission[ji] = TPBEmissionSpectrum(LAr_SCPPTPB[ji]);
     hWLSPhotonE->SetBinContent(hWLSPhotonE->FindBin(LAr_SCPPTPB[ji]),WLS_emission[ji]);
     hWLSPhotonWavelength->SetBinContent(hWLSPhotonWavelength->FindBin(LambdaE/LAr_SCPPTPB[ji]),WLS_emission[ji]);
     if(TpbDebug){
      G4cout<<" WLS Emmsion "<<WLS_emission[ji]<<" LAr Energy "<<LAr_SCPPTPB[ji]<<G4endl;
      G4cout<<" WLS Absorption Length "<<WLS_absorption[ji]<<" LAr Energy "<<LAr_SCPPTPB[ji]<<G4endl;
     }
   }

   for (G4int ji=0; ji<numTPB ;  ++ ji) {
       hWlsEmission->SetBinContent(ji+1,WLS_emission[ji]);
       hWlsAbsorption->SetBinContent(ji+1,WLS_absorption[ji]);
       hWlsRefraction->SetBinContent(ji+1,WLS_refraction[ji]);
   }

   tpbTable->AddProperty("RINDEX",LAr_SCPPTPB,WLS_refraction,numTPB);
   tpbTable->AddProperty("WLSABSLENGTH",LAr_SCPPTPB,WLS_absorption,numTPB);
   tpbTable->AddProperty("WLSCOMPONENT",LAr_SCPPTPB,WLS_emission,numTPB);
   // From WArP
   tpbTable->AddConstProperty("WLSTIMECONSTANT", 0.01*ns);
   G4double WLSyield = 1.2;// should be integral of TGraph!MG!!
   tpbTable->AddConstProperty("WLSMEANNUMBERPHOTONS",WLSyield);
   fTPB->SetMaterialPropertiesTable(tpbTable);
}
void DetectorConstruction::PMTGlassOpticalProperties(){
  /// Optical properties of the PMT glass:
  const G4int num_entries = 24;
  G4double PMTGlassEnergy[num_entries] = {
                                          LambdaE/(115*nanometer),
                                          LambdaE /(160.8*nanometer),
                                          LambdaE /(170*nanometer), LambdaE /(182.3*nanometer), LambdaE /(200*nanometer),
                                          LambdaE/(230*nanometer), LambdaE /(250*nanometer), LambdaE /(270*nanometer), LambdaE /(300*nanometer), LambdaE /(320*nanometer),
                                          LambdaE/(340*nanometer), LambdaE /(360*nanometer), LambdaE /(380*nanometer), LambdaE /(400*nanometer), LambdaE /(450*nanometer),
                                          LambdaE/(500*nanometer), LambdaE /(550*nanometer), LambdaE /(580*nanometer), LambdaE /(600*nanometer), LambdaE /(630*nanometer),
                                          LambdaE/(660*nanometer), LambdaE /(700*nanometer), LambdaE /(730*nanometer), LambdaE /(750*nanometer)};
  G4double PMTGlassReflectivity[num_entries] = {0.0,0.0,0.0,0.0,0.0,
                                                0.0,0.0,0.0,0.0,0.0,
                                                0.0,0.0,0.0,0.0,0.0,
                                                0.0,0.0,0.0,0.0};
  G4double PMTGlassEfficiency[num_entries] = {0.0000,0.0034,0.0322,0.0741,0.1297,
                                              0.1450,0.1673,0.1965,0.2348,0.2473,
                                              0.2467,0.2399,0.2368,0.2264,0.1847,
                                              0.1309,0.0692,0.0371,0.0231,0.0104,
                                              0.0036,0.0006,0.0001,0.0000};

  G4MaterialPropertiesTable *PMTGlassOptTable = new G4MaterialPropertiesTable();
  PMTGlassOptTable->AddProperty("REFLECTIVITY",PMTGlassEnergy,PMTGlassReflectivity,num_entries);
  PMTGlassOptTable->AddProperty("EFFICIENCY",PMTGlassEnergy,PMTGlassEfficiency,num_entries);

  fMaterialPMTGlass->SetMaterialPropertiesTable(PMTGlassOptTable);
}
void DetectorConstruction::GeOpticalProperties()
{
  /*
  nist = G4NistManager::Instance();
  fGeMaterial = nist->FindOrBuildMaterial("G4_Ge");
  */
  G4double abundance;
  G4String name;
  G4int natoms;
  G4Isotope* Ge70 = new G4Isotope(name="Ge70",  32, 70, 69.92*g/mole);
  G4Isotope* Ge72 = new G4Isotope(name="Ge72",  32, 72, 71.92*g/mole);
  G4Isotope* Ge73 = new G4Isotope(name="Ge73",  32, 73, 73.0*g/mole);
  G4Isotope* Ge74 = new G4Isotope(name="Ge74",  32, 74, 74.0*g/mole);
  G4Isotope* Ge76 = new G4Isotope(name="Ge76",  32, 76, 76.0*g/mole);

  G4Element* elGeEnr = new G4Element(name="enrichedGermanium","GeEnr",5);
  elGeEnr->AddIsotope(Ge70,abundance= 0.0*perCent);
  elGeEnr->AddIsotope(Ge72,abundance= 0.1*perCent);
  elGeEnr->AddIsotope(Ge73,abundance= 0.2*perCent);
  elGeEnr->AddIsotope(Ge74,abundance= 13.1*perCent);
  elGeEnr->AddIsotope(Ge76,abundance= 86.6*perCent);

  //enriched Ge
  G4double density = 5.54*g/cm3;
  fGeMaterial = new G4Material(name="EnrichedGe", density, 1);
  fGeMaterial->AddElement(elGeEnr,natoms=1);
  G4double fano = 0.129;
  
  GeMaterialTable = new G4MaterialPropertiesTable();

  const G4int numGe = 100;
  //Ge_Reflectivity TGraph range is 111-657 nm
  G4double HighEGe = LambdaE /(115*nanometer);
  G4double LowEGe = LambdaE /(650*nanometer);//(650*nanometer); //598
  G4double deeGe = ((HighEGe - LowEGe) / ((G4double)(numGe-1)));
  
  G4double NRGSpec[numGe];//if we are going to use the dumb names from MaGe then I get to use my dumb names too!
  G4double ReflectionSpec[numGe];

  G4double Efficiency[numGe];

  G4double ReReflection[numGe];
  G4double ImReflection[numGe];

  G4cout<<"********  DetectorConstruction dump of GeMaterial Table " << G4endl;
  for(G4int i = 0; i < numGe ; i++) {
    NRGSpec[i] = LowEGe +( (G4double) i*deeGe );
    ReflectionSpec[i] = GeReflectionSpectrum( (LambdaE /NRGSpec[i])/nm );//in nm
    //ReReflection[i] = GeReflectionSpectrum( (LambdaE /NRGSpec[i])/nm );//function uses nm
    //ImReflection[i]=0.5;
    Efficiency[i] = 1.;
    if(GeDebug){
      G4cout<<"\t GeOpticalProperties() bin " << i << " ...Energy Spec = "<<NRGSpec[i]/eV << 
      " ...Wavelength Spec = "<<(LambdaE /NRGSpec[i])/nm<<
      " ...Reflection REFLECTIVITY = "<<ReReflection[i] << G4endl;
      //" ...Reflection Re = "<<ReReflection[i]<<
      //" ... Im  = "<<ImReflection[i]<<G4endl;
    }
  }
  GeMaterialTable->AddProperty("REFLECTIVITY",NRGSpec,ReflectionSpec,numGe);
  //GeMaterialTable->AddProperty("REALRINDEX",NRGSpec, ReReflection, numGe); 
  //GeMaterialTable->AddProperty("IMAGINARYRINDEX",NRGSpec, ImReflection, numGe); 
  GeMaterialTable->AddProperty("EFFICIENCY",NRGSpec,Efficiency,numGe);

  //GeMaterialTable->DumpTable();
  
  fGeOpticalSurface = new G4OpticalSurface("GermaniumDetSurface");
  fGeOpticalSurface->SetType(dielectric_metal);
  fGeOpticalSurface->SetModel(glisur);
  fGeOpticalSurface->SetFinish(polished);
  fGeOpticalSurface->SetPolish(0.5);

  fGeOpticalSurface->SetMaterialPropertiesTable(GeMaterialTable);
  /*
  //added by Neil in a vayne attempt to track Ge hits in Tracking Action
  GeMaterialTable->AddConstProperty("RESOLUTIONSCALE",fano);
  GeMaterialTable->AddConstProperty("RESOLUTIONSCALE",1.0);

  fGeMaterial->SetMaterialPropertiesTable(GeMaterialTable);
  */
}

void DetectorConstruction::CuOpticalProperties()
{
  nist = G4NistManager::Instance();
  fCuMaterial = nist->FindOrBuildMaterial("G4_Cu");
  if(!fCuMaterial) G4cout<<"DetectorConstruction::CuOpticalProperties()...Copper Material not found in Nist Manager!"<<G4endl;
  CuMaterialTable = new G4MaterialPropertiesTable();
  
  const G4int numCu = 100;
  //Ge_Reflectivity TGraph range is 112.74165-654.16459 nm
  G4double HighEGe = LambdaE /(115*nanometer);
  G4double LowEGe = LambdaE /(650*nanometer);//(650*nanometer); //598
  G4double deeGe = ((HighEGe - LowEGe) / ((G4double)(numCu-1)));
  G4double NRGSpec[numCu];//if we are going to use the dumb names from MaGe then I get to use my dumb names too!
  G4double ReflectionSpec[numCu];
  for(G4int i = 0; i < numCu ; i++) {
    NRGSpec[i] = LowEGe +( (G4double) i*deeGe );
    ReflectionSpec[i] = CuReflectionSpectrum( (LambdaE /NRGSpec[i])/nm );//in nm
    if(CuDebug){
      G4cout<<"DetectorConstruction::CuOpticalProperties()...Energy Spec = "<<NRGSpec[i]/eV<<G4endl;
      G4cout<<"DetectorConstruction::CuOpticalProperties()...Wavelength Spec = "<<(LambdaE /NRGSpec[i])/nm<<G4endl;
      G4cout<<"DetectorConstruction::CuOpticalProperties()...Reflection Spec = "<<ReflectionSpec[i]<<G4endl;
    }
  }
  
  CuMaterialTable->AddProperty("REFLECTIVITY",NRGSpec,ReflectionSpec,numCu);
  
  fCuOptSurface = new G4OpticalSurface("Cu surface");
  fCuOptSurface->SetType(dielectric_metal);
  fCuOptSurface->SetFinish(ground);
  fCuOptSurface->SetPolish(0.5);
  //fCuOptSurface->SetMaterialPropertiesTable(CuMaterialTable);
  
  fCuMaterial->SetMaterialPropertiesTable(CuMaterialTable);
}


void DetectorConstruction::IronOpticalProperties()
{
  IronMaterialTable = new G4MaterialPropertiesTable();
  
  const G4int num = 100;
  //Ge_Reflectivity TGraph range is 112.74165-654.16459 nm
  G4double HighEGe = LambdaE /(115*nanometer);
  G4double LowEGe = LambdaE /(650*nanometer);//(650*nanometer); //598
  G4double deeGe = ((HighEGe - LowEGe) / ((G4double)(num-1)));
  G4double NRGSpec[num];//if we are going to use the dumb names from MaGe then I get to use my dumb names too!
  G4double ReflectionSpec[num];
  for(G4int i = 0; i < num ; i++) {
    NRGSpec[i] = LowEGe +( (G4double) i*deeGe );
    ReflectionSpec[i] = IronReflectionSpectrum( (LambdaE /NRGSpec[i])/nm );//in nm
    if(IronDebug){
      G4cout<<"DetectorConstruction::IronOpticalProperties()...Energy Spec = "<<NRGSpec[i]/eV<<G4endl;
      G4cout<<"DetectorConstruction::IronOpticalProperties()...Wavelength Spec = "<<(LambdaE /NRGSpec[i])/nm<<G4endl;
      G4cout<<"DetectorConstruction::IronOpticalProperties()...Reflection Spec = "<<ReflectionSpec[i]<<G4endl;
    }
  }
  
  IronMaterialTable->AddProperty("REFLECTIVITY",NRGSpec,ReflectionSpec,num);
  
  fIronOptSurface = new G4OpticalSurface("SSteel surface");
  fIronOptSurface->SetType(dielectric_metal);
  fIronOptSurface->SetFinish(ground);
  fIronOptSurface->SetPolish(0.5);
  //fIronOptSurface->SetMaterialPropertiesTable(IronMaterialTable);
  
  fSSteelMaterial->SetMaterialPropertiesTable(IronMaterialTable);
}

void DetectorConstruction::VM2000OpticalProperties()
{
  VM2000MaterialTable = new G4MaterialPropertiesTable();
  
  const G4int numVM2000 = 100;
  //Ge_Reflectivity TGraph range is 112.74165-654.16459 nm
  G4double HighEGe = LambdaE /(115*nanometer);
  G4double LowEGe = LambdaE /(650*nanometer);//(650*nanometer); //598
  G4double deeGe = ((HighEGe - LowEGe) / ((G4double)(numVM2000-1)));
  
  G4double NRGSpec[numVM2000];//if we are going to use the dumb names from MaGe then I get to use my dumb names too!
  G4double ReflectionSpec[numVM2000];
  
  for (G4int i = 0; i < numVM2000 ; i++) { 
    NRGSpec[i] = LowEGe +( (G4double) i*deeGe );
    if (NRGSpec[i] < (LambdaE/(370*nanometer)))
      ReflectionSpec[i] = 0.98; //visable
    else
      ReflectionSpec[i] = 0.15; //UV
    
    if(VM2000Debug){
      G4cout<<"DetectorConstruction::VM2000OpticalProperties()...Energy Spec = "<<NRGSpec[i]/eV<<G4endl;
      G4cout<<"DetectorConstruction::VM2000OpticalProperties()...Wavelength Spec = "<<(LambdaE /NRGSpec[i])/nm<<G4endl;
      G4cout<<"DetectorConstruction::VM2000OpticalProperties()...Reflection Spec = "<<ReflectionSpec[i]<<G4endl;
    }
  }
  VM2000MaterialTable->AddProperty("REFLECTIVITY",NRGSpec,ReflectionSpec,numVM2000);
  fVM2000OptSurface = new G4OpticalSurface("VMOptSurface");
  fVM2000OptSurface->SetType(dielectric_dielectric);
  fVM2000OptSurface->SetFinish(polishedfrontpainted);
  fVM2000OptSurface->SetMaterialPropertiesTable(VM2000MaterialTable);
  fVM2000->SetMaterialPropertiesTable(VM2000MaterialTable);
  G4cout<<"DetectorConstruction::VM2000OpticalProperties" << G4endl;
  //VM2000MaterialTable->DumpTable();
  G4MaterialPropertyVector* PropertyPointer=NULL; 
  PropertyPointer = VM2000MaterialTable->GetProperty("REFLECTIVITY");
  if(PropertyPointer)  G4cout<<"DetectorConstruction::VM20000 property pointer found " << G4endl;
  else G4cout<<"DetectorConstruction::VM20000 PROPERTY POINTER NOT FOUND " << G4endl;
  
}

void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetOverlapsCheck(G4bool f_check)
{
	//checkOverlaps = f_check;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetFillMaterial(G4String smaterial)
{
	innerVessel_FillMaterial = smaterial;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetShieldStyle(G4String f_type)
{
	//detector_type = f_type;
}
