//---------------------------------------------------------------------------//
//bb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nu//
//                                                                           //
//                            MaGe Simulation                                //
//                                                                           //
//      This code implementation is the intellectual property of the         //
//      MAJORANA and Gerda Collaborations. It is based on Geant4, an         //
//      intellectual property of the RD44 GEANT4 collaboration.              //
//                                                                           //
//                        *********************                              //
//                                                                           //
//    Neither the authors of this software system, nor their employing       //
//    institutes, nor the agencies providing financial support for this      //
//    work  make  any representation or  warranty, express or implied,       //
//    regarding this software system or assume any liability for its use.    //
//    By copying, distributing or modifying the Program (or any work based   //
//    on on the Program) you indicate your acceptance of this statement,     //
//    and all its terms.                                                     //
//                                                                           //
//bb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nu//
//---------------------------------------------------------------------------//
//                                                          
//
//---------------------------------------------------------------------------//
/**
 * SPECIAL NOTES:
 * 
 * Original desing by Alexander Klimenko
 */
// 
//---------------------------------------------------------------------------//
/**
 *
 * AUTHOR:  Markus Knapp
 * CONTACT: @CONTACT@
 * FIRST SUBMISSION: 
 * 
 * REVISION:
 *
 */
//---------------------------------------------------------------------------//
//



#include "PMTSD.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4UserEventAction.hh"
#include "G4EventManager.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "Randomize.hh"

//#include "io/MGLogger.hh"//Gerda log maker...will flush later


PMTSD::PMTSD(G4String name, G4int nCells, G4String colName) 
:G4VSensitiveDetector(name),numberOfCells(nCells),HCID(-1)
{
  fParticleFilter = new G4SDParticleFilter("PmtSDFilter");
  fParticleFilter->add("opticalphoton");
  fParticleFilter->show();
  
  G4String HCname;
  collectionName.insert(HCname=colName);
  fDir = LegendAnalysis::Instance()->topHistDir()->mkdir("PMThits");
  fDir->cd();
  hTime = new TH1F("PMTHitsTime"," PMT hit time  ",2000,0,4000);
  hTime->GetYaxis()->SetTitle(" photons/(2ns)");
  hTime->GetXaxis()->SetTitle(" time (ns) ");

  hWavelength = new TH1F("PMTWavelength"," absorbed photon wavelength ",900,100,1000);
  hWavelength->GetYaxis()->SetTitle(" number of photons/(nm) ");
  hWavelength->GetXaxis()->SetTitle(" wavelength (nm) ");

  // must be in top directory for ChangeFile to work
  LegendAnalysis::Instance()->topTreeDir()->cd();

  ntWLS = new TNtuple("ntWLS"," WLS hits ","evId:copy:PDG:musec:micron:ekev:posxum:posyum:posyzum");

  
}

PMTSD::~PMTSD()
{
}

void PMTSD::Initialize(G4HCofThisEvent* /*HCE*/)
{
  //G4cout <<  "\tPMTSD Initialized   " <<  G4endl;
}

G4bool PMTSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  return ProcessHits_constStep(aStep,NULL);
}

G4bool PMTSD::ProcessHits_constStep(const G4Step* aStep, G4TouchableHistory* )
{
  if(aStep==NULL) {
    //G4cout << " ProcessHits_constStep called with null step " << G4endl;
    return false;
  }
  G4ParticleDefinition* particleType = aStep->GetTrack()->GetDefinition();
  G4double length = aStep->GetStepLength();
  G4String particleName = particleType->GetParticleName();
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4double gtime = aStep->GetPostStepPoint()->GetGlobalTime();   //measured in nanoseconds;
  G4double time = aStep->GetTrack()->GetGlobalTime();
  G4ThreeVector position = aStep->GetTrack()->GetPosition();
  //G4cout << "testpoint time " << time << " global time   " << gtime << " edep " << edep << " particle name " << particleName << G4endl; 
  //if((particleName != "opticalphoton")) return false;
  //const G4VPhysicalVolume* physVol = aStep->GetPostStepPoint()->GetPhysicalVolume();
  hTime->Fill( aStep->GetTrack()->GetGlobalTime()/ns ); //convert to ns
  G4double wavelength =  CLHEP::h_Planck*CLHEP::c_light/edep/nm;//700 nm
  hWavelength->Fill(wavelength);

  G4int eventId = G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->GetEventID();
  //how to get volume names
  G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
  G4int copy = theTouchable->GetVolume()->GetCopyNo();

  //ntWLS->Fill(float(eventId),float(copy),particleType->GetPDGEncoding(),aStep->GetTrack()->GetGlobalTime()/microsecond,
      //length/micrometer,edep/keV,position(0)/micrometer,position(1)/micrometer,position(2)/micrometer);
  return true;
}

void PMTSD::EndOfEvent(G4HCofThisEvent*HCE)
{
  //if(HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  //HCE->AddHitsCollection( HCID, PMTCollection );
}

void PMTSD::clear()
{

} 

void PMTSD::DrawAll()
{
  
} 

void PMTSD::PrintAll()
{

} 




