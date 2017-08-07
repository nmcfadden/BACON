/// M.G. started with exgps example 
// $Id: HistoManager.cc 83882 2014-09-22 11:09:30Z maire $
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "LegendAnalysis.hh"
#include "LegendTrajectory.hh"
#include "G4RunManager.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "UserTrackInformation.hh"
#include "G4VProcess.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "UserEventInformation.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
LegendAnalysis* LegendAnalysis::fLegendAnalysis=NULL;

LegendAnalysis* LegendAnalysis::Instance() {
  if (! fLegendAnalysis) fLegendAnalysis = new LegendAnalysis();
  return fLegendAnalysis;
}

void LegendAnalysis::Initialize()
{
 // open new ouput file with time stamp.
  time_t tnow;
  time(&tnow);

  struct tm* timeinfo;
  timeinfo = localtime(&tnow);

  char chtime[80];
  sprintf(chtime,"%u-%u-%u-%u-%u-%u",timeinfo->tm_year-100+2000,timeinfo->tm_mon,timeinfo->tm_mday,
      timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
  G4String fTreeFileName = G4String("RooT/BAConTree-") + G4String(chtime) + G4String(".root");
  fTreeFile=new TFile(fTreeFileName.data(),"RECREATE");
  G4String fHistFileName = G4String("RooT/BAConHist-") + G4String(chtime) + G4String(".root");
  fHistFile=new TFile(fHistFileName.data(),"RECREATE");


  G4String gmess= G4String(" LegendAnalysis: ************  output tree file  ") + fTreeFileName + G4String(" hist file ") + fHistFileName + G4String(" ************ ");
  G4cout << gmess << G4endl;
  fHistFile->cd();
  hOptical = new TH1F("Optical"," optical photons (nm) ",900,100,1000);
  hOptical->GetYaxis()->SetTitle(" photons/nm ");
  hOptical->GetXaxis()->SetTitle(" wavelength (nm) ");

  hWls = new TH1F("Wls"," Wls photons (nm) ",900,100,1000);
  hWls->GetYaxis()->SetTitle(" photons/nm ");
  hWls->GetXaxis()->SetTitle(" wavelength (nm) ");
  

  hPmtHits = new TH1F("PmtHits"," pmt photons (nm) ",900,100,1000);
  hPmtHits->GetYaxis()->SetTitle(" photons/nm ");
  hPmtHits->GetXaxis()->SetTitle(" wavelength (nm) ");
  

  hEElectron = new TH1F("EElectron"," electrons ",1000,0,1000);
  hEElectron->GetYaxis()->SetTitle(" electrons/KeV ");
  hEElectron->GetXaxis()->SetTitle(" energy (KeV) ");
  
  hEGamma = new TH1F("EGamma"," gammas ",1000,0,1000);
  hEGamma->GetYaxis()->SetTitle(" gamma/KeV ");
  hEGamma->GetXaxis()->SetTitle(" energy (KeV) ");

  hScintYield = new TH1F("ScintYield"," scint yield photons/kev ",1000,0,100);
  hScintYield->GetYaxis()->SetTitle("  events ");
  hScintYield->GetXaxis()->SetTitle("  photons/kev ");

  hWlsYield = new TH1F("WlsYield"," wls yield photons/kev ",1000,0,100);
  hWlsYield->GetYaxis()->SetTitle("  events ");
  hWlsYield->GetXaxis()->SetTitle("  photons/kev ");


  hPmtHitCount = new TH1F("PmtHitCount"," photons detected per event  ",1000,0,1000);
  hPmtHitCount->GetYaxis()->SetTitle("  events ");
  hPmtHitCount->GetXaxis()->SetTitle("  photons detected  ");
  

  topHistDir()->ls();

  fTreeFile->cd();

  // make tree in output file
  fTree = new TTree("LTree","LTree");
  //fTree->SetMaxTreeSize(1000000);
  fTree->SetMaxTreeSize(25);
  fEvent = new LTEvent();
  fTree->Branch("event",&fEvent);
  topTreeDir()->ls();

}

void LegendAnalysis::printSummary() 
{
  G4cout << G4endl << " ************************************************************* " << G4endl;
  G4cout << " LegendAnalysis summary : event entries " << fTree->GetEntries() << G4endl;
  G4cout  << " ************************************************************* " << G4endl;

}
    
void  LegendAnalysis::anaEvent( const G4Event *anEvent)
{
  if(!anEvent) {
    G4cout << " LegendAnalysis called with NULL G4Event pointer!!! " << G4endl;
    return;
  }
  /*
  printf(" ++++++++++++++++++++++++++++++++++++++++++++++++ \n");
  printf("      LegendAnalysis:anaEvent called event %i  \n",anEvent->GetEventID());
  printf(" ++++++++++++++++++++++++++++++++++++++++++++++++ \n");
  */
  
  UserEventInformation* eventInformation = (UserEventInformation*)G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation();
  fEvent->evId = anEvent->GetEventID();
  fEvent->nPVert = anEvent->GetNumberOfPrimaryVertex();
  for(int iv=0; iv < fEvent->nPVert; ++iv) {
    G4PrimaryVertex* pvi = anEvent->GetPrimaryVertex(iv);
    LTPVertex lpv;
    G4VPhysicalVolume *physVol = eventInformation->GetPrimaryPhysicalVolume();
    
    lpv.physVolumeName = physVol->GetName();
    // fill primary vertex info
    lpv.VertexId=iv;
    lpv.Position.SetXYZT(pvi->GetX0(),pvi->GetY0(),pvi->GetZ0(),pvi->GetT0());
    G4ThreeVector pvrel = pvi->GetPosition() - physVol->GetTranslation();
    lpv.RelPosition.SetXYZT(pvrel.x(),pvrel.y(),pvrel.z(),pvi->GetT0());
    //G4VUserPrimaryVertexInformation* pvinfo= pvi->GetUserInformation() 
    lpv.nParticles =pvi->GetNumberOfParticle();
    for(int ip=0; ip< lpv.nParticles; ++ip ) {
      G4PrimaryParticle* pvPart = pvi->GetPrimary(ip); 
      LTParticle part;
      part.TrackId= pvPart->GetTrackID();
      part.VertexId=iv;
      part.PDG=     pvPart->GetPDGcode();
      part.Mass=    pvPart->GetMass();
      part.Charge=  pvPart->GetCharge();
      part.KEnergy= pvPart->GetKineticEnergy();
      part.Momentum.SetPxPyPzE( pvPart->GetPx(),pvPart->GetPy(),pvPart->GetPz(),pvPart->GetTotalEnergy());
      //part.print(ip);
      lpv.particle.push_back(part);
      if(iv==0&&part.TrackId==1) { 
        fEvent->PDG=part.PDG;
        fEvent->ePrimary = pvPart->GetKineticEnergy();
      }
    }
    //lpv.print(fEvent->evId);
    fEvent->pvertex.push_back(lpv);
  }
  // plot yields
  hScintYield->Fill( double(fEvent->nArScint) / (fEvent->ePrimary/keV) );
  hWlsYield->Fill( double(fEvent->nWlsScint) / (fEvent->ePrimary/keV) );

   //G4cout << " LegendAnalysis eprim " << fEvent->ePrimary/keV << " nscint " << fEvent->nArScint<< " nwls " <<fEvent->nWlsScint << 
   //  " sint yield " << double(fEvent->nArScint) / (fEvent->ePrimary/keV) << " wls yield " << double(fEvent->nWlsScint) / (fEvent->ePrimary/keV) << G4endl;
  anaTrajectories( anEvent->GetTrajectoryContainer());
  
  // and end of analysis save this event
  //fEvent->print();
  fTree->Fill();
  //printf(" +++++++++++++++++++ Leaving Legend Analysis +++++++++++++++++++++++++++++ \n");
}
   
void  LegendAnalysis::anaTrajectories(G4TrajectoryContainer* trajectoryContainer)
{
  if(trajectoryContainer==NULL) {
    G4cout << " LegendAnalysis:: anaTrajectories called with null container " << G4endl;
    return;
  }
  fEvent->nTraj = trajectoryContainer->entries();
  // loop over trajectories 
  int hitCount=0;
  G4cout << " LegendAnalysis:: anaTrajectories ntraj= "<< fEvent->nTraj  << G4endl;
  for(int ij=0; ij < fEvent->nTraj; ++ij) {
    LegendTrajectory *gtrj = dynamic_cast<LegendTrajectory*>((*(trajectoryContainer))[ij]);
    if(!gtrj) {
      G4cout << "  LegendAnalysis:: NULL trajectory  " << ij << " which is impossible!! "  << endl;
      continue;
    }
    //G4cout << " LegendAnalysis:: track " << ij << " named " << gtrj->GetParticleName() << G4endl;
    
    const G4Track* aTrack= gtrj->GetTrack();
    if(!aTrack) {
      G4cout << "  LegendAnalysis:: no track for this trajectory  " << ij << endl;
      continue;
    }
  
    // start filling here
    LTTraject ltraj;
    // fill track info 
    // Hereafter we call current volume the volume where the step has just gone through. Geometrical informations are available from preStepPoint.
    G4StepPoint* currentPoint = aTrack->GetStep()->GetPreStepPoint();
    G4StepPoint* nextPoint    = aTrack->GetStep()->GetPostStepPoint();
    //G4VTouchable and its derivates keep these geometrical informations. We retrieve a touchable by creating a handle for it:
    G4TouchableHandle ctouch = currentPoint->GetTouchableHandle();
    if(!ctouch) 
      G4cout << " LegendAnalysis:: no touch handle  " << aTrack->GetDefinition()->GetParticleName()<< "  " << endl;
    
    ltraj.physVolName = ctouch->GetVolume()->GetName();
    ltraj.copy = ctouch->GetCopyNumber();
    ltraj.evId = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    ltraj.trkId = aTrack->GetTrackID();
    ltraj.trkParentId = aTrack->GetParentID();
    ltraj.trkLength=aTrack->GetTrackLength();
    ltraj.trkStep=aTrack->GetCurrentStepNumber();
    // G4ThreeVector trkPos = aTrack->GetPosition();
    // Global time (time since the current event began)
    ltraj.time=aTrack->GetGlobalTime()/microsecond;
    // Local time (time since the current track began)
    ltraj.trkTime=aTrack->GetLocalTime()/microsecond;
    ltraj.ke=aTrack->GetKineticEnergy()/electronvolt;
    ltraj.trkEdep=aTrack->GetStep()->GetTotalEnergyDeposit()/electronvolt;
    ltraj.name = aTrack->GetDefinition()->GetParticleName();
    
    // find creator process 
    const G4VProcess* creator=aTrack->GetCreatorProcess();
    if(!creator) 
      G4cout << " LegendAnalysis:: track definition is  " << aTrack->GetDefinition()->GetParticleName()<< " no creator " << endl;

   // fill from trajectory
    ltraj.trkStatus = gtrj->GetTrackStatus();
    ltraj.trajId = gtrj->GetTrackID() ;  
    ltraj.parentId = gtrj->GetParentID();        
    //PrimaryId = gtrj-> ;        
    ltraj.PDG = gtrj->GetPDGEncoding();       
    //Mass = gtrj-> ;   
    ltraj.charge = gtrj->GetCharge(); 

    // each element in std::vector corresponds to a point on the particle path
    // typdef CLHEP::Hep3Vector G4ThreeVector;
    G4ThreeVector gpos0 = gtrj->GetPoint(0)->GetPosition();
    TLorentzVector r4(gpos0.x(),gpos0.y(),gpos0.z(),0);// were is the time?
    
    // G4int npoints = gtrj->GetPointEntries();
    /*for(G4int ip=0; ip<npoints ; ++ip ) {
      G4VTrajectoryPoint* gtrjp = gtrj->GetPoint(ip);
      G4ThreeVector GetPosition() 
    }*/

    ltraj.position.push_back(r4);
    G4ThreeVector momentum3 = gtrj->GetInitialMomentum();
    TVector3 p3(momentum3.x(),momentum3.y(),momentum3.z());
    ltraj.ke = p3.Mag();// don't have mass
    ltraj.momentum.push_back(p3);

    // set primary 
    if(gtrj->IsPrimary()) {
      ltraj.type = LTTrajectType::PRI;
    }

    //G4cout <<  " ANALYSIS id " <<   ltraj.trkId  << " parent "  << ltraj.parentId << " PDG "  <<  ltraj.PDG << " charge " << ltraj.charge << G4endl;
    if(ltraj.charge==0) {
      if(gtrj->GetParticleName()=="opticalphoton") fEvent->eOptical += ltraj.ke;
      else fEvent->eNeutral += ltraj.ke;
    } else {
      //gtrj->ShowTrajectory();
      fEvent->eCharged += ltraj.ke;
    }

    //G4cout << " ANALYSIS E charged " << fEvent->eCharged << " neutral  " <<  fEvent->eNeutral << " optical  " << fEvent->eOptical << G4endl;
    
    ltraj.name = TString(gtrj->GetParticleName().data());

    //G4cout << " particle name " << ltraj.name << G4endl;

    //enum LTTrajectType {UNK,PRI,SCI,WLS,HIT};
    ltraj.type = LTTrajectType::UNK;

    if(gtrj->IsGeHit()) {
      ltraj.type = LTTrajectType::GEHIT;
      ++fEvent->nTrajGeHits;
      //something is wrong with this, gtrj is not correct sometimes 
      //fEvent->GeTrackLength += (gtrj->GetTrack()->GetTrackLength()/mm );
      //cannot access energy from Tracks, slipping into GermanSD.cc
      //fEvent->eGe += (gtrj->GetTrack()->GetStep()->GetTotalEnergyDeposit() / keV );
      //G4cout<<"LegendAnalysis::GermaniumTrajectories...eGe "<<fEvent->eGe<<", stepE "<<(gtrj->GetTrack()->GetStep()->GetTotalEnergyDeposit() / keV )<<G4endl;
    }

    if(ltraj.PDG==11) { // electron
      hEElectron->Fill(ltraj.ke/keV);
    } else if(ltraj.PDG==22) { // gamma 
      hEGamma ->Fill(ltraj.ke/keV);
    }
    
    if(gtrj->GetParticleName()==G4String("opticalphoton") ) { 
      ++fEvent->nTrajOptPhotons;
      G4double waveLength =  h_Planck*c_light/ltraj.ke/nm;//700 nm
      if(gtrj->IsPmtHit()) {
        ++fEvent->nTrajPmtHits;
        fEvent->ePmt += ltraj.ke;
        hPmtHits->Fill(waveLength);
        ltraj.type = LTTrajectType::PMTHIT;
        ++hitCount;
      } else if(gtrj->IsWLS()) {
        ++fEvent->nTrajWlsScint;
        hWls->Fill(waveLength);
        ltraj.type = LTTrajectType::WLS;
        //G4cout<<"LegendAnalysis::WLSTrajectories"<<G4endl;
      } else {
        ++fEvent->nTrajArScint;
        hOptical->Fill(waveLength);
        ltraj.type = LTTrajectType::SCI;
        //G4cout<<"LegendAnalysis::ArScintTrajectories"<<G4endl;
      }
    } // end loop over optical

    // save in tree
    fEvent->traject.push_back(ltraj);
  } // loop over trajectories
  hPmtHitCount->Fill(double(hitCount));
  //G4cout << " Legend analaysis nTraj = " << fEvent->nTraj << " hit count= " << hitCount << G4endl; 
}
