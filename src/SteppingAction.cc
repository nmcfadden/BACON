//
// ********************************************************************
// * License and Disclaimer                                           *
/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class

#include "SteppingAction.hh"
#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "PMTSD.hh"
#include "GermaniumSD.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"

#include "UserEventInformation.hh"
#include "UserTrackInformation.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction* det, EventAction* evt)
:detector(det), eventaction(evt)
{ 
   GeDebug = false;
  // create directory 
  fDir = LegendAnalysis::Instance()->topHistDir()->mkdir("step");
  fDir->cd();
  G4cout<<" StepAction working root directory  is  " << G4endl;  
  gDirectory->pwd();
  G4cout << " ... " << G4endl;
  hBoundaryStatus = new TH1F("StepBoundaryStatus"," boundary status ",Dichroic,0,Dichroic); // last in enum G4OpBoundaryProcessStatus
  hParticleType = new TH1F("StepParticleType"," step particle type ",100,0,100);

  // must be in top directory for ChangeFile to work
  LegendAnalysis::Instance()->topTreeDir()->cd();
  ntStep = new TNtuple("ntStep"," step variables ","ev:parent:pdg:status:tglobal:tlocal:length:energy");
  //ntGeStep = new TNtuple("ntGeStep"," step variables ","num:pdg:length:energy");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{

  LegendAnalysis::Instance()->getHistFile();
  // get volume of the current step
  G4StepPoint* preStepPoint = step->GetPreStepPoint();
  G4double length = 					step->GetStepLength();
  G4double edep =             step->GetTotalEnergyDeposit();
  G4Track* aTrack = 					step->GetTrack();
  G4double KE = aTrack->GetKineticEnergy();
  G4String particleName = aTrack->GetDefinition()->GetParticleName();
  //G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();
  G4TouchableHistory* theTouchable = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
  G4VPhysicalVolume* volume = theTouchable->GetVolume();
  G4String volumename = 			volume->GetName();
  G4ThreeVector pos = 				aTrack->GetPosition();
  G4ThreeVector dir = 				aTrack->GetMomentumDirection();
  G4ParticleDefinition* particleType = aTrack->GetDefinition();
  //Used to find other non optical processes
  const G4VProcess * process = aTrack->GetCreatorProcess();
  G4String processName;
  if(process) processName = process->GetProcessName();


  

  /*************************************
  ** from LegendStepping Action 
  *************************************/
  if ( aTrack->GetCurrentStepNumber() == 1 ) fExpectedNextStatus = Undefined;
  UserTrackInformation* trackInformation = dynamic_cast<UserTrackInformation*>(aTrack->GetUserInformation());
  UserEventInformation* eventInformation = dynamic_cast<UserEventInformation*>(G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());

  G4StepPoint* thePrePoint = step->GetPreStepPoint();
  G4VPhysicalVolume* thePrePV = thePrePoint->GetPhysicalVolume();
  

  G4StepPoint* thePostPoint = step->GetPostStepPoint();
  trackInformation->AddPostStepStatus(thePostPoint->GetStepStatus());
  G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();
 
  if(!thePostPV){//out of the world
    G4cout<<"SteppingAction:: WARNING Primary Vertex is out of this world \n\t Particle name "<<particleName<<", Ending Stepping Action!"<<G4endl;
    trackInformation->AddTrackStatusFlag(inactive);
    fExpectedNextStatus=Undefined;
    return;
  }

  // determine if step is at Ge boundary
  G4String preName = thePrePV->GetName();
  G4String postName = thePostPV->GetName();
  G4bool isPreGroup1   =  preName.contains("group1Physical") && thePrePoint->GetStepStatus() == fGeomBoundary;
  G4bool isPostGroup1  =  postName.contains("group1Physical") && thePostPoint->GetStepStatus() == fGeomBoundary;
  G4bool isPreGe  = (preName.contains("B8")||preName.contains("P4")) && thePrePoint->GetStepStatus() == fGeomBoundary;
  G4bool isPostGe = (postName.contains("B8")||postName.contains("P4")) && thePostPoint->GetStepStatus() == fGeomBoundary;
  G4bool isInToGe = isPreGroup1&&isPostGe;
  G4bool isOutOfGe = isPreGe&&isPostGroup1;
 
  //This is a primary track 
  // did we miss any secondaries from the primary track?
  trackInformation->SetParentId(aTrack->GetParentID());
  trackInformation->SetProcessName(processName);
  trackInformation->SetPreName(thePrePV->GetName());
  trackInformation->SetPostName(thePostPV->GetName());
  if(aTrack->GetParentID()==0){
    //G4cout<<"SteppingAction::Primary Vertex found "<<G4endl;
    trackInformation->SetPrimary();
    eventInformation->SetPrimaryPhysicalVolume(thePostPV);
    G4TrackVector* fSecondary = fpSteppingManager->GetfSecondary();
    G4int tN2ndariesTot = fpSteppingManager->GetfN2ndariesAtRestDoIt()
      + fpSteppingManager->GetfN2ndariesAlongStepDoIt()
      + fpSteppingManager->GetfN2ndariesPostStepDoIt();

    //If we havent already found the conversion position and there were
    //Loop over all 2ndaries that have not been found with N2ndariesTot
    if(!eventInformation->IsConvPosSet() && tN2ndariesTot>0 ){
      for(size_t lp1=(*fSecondary).size()-tN2ndariesTot; lp1<(*fSecondary).size(); lp1++){
        const G4VProcess* creator=(*fSecondary)[lp1]->GetCreatorProcess();
        if(creator){
          G4String creatorName=creator->GetProcessName();
          //Added Scint to list -Neil
          if(creatorName=="phot"||creatorName=="compt"||creatorName=="conv"||creatorName=="Scintillation"){
            //since this is happening before the secondary is being tracked
            //the Vertex position has not been set yet(set in initial step)
            //so set Conversion Position
            eventInformation->SetConvPos((*fSecondary)[lp1]->GetPosition());
          } //else if(!(creatorName=="eIoni"||creatorName=="eBrem")) G4cout << " SteppingAction unknown creatorName " << creatorName << G4endl;
        }
      }
    }
  }

  // find the optical boundary process only once
  // this is a list of all available processes 
  G4OpBoundaryProcessStatus boundaryStatus = Undefined;
  static G4ThreadLocal G4OpBoundaryProcess* boundary = NULL;

  if(!boundary){
      G4ProcessManager* pm = step->GetTrack()->GetDefinition()->GetProcessManager();
      G4int nprocesses = pm->GetProcessListLength();
      G4ProcessVector* pv = pm->GetProcessList();
     //G4cout << "  Stepping action looking for OpBoundary process " << G4endl;
     //for(G4int i = 0; i < nprocesses; i++) G4cout << "\t" << i << " process  " << (*pv)[i]->GetProcessName()<< G4endl ;
     for(G4int i = 0; i < nprocesses; i++){
      if((*pv)[i]->GetProcessName()=="OpBoundary" ){
        boundary = dynamic_cast<G4OpBoundaryProcess*>( (*pv)[i] );
        G4cout << "  Stepping action has found OpBoundary " << G4endl;
        break;
      }
    }
  }
  if(step == NULL){
    G4cout<<"NULL Step in steppingaction!"<<G4endl;
  }
  
  if(particleName == "gamma" && 0){
    G4cout <<  " SteppingAction::Gamma...KE = "<<KE/keV<<", edep "<<edep/keV<<", Pre/Post "<<preName<<"/"<<postName
      //", thePrePoint Process "<<thePrePoint->GetProcessDefinedStep()->GetProcessName()
      <<",trkID "<<aTrack->GetTrackID()
      <<", thePostPoint Process "<<thePostPoint->GetProcessDefinedStep()->GetProcessName()<<G4endl;
    const std::vector<const G4Track*>* trkVec = step->GetSecondaryInCurrentStep();
    for(size_t i =0; i < (*trkVec).size(); i++){
      G4String name = (*trkVec)[i]->GetDefinition()->GetParticleName();
      if(name == "opticalphoton") continue;
      G4double trkKE = (*trkVec)[i]->GetKineticEnergy();
      G4String procName = (*trkVec)[i]->GetCreatorProcess()->GetProcessName();
      G4int trkID = (*trkVec)[i]->GetTrackID();

      G4cout<<"\t Secondaries Name "<<name<<", creator Process Name = "<<procName<<", KE "<<trkKE/keV<<",trkID "<<trkID<<G4endl;
    }
  }
  if(particleName == "e-" && 0){
      G4cout <<  " SteppingAction::Electron...KE = "<<KE/keV<<", edep "<<edep/keV<<", Pre/Post "<<preName<<"/"<<postName
            //", thePrePoint Process "<<thePrePoint->GetProcessDefinedStep()->GetProcessName()
            <<",trkID "<<aTrack->GetTrackID()
            <<", thePostPoint Process "<<thePostPoint->GetProcessDefinedStep()->GetProcessName()<<G4endl;
      const std::vector<const G4Track*>* trkVec = step->GetSecondaryInCurrentStep();
      for(size_t i =0; i < (*trkVec).size(); i++){
        G4String name = (*trkVec)[i]->GetDefinition()->GetParticleName();
        if(name == "opticalphoton") continue;
        G4double trkKE = (*trkVec)[i]->GetKineticEnergy();
        G4String procName = (*trkVec)[i]->GetCreatorProcess()->GetProcessName();
        G4int trkID = (*trkVec)[i]->GetTrackID();

        G4cout<<"\t Secondaries Name "<<name<<", creator Process Name = "<<procName<<", KE "<<trkKE/keV<<",trkID "<<trkID<<G4endl;
      }
  }

  //If the pre-step volume name is a Ge Detector!
  G4bool  inGeDetector = false;
  //if (  (volumename.find("B8") != string::npos) ||(volumename.find("P4") != string::npos ) ) {
  //if the step is going into or out of the volume OR if the prestep is inside the volume then the track is in the Ge
  if ( isInToGe || isOutOfGe || (volumename.find("B8") != string::npos) ||(volumename.find("P4") != string::npos ) ) {
      inGeDetector = true;
  }

  // if we do not have a boundary, do nothing.
  if(!boundary) return;
  boundaryStatus=boundary->GetStatus();
  hBoundaryStatus->Fill(boundaryStatus);
  trackInformation->AddBoundaryProcessStatus(boundaryStatus);
  trackInformation->AddBoundaryName(preName);

  /*
  if(boundary->GetStatus()==NotAtBoundary) G4cout<<"SteppingAction:: NotAtBoundary" << processName << "  " 
    << particleType->GetParticleType() << "  " << preName << "  " << postName << " expect  " << boundary->GetStatus() << "  " << thePostPoint->GetStepStatus() << "=? " <<  fGeomBoundary << G4endl;
    */

  // add position and energy points of step to vectors.
  trackInformation->AddPositionHistory(aTrack->GetPosition());
  trackInformation->AddPositionEnergy(step->GetTotalEnergyDeposit());
  trackInformation->AddStepLength(step->GetStepLength());
  trackInformation->AddStepKE(step->GetTrack()->GetKineticEnergy());
  
 
  //Optical Photons
  /*
  G4cout<<"\t SteppingAction:: particleType " 
    << particleType->GetParticleName() 
    << "  type  " << particleType->GetParticleType() 
    << "  PDG " << particleType->GetPDGEncoding() << G4endl;
    */
  hParticleType->Fill(particleType->GetPDGEncoding());

  
  if(particleType==G4OpticalPhoton::OpticalPhotonDefinition()){


    //Kill photons exiting cryostat
    if(thePostPV->GetName()=="phy_World"){
      aTrack->SetTrackStatus(fStopAndKill);
      //eventInformation->IncPhotonCount_Escape();
      return;
    }

    //The photon was absorbed at another place other than a boundry
    if(thePostPoint->GetProcessDefinedStep()->GetProcessName()=="OpAbsorption"){
      aTrack->SetTrackStatus(fStopAndKill); // DO I NEED TO DO THIS BY HAND????
      trackInformation->AddTrackStatusFlag(absorbed);
      eventInformation->IncAbsorption();
      //if the photon was absorbed in LAr ProcessHit with name defined in DetectorConstruction 
      if(thePrePV->GetName()=="larPhysical"){
        trackInformation->AddTrackStatusFlag(absorbedLAr);
        /*G4SDManager* SDman = G4SDManager::GetSDMpointer();
        G4String sdName="ScintSD";
        LegendScintSD* ScintSD = (LegendScintSD*)SDman->FindSensitiveDetector(sdName);
        if(ScintSD){ 
          ScintSD->ProcessHits(step,NULL);
        }
        */
      }
    }
    
 
    //G4cout << " Stepping Action  " <<  boundaryStatus << " " << trackInformation->GetBoundaryProcessStatus() << G4endl;
    // G4cout << " Stepping geom boundary process " << boundaryStatus  << G4endl;
    //Check to see if the partcile was actually at a boundary
    //Otherwise the boundary status may not be valid
    //Prior to Geant4.6.0-p1 this would not have been enough to check
    /* enum G4OpBoundaryProcessStatus   Undefined,
                                  Transmission, FresnelRefraction,
                                  FresnelReflection, TotalInternalReflection,
                                  LambertianReflection, LobeReflection,
                                  SpikeReflection, BackScattering,
                                  Absorption, Detection, NotAtBoundary,
                                  SameMaterial, StepTooSmall, NoRINDEX,
      .... and more in G4OpBoundaryProcess.hh
    */
    if(thePostPoint->GetStepStatus()==fGeomBoundary){
      if(fExpectedNextStatus==StepTooSmall){
        if(boundaryStatus!=StepTooSmall){
          G4cout<< "SteppingAction::UserSteppingAction(): No reallocation step after reflection!"<<G4endl;          
          G4cout<<"SteppinAction:: thePrePV of Process is :: "<< thePrePV->GetName()<<G4endl;
          G4cout<<"SteppinAction:: thePostPV of Process is :: "<< thePostPV->GetName()<<G4endl;
          G4cout<<"\t    >>>>>>>>>> Something is wrong with the surface normal or geometry....Track is killed"<<G4endl;
          aTrack->SetTrackStatus(fStopAndKill);
        }
      }
      fExpectedNextStatus=Undefined;
      // case are evaluated if boundaryStatus == case 

      //if( isInToGe) G4cout<<"SteppingAction::  isInTo Ge geom boundary "<< thePrePV->GetName()<< "->" << thePostPV->GetName() << " status " << boundaryStatus << G4endl;
      //if( isOutOfGe) G4cout<<"SteppingAction::  isOutOf Ge geom boundary "<< thePrePV->GetName()<< "->"  << thePostPV->GetName() << " status "<< boundaryStatus << G4endl;

      if( isInToGe) trackInformation->IncInToGe();
      if( isOutOfGe) trackInformation->IncOutOfGe();
      // breaks not needed in case statement, but leaving in ... M.Gold
      G4double wavelength =  CLHEP::h_Planck*CLHEP::c_light/aTrack->GetKineticEnergy()/nm;
      switch(boundaryStatus){
        case Absorption: 
          {
            // optical photon absorbed in ge
            if (  (postName.find("B8") != string::npos) ||(postName.find("P4") != string::npos ) ) {
              trackInformation->AddTrackStatusFlag(absGe);
              //G4cout << " SteppingAction Ge absorbed photon in  " <<  volumename << " preName  " << preName << " postName " << postName <<  " wave = " 
              //  << wavelength  << G4endl; 
            }
            //This all Transportation
            trackInformation->AddTrackStatusFlag(boundaryAbsorbed);
            eventInformation->IncAbsorption();
            aTrack->SetTrackStatus(fStopAndKill);
            break;
          }
        case Detection:
          {
            aTrack->SetTrackStatus(fStopAndKill);
            // check if detector is PMT.
            if(thePostPV->GetName().find("PMT")!=std::string::npos) {
              trackInformation->AddTrackStatusFlag(hitPMT);
              //Triger sensitive detector manually since photon is absorbed but status was Detection
              //MG thinks Germanium will automatically be processed. 
              G4SDManager* SDman = G4SDManager::GetSDMpointer();
              G4String sdName="PhotoCathode";//"/LegendDet/pmtSD";
              PMTSD* pmtSD = dynamic_cast<PMTSD*>(SDman->FindSensitiveDetector(sdName));
              if(pmtSD) pmtSD->ProcessHits_constStep(step,NULL);
              else G4cout << " SteppingAction ERROR!!!!!   cannot find PhotoCathode " << G4endl;
            }
            // how does it get here with the filter??
            if (  (postName.find("B8") != string::npos) ||(postName.find("P4") != string::npos ) ) {
              trackInformation->AddTrackStatusFlag(absGe);
              //G4cout << " SteppingAction Ge Detection  photon in  " <<  volumename << " preName  " << preName << " postName " << postName <<  " wave = " 
              //  << wavelength  << G4endl; 
              }
            break;
          }
        case FresnelReflection:
          {
            //G4cout<<"SteppingAction::  FresnelReflection reflectivity = "<< boundary->GetReflectivity() << "  " << thePrePV->GetName()<< "->"  << thePostPV->GetName() << " status "<< boundaryStatus << G4endl;
            
            trackInformation->AddTrackStatusFlag(fresnelReflect);
            break;
          }
        case TotalInternalReflection:
          {
            trackInformation->AddTrackStatusFlag(totalInternal);
            break;
          }
        case LambertianReflection:
          break;
        case LobeReflection:
          break;
        case SpikeReflection:
          {
            trackInformation->AddTrackStatusFlag(spikeReflect);
            trackInformation->IncSpikeReflection();
            /*
              G4double LambdaE = 2.0*TMath::Pi()*1.973269602e-10 * mm * MeV;// in default Mev-mm units
              G4cout<<"SteppingAction::  SpikeReflection wave " << LambdaE/aTrack->GetKineticEnergy()/nanometer <<  " (nm)  reflectivity = "
              << boundary->GetReflectivity() << " angle " <<  boundary->GetIncidentAngle()<< " @ "  
              << thePrePV->GetName()<< "->"  << thePostPV->GetName() << " status "<< boundaryStatus << G4endl;
              */
            break;
          }
        case StepTooSmall:  // what to do here?gg
          break;
        case BackScattering:
          {
            trackInformation->IncReflections();
            fExpectedNextStatus= BackScattering;//: StepTooSmall; MG problem with VM2000? if StepTooSmall ???
            trackInformation->AddTrackStatusFlag(backScatter);
            break;
          }
        case FresnelRefraction:
          {
            trackInformation->AddTrackStatusFlag(fresnelRefract);
            break;
          }
        default:
          break;
      }


      // WLS is optical but doesnt seem to correspond to above boundary case
      if(processName == "OpWLS" ){ 
        trackInformation->AddTrackStatusFlag(hitWLS);
        // G4cout << " SteppingAction OpWLS boundary status = %i " << boundaryStatus << G4endl;
        // these tracks end up as "NotAtBoundary", so should be killed. 
        aTrack->SetTrackStatus(fStopAndKill); 
      } 
    }  //end of if(thePostPoint->GetStepStatus()==fGeomBoundary)
  } 
  /*else if(processName == "phot" ){ 
  } else if(processName == "compt"){
  } else if(processName == "eBrem"){
  } else if(processName == "conv"){
  } else if(processName == "Cerenkov"){
  } */



  // scint
  if(processName =="Scintillation") {
    trackInformation->AddTrackStatusFlag(scint);
  }
  
  // ionizing process
  if(processName == "eIoni" ) {   
    if(inGeDetector && GeDebug) G4cout<<"SteppingAction:: eIoni Process Name ... "<<processName<<" boundaryStatus " << boundaryStatus <<G4endl;
  }

  // ionizing process
  if(processName == "hIoni" ) {   
    if(inGeDetector && GeDebug) G4cout<<"SteppingAction:: hIoni Process Name ... "<<processName<<" boundaryStatus " << boundaryStatus <<G4endl;
  }

  // ionizing process
  if(processName == "ionIoni" ) {   
    if(inGeDetector && GeDebug) G4cout<<"SteppingAction:: ionIoni Process Name ... "<<processName<<" boundaryStatus " << boundaryStatus <<G4endl;
  }

  // compt process
  if(processName == "compt" ) {   
    if(inGeDetector && GeDebug) G4cout<<"SteppingAction:: compt Process Name ... "<<processName<<" boundaryStatus " << boundaryStatus <<G4endl;
  }

           
  if(inGeDetector){
    trackInformation->AddTrackStatusFlag(hitGe);
    if(particleName == "gamma"){
      G4cout<<"inGeDetector...ID "<<aTrack->GetTrackID()<<", PreNameV = "<<preName<<", PostNameV = "<<postName<<" KE = "<<KE/keV
        <<"Status"<<trackInformation->GetTrackStatus()<<G4endl;
    }
  }
  if(trackInformation->GetTrackStatus()&hitGe && GeDebug) G4cout << " SteppingAction hitGe...TrackStatus "<<trackInformation->GetTrackStatus() << G4endl;
  if(trackInformation->GetParentId() == 0)
    G4cout<<"SteppingAction KE = "<<KE/MeV<<" name "<<particleName<<endl;
  G4int eventId = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
//  ntStep->Fill(eventId,trackInformation->GetParentId(),particleType->GetPDGEncoding(),
  //    trackInformation->GetTrackStatus(),aTrack->GetGlobalTime()/microsecond,aTrack->GetLocalTime()/microsecond,length,aTrack->GetKineticEnergy());
}


