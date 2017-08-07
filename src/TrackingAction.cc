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
// $Id: TrackingAction.cc 68752 2013-04-05 10:23:47Z gcosmo $
//
/// \file optical/Legend/src/TrackingAction.cc
/// \brief Implementation of the TrackingAction class
//
//
#include "TrackingAction.hh"
#include "G4EventManager.hh"
#include "DetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4ParticleTypes.hh"
#include "G4SystemOfUnits.hh"
#include "TString.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::TrackingAction()
{
  // create directory 
  fDir = LegendAnalysis::Instance()->topHistDir()->mkdir("track");
  fDir->cd();
  G4cout<<" TrackingAction working root directory  is  " << G4endl;  
  gDirectory->pwd();
  G4double LowE = 2.4796*eV;//500 nm
  G4double HighE = 12.3984*eV;//100 nm
  /* 
  G4double LowE = 1.7712*eV;//700 nm
  G4double HighE = 12.3984*eV;//100 nm
  hWLSPhotonE = new TH1F("StepWLSPhotonE"," photon energy from WLS",1000,LowE,HighE);
   */
  G4double LowWLS =  h_Planck*c_light/(700.0*nm);//700 nm
  G4double HighWLS = h_Planck*c_light/(200.0*nm);//200 nm
  hTrackScintE = new TH1F("TrackScintE"," scint photon energy in LAr",1000,LowE,HighE);
  hTrackPhotonE = new TH1F("TrackPhotonE"," all photon energy in LAr",1000,LowE,HighE);
  hTrackScintYield = new TH1F("TrackScintYield"," scint photon yield",1000,0,5);
  hTrackScintYield->SetXTitle(" mean scint photons / primary energy (MeV) ");
  hAbsorbedPhotonE = new TH1F("AbsorbedPhotonE"," absorbed scint photon energy in LAr",1000,LowE,HighE);
  hWLSPhotonE = new TH1F("WLSPhotonE"," WLS photon energy ",1000,LowWLS,HighWLS);
  hPMTPhotonE = new TH1F("PMTPhotonE"," WLS photon energy ",1000,LowWLS,HighWLS);
  hCherenkovPhotonE  = new TH1F("CherenkovPhotonE"," WLS photon energy ",1000,LowWLS,HighWLS);
  
  // must be in top directory for ChangeFile to work
  LegendAnalysis::Instance()->topTreeDir()->cd();
  // make tree in output file
  fTrackTree = new TTree("trkTree","trkTree");
  fLTTrack = new LTTrack();
  fTrackTree->Branch("track",&fLTTrack);

  // make tree for Germanium tracks
  fGeTree = new TTree("geTree","geTree");
  gLTTrack = new LTTrack();
  fGeTree->Branch("geTrk",&gLTTrack);

  // make tree for Germanium tracks
  fNoOptTree = new TTree("noOptTree","noOptTree");
  noptLTTrack = new LTTrack();
  fNoOptTree->Branch("noOptTree",&noptLTTrack);

  
  G4cout << " ...  = " << G4endl;
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PreUserTrackingAction(const G4Track* theTrack)
{
  //if below commented out, Let this be up to the user via vis.mac
  fpTrackingManager->SetStoreTrajectory(true);
  fpTrackingManager->SetTrajectory(new LegendTrajectory(theTrack) );
  fpTrackingManager->SetUserTrackInformation(new UserTrackInformation);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PostUserTrackingAction(const G4Track* theTrack){
  if(!theTrack) { 
    G4cout << " WARNING TrackingAction called with NULL track!  " << G4endl;
    return;
  }
  aTrack=theTrack;
  G4String ParticleName = aTrack->GetDefinition()->GetParticleName();

  if(0 && aTrack->GetDefinition()->GetParticleName() != "e-" && aTrack->GetDefinition()->GetParticleName() != "opticalphoton"  
      && aTrack->GetDefinition()->GetParticleName() != "gamma" && aTrack->GetDefinition()->GetParticleName() != "alpha")
    G4cout<<"TrackAction:: Name = "<<aTrack->GetDefinition()->GetParticleName()<<", KE = "<< aTrack->GetKineticEnergy()<<G4endl;
  
  LTEvent* ltEvent = LegendAnalysis::Instance()->getEvent();
  
  //The UI command /tracking/storeTrajectory _bool_ does the same.
  fpTrackingManager->SetStoreTrajectory(true);
  LegendTrajectory* trajectory=dynamic_cast<LegendTrajectory*>(fpTrackingManager->GimmeTrajectory());
  if(!trajectory) {
    G4cout << " WARNING  TrackingAction::PostUserTrackingAction no trajectory found so returning " << G4endl;
    return;
  }

  //trajectory->SetForceDrawTrajectory(true);
  trajectory->SetDrawTrajectory(false);
  trackInformation= dynamic_cast<UserTrackInformation*>(aTrack->GetUserInformation());
  if(trackInformation->GetParentId() == 0)
    G4cout<<"TrackAction:: Name = "<<aTrack->GetDefinition()->GetParticleName()<<", KE = "<< aTrack->GetKineticEnergy()<<G4endl;
  //LegendAnalysis::Instance()->FillTrajectory(trajectory);

  G4double kineticE = aTrack->GetKineticEnergy();//Returns energy in MeV
  
  trajectory->SetParentId(trackInformation->GetParentId());
  const G4VProcess* creator=aTrack->GetCreatorProcess();
  if(!creator&&trackInformation->GetParentId()!=0) {
    G4cout << " WARNING TrackingAction called with NULL track G4VProcess!  parent id " << trackInformation->GetParentId()   << G4endl;
    hTrackStatus->Fill(isBad); 
    return;
  } 

  if(trackInformation->IsPrimary()) {  
    trajectory->SetDrawTrajectory(true);
    trajectory->SetPrimary();
    ltEvent->ePrimary=kineticE;
    //G4cout << " TrackingAction PRIMARY TRACK track definition is  " << aTrack->GetDefinition()->GetParticleName() << " is prim? " << trajectory->IsPrimary() << G4endl;
  }

   float evertex  =  ltEvent->ePrimary;
   //G4cout << " \t TRACKINGACTION event vertex energy " << evertex << G4endl;
   G4double meanScintE = h_Planck*c_light/(128.0*nm);

  // Hereafter we call current volume the volume where the step has just gone through. Geometrical informations are available from preStepPoint.
  G4StepPoint* currentPoint = aTrack->GetStep()->GetPreStepPoint();
  nextPoint    = aTrack->GetStep()->GetPostStepPoint();
  G4TouchableHandle ctouch = currentPoint->GetTouchableHandle();
  fphysVolName = ctouch->GetVolume()->GetName();
  fcopy = ctouch->GetCopyNumber();
  
  if(trackInformation->GetTrackStatus()&hitGe) {
      //trajectory->SetDrawTrajectory(true);
      ++ltEvent->nGeHits; 
      // fill gLTTrack here
      //fillTTrack(gLTTrack);
      //fGeTree->Fill();
  }
  
  // only keep photons at geometrical boundary 
  //if(aTrack->GetDefinition() ==G4OpticalPhoton::OpticalPhotonDefinition() && trackInformation->GetPostStepLast() == TrackPostStepStatus::isGeomBoundary) {
  if(aTrack->GetDefinition() ==G4OpticalPhoton::OpticalPhotonDefinition() ) {
    ++ltEvent->nOptPhotons;
    hTrackPhotonE->Fill(kineticE);
    if(trackInformation->GetTrackStatus()&absorbed) hAbsorbedPhotonE->Fill(kineticE);
    if(creator->GetProcessName() ==  "Scintillation") {
      //trajectory->SetDrawTrajectory(true);
      ++ltEvent->nArScint;
      hTrackScintE->Fill(kineticE);
      if(evertex>0) hTrackScintYield->Fill( kineticE /evertex /meanScintE);
      //if(evertex>0) G4cout << " \t TRACKINGACTION event vertex energy " << evertex << "  photon energy ratio " << kineticE /evertex << G4endl;
    }
    if(creator->GetProcessName() == "Cerenkov") hCherenkovPhotonE->Fill(kineticE);
    
    // use track status set in SteppingAction
    trajectory->SetTrackStatus(trackInformation->GetTrackStatus());
    if(trackInformation->GetTrackStatus()&hitPMT) {
      hPMTPhotonE->Fill(kineticE);
      trajectory->SetDrawTrajectory(true);
      //trajectory->SetDrawTrajectory(false);
      ++ltEvent->nPmtHits;
    } else if(trackInformation->GetTrackStatus()&hitWLS) {
      //trajectory->SetDrawTrajectory(false);
      //trajectory->SetDrawTrajectory(true);
      hWLSPhotonE->Fill(kineticE);
      ++ltEvent->nWlsScint; 
    } else if(trackInformation->GetTrackStatus()&absGe) {
      /*G4int ibit = G4int( log2(G4int(absGe)) );
      G4cout << " \t TRACKINGACTION absGe = " << trackInformation->GetTrackStatus() << " & " << absGe << " bit " << ibit  << " name " <<  
         trackInformation->GetTrackStatusBitName( ibit ) << G4endl;
       for (G4int istat =0; istat< trackInformation->GetTrackStatusSize() ; ++istat) {
         G4cout << " bit " << istat << " value " << pow(2,istat) << "  " << trackInformation->GetTrackStatusBitName(istat) << G4endl;
       }*/
      ++ltEvent->nAbsGe; 
    } 
    // fill optical photon tree
    //fillTTrack(fLTTrack);
    // fLTTrack->print();
    //fTrackTree->Fill();
  } 

  bool fillSwitch = true;
  if(aTrack->GetDefinition() ==G4OpticalPhoton::OpticalPhotonDefinition()){
    if( !trackInformation->GetTrackStatus()&hitPMT)
      fillSwitch = false;
  }
  if(fillSwitch){
    fillTTrack(noptLTTrack);
    fNoOptTree->Fill();
  }

  /* from Geant4 users guide for application developers,  Version: geant4 10.3 Publication date 9 December 2016 
    The ideal place to copy a G4VUserTrackInformation object from a
    mother track to its daughter tracks is 4UserTrackingAction::PostUserTrackingAction().
  */
  G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
  if(secondaries){
    UserTrackInformation* info =  dynamic_cast<UserTrackInformation*>(aTrack->GetUserInformation());
    size_t nSeco = secondaries->size();
    if(nSeco>0)   {
      for(size_t i=0; i < nSeco; i++) { 
        UserTrackInformation* infoNew = new UserTrackInformation(*info);
        (*secondaries)[i]->SetUserInformation(infoNew);
      }
    }
  }
/* also “The concrete class object is deleted by the Geant4 kernel when the associated G4Track 
** object is deleted. In case the user wants to keep the information, it should be copied to a trajectory corresponding to the track.
*/


}

void::TrackingAction::fillTTrack(LTTrack* lttrk)
{
  lttrk->clear(); // this is needed because of push_backs below 
  lttrk->physVolName = fphysVolName;
  lttrk->copy = fcopy;
  // To check that the particle is leaving the current volume (i.e. it is at the last step in the volume; the postStepPoint is at the boundary):
  lttrk->isLeaving = nextPoint->GetStepStatus() == fGeomBoundary;
  
  lttrk->evId = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  lttrk->trkId = aTrack->GetTrackID();
  lttrk->pdg = aTrack->GetDefinition()->GetPDGEncoding();
  lttrk->parentId = aTrack->GetParentID();
  lttrk->postStatus=trackInformation->GetPostStepStatusVector();
  lttrk->status=trackInformation->GetTrackStatus();
  lttrk->process=trackInformation->GetProcessName();
  lttrk->boundaryStatus = trackInformation->GetBoundaryStatusVector();
  lttrk->stepLength = trackInformation->GetStepLengthVector();
  lttrk->stepKE = trackInformation->GetStepKEVector();
  lttrk->length=aTrack->GetTrackLength();
  lttrk->nstep=aTrack->GetCurrentStepNumber();
  lttrk->trkStepLength=aTrack->GetStepLength();
  G4ThreeVector trkPos = aTrack->GetPosition();
  lttrk->position.SetXYZ(trkPos.x(),trkPos.y(),trkPos.z());
  G4ThreeVector vertPos = aTrack->GetVertexPosition(); 
  lttrk->vertPosition.SetXYZ(vertPos.x(),vertPos.y(),vertPos.z());
   // Global time (time since the current event began)
  lttrk->time=aTrack->GetGlobalTime()/microsecond;
  // Local time (time since the current track began)
  lttrk->trkTime=aTrack->GetLocalTime()/microsecond;
  lttrk->ke=aTrack->GetKineticEnergy()/electronvolt;
  lttrk->edep=aTrack->GetStep()->GetTotalEnergyDeposit()/electronvolt;
  lttrk->particleName = aTrack->GetDefinition()->GetParticleName();
  lttrk->preName = trackInformation->GetPreName();
  lttrk->postName = trackInformation->GetPostName();
  lttrk->nInToGe=trackInformation->GetInToGe();
  lttrk->nOutOfGe=trackInformation->GetOutOfGe();
  lttrk->nSpike=trackInformation->GetSpikeReflection();
  lttrk->boundaryName = trackInformation->GetBoundaryNameVector();//actually PreName, only filled if the OpticalBoundary is there (usally is)

  // add postion and energy history
  for(unsigned istep =0; istep<  trackInformation->GetPositionHistoryVector().size(); ++istep){
    G4ThreeVector pos = trackInformation->GetPositionHistory(istep);
    lttrk->addPositionHistory(pos.x(),pos.y(),pos.z());
  }
  for(unsigned istep =0; istep<  trackInformation->GetPositionEnergyVector().size(); ++istep){
    G4double posEnergy = trackInformation->GetPositionEnergy(istep)/electronvolt;  // convert to eV
    lttrk->positionEnergy.push_back(posEnergy);
  }
}
