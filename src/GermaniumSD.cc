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



#include "GermaniumSD.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4OpticalPhoton.hh"
#include "G4SDManager.hh"
#include "G4UserEventAction.hh"
#include "G4EventManager.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"

//#include "io/MGLogger.hh"//Gerda log maker...will flush later


GermaniumSD::GermaniumSD(G4String name, G4int nCopy) 
:G4VSensitiveDetector(name),fCopy(nCopy)
{
  // init filter
  fKineticFilter = new G4SDKineticEnergyFilter("GeSDFilter",2.0E-4,DBL_MAX); // threshold is 0.2 KeV
  fKineticFilter->show();
  
  if(fCopy==0) {
    fDir = LegendAnalysis::Instance()->topHistDir()->mkdir("GeThits");
    fDir->cd();
    hTime = new TH1F("GeHitsTime"," Ge hit time  ",2000,0,1000);
    hTime->GetYaxis()->SetTitle(" hits )");
    hTime->GetXaxis()->SetTitle(" time (micro-s) ");

    hEnergy = new TH1F("GeEnergy"," absorbed energy ",2000,0,200);
    hEnergy->GetYaxis()->SetTitle(" hits ");
    hEnergy->GetXaxis()->SetTitle(" energy (keV) ");

    // must be in top directory for ChangeFile to work
    LegendAnalysis::Instance()->topTreeDir()->cd();
    ntGe = new TNtuple("ntGe"," Ge hits ","evId:PDG:musec:micron:edep:ke:beta:posxum:posyum:poszum");
  }
  
}

GermaniumSD::~GermaniumSD()
{
}

void GermaniumSD::Initialize(G4HCofThisEvent* /*HCE*/)
{
  //G4cout <<  "\tGermaniumSD Initialized   " <<  G4endl;
}

G4bool GermaniumSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  return ProcessHits_constStep(aStep,NULL);
}

G4bool GermaniumSD::ProcessHits_constStep(const G4Step* aStep, G4TouchableHistory* )
{
  if(aStep==NULL) {
    G4cout << "GermaniumSD::ProcessHits_constStep() called with null step " << G4endl;
    return false;
  }
  // Ignore energy deposited by optical photons
  if (aStep->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()){
    return false;
  }
  
  LTEvent* ltEvent = LegendAnalysis::Instance()->getEvent();

  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep <= 0){
   // G4cout<<"GermaniumSD::ProcessHits_constStep()...No Energy Deposited, returnin false"<<G4endl; TODO
   // return false;
  }
  G4double length = 					aStep->GetStepLength();
  G4double KE = aStep->GetTrack()->GetKineticEnergy();
    
  // how to get volume name
  G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
  G4int GePostNumber = aStep->GetPostStepPoint()->GetTouchableHandle()->GetCopyNumber();
  G4Track* aTrack = aStep->GetTrack();
  //const G4VPhysicalVolume* physVol = aStep->GetPostStepPoint()->GetPhysicalVolume();
  
  G4ParticleDefinition* particleType = aStep->GetTrack()->GetDefinition();
  G4String particleName = particleType->GetParticleName();
  G4double gtime = aStep->GetPostStepPoint()->GetGlobalTime();   //measured in nanoseconds;
  G4double time = aStep->GetTrack()->GetGlobalTime();
  G4ThreeVector position = aStep->GetTrack()->GetPosition();
  G4double vel = (aStep->GetTrack()->GetVelocity())/(meter/second);
  
  hTime->Fill( aStep->GetTrack()->GetGlobalTime()/microsecond); //convert to ns
  hEnergy->Fill(edep/keV);
  
  G4int copy = theTouchable->GetVolume()->GetCopyNo();
  // add event number
//G4cout<<"GermaniumSD pre/post volume name "<<aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()
  //<<"/"<<aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()<<G4endl;
  /*G4cout <<  " GeSD:: Particle Name = "<<
        aStep->GetTrack()->GetDefinition()->GetParticleName()<<
          ", Energy Deposited = "<<aStep->GetTotalEnergyDeposit()/keV<<
          " kinetic Energy = "<< KE/keV<<G4endl;
  */
  G4int eventId = G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->GetEventID();
//  G4cout<<"GermaniumSD:: PDGEncoding = "<<particleType->GetPDGEncoding()<<G4endl;
  //ntGe->Fill( float(eventId),float(particleType->GetPDGEncoding()),aStep->GetTrack()->GetGlobalTime()/microsecond,
    //  length/micrometer,edep/keV,KE/keV,vel/299792458,position(0)/micrometer,position(1)/micrometer,position(2)/micrometer);
  
  return true;
}

void GermaniumSD::EndOfEvent(G4HCofThisEvent*HCE)
{
  //if(HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  //HCE->AddHitsCollection( HCID, PMTCollection );
}

void GermaniumSD::clear()
{

} 

void GermaniumSD::DrawAll()
{
  
} 

void GermaniumSD::PrintAll()
{

} 




