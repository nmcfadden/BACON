//#include <TLorentzVector.h>
#include <TVector3.h>
#include <map>
#include <limits.h>

enum { SCINT,ABS,OTHER,OUT,CONVERT,BOUNDABS,ABSGEWLS,ABSGESCINT,WLS,PMT,LAST };


enum G4OpBoundaryProcessStatus {  Undefined,
                                  Transmission, FresnelRefraction,
                                  FresnelReflection, TotalInternalReflection,
                                  LambertianReflection, LobeReflection,
                                  SpikeReflection, BackScattering,
                                  Absorption, Detection, NotAtBoundary,
                                  SameMaterial, StepTooSmall, NoRINDEX,
                                  PolishedLumirrorAirReflection,
                                  PolishedLumirrorGlueReflection,
                                  PolishedAirReflection,
                                  PolishedTeflonAirReflection,
                                  PolishedTiOAirReflection,
                                  PolishedTyvekAirReflection,
                                  PolishedVM2000AirReflection,
                                  PolishedVM2000GlueReflection,
                                  EtchedLumirrorAirReflection,
                                  EtchedLumirrorGlueReflection,
                                  EtchedAirReflection,
                                  EtchedTeflonAirReflection,
                                  EtchedTiOAirReflection,
                                  EtchedTyvekAirReflection,
                                  EtchedVM2000AirReflection,
                                  EtchedVM2000GlueReflection,
                                  GroundLumirrorAirReflection,
                                  GroundLumirrorGlueReflection,
                                  GroundAirReflection,
                                  GroundTeflonAirReflection,
                                  GroundTiOAirReflection,
                                  GroundTyvekAirReflection,
                                  GroundVM2000AirReflection,
                                  GroundVM2000GlueReflection,
                                  Dichroic };

// max is 16 bits!
enum TrackStatus { active=1, scint=2, absorbed=4, boundaryAbsorbed=8,
                      absorbedLAr=16, inactive=32, hitWLS = 64, totalInternal=128, backScatter=256, fresnelRefract=512,
                      fresnelReflect = 2*fresnelRefract,
                      spikeReflect=2*fresnelReflect, 
                      hitPMT=2*spikeReflect, 
                      hitGe=2*hitPMT,
                      absGe=2*hitGe,
                      isBad=2*absGe};

double hc=1.23984193E+3; // eV-nm  KE unit is eV

TString bnames[Dichroic+1];

int setBorderNames() 
{
  int in=0;
  bnames[in++]="Undefined";   
  bnames[in++]="Transmission";  
  bnames[in++]="FresnelRefraction";
  bnames[in++]="FresnelReflection";
  bnames[in++]="TotalInternalReflection";
  bnames[in++]="LambertianReflection";
  bnames[in++]="LobeReflection";
  bnames[in++]="SpikeReflection";
  bnames[in++]="BackScattering";
  bnames[in++]="Absorption";
  bnames[in++]="Detection";
  bnames[in++]="NotAtBoundary";
  bnames[in++]="SameMaterial";
  bnames[in++]="StepTooSmall";
  bnames[in++]="NoRINDEX";
  bnames[in++]="PolishedLumirrorAirReflection";
  bnames[in++]="PolishedLumirrorGlueReflection";
  bnames[in++]="PolishedAirReflection";
  bnames[in++]="PolishedTeflonAirReflection";
  bnames[in++]="PolishedTiOAirReflection";
  bnames[in++]="PolishedTyvekAirReflection";
  bnames[in++]="PolishedVM2000AirReflection";
  bnames[in++]="PolishedVM2000GlueReflection";
  bnames[in++]="EtchedLumirrorAirReflection";
  bnames[in++]="EtchedLumirrorGlueReflection";
  bnames[in++]="EtchedAirReflection";
  bnames[in++]="EtchedTeflonAirReflection";
  bnames[in++]="EtchedTiOAirReflection";
  bnames[in++]="EtchedTyvekAirReflection";
  bnames[in++]="EtchedVM2000AirReflection";
  bnames[in++]="EtchedVM2000GlueReflection";
  bnames[in++]="GroundLumirrorAirReflection";
  bnames[in++]="GroundLumirrorGlueReflection";
  bnames[in++]="GroundAirReflection";
  bnames[in++]="GroundTeflonAirReflection";
  bnames[in++]="GroundTiOAirReflection";
  bnames[in++]="GroundTyvekAirReflection";
  bnames[in++]="GroundVM2000AirReflection";
  bnames[in++]="GroundVM2000GlueReflection";
  bnames[in++]="Dichroic";
  return in;
}
std::vector<std::string> trackStatusNames;
std::vector<int> trackStatusValue;


unsigned setTrackStatusNames()
{
  trackStatusValue.clear();
  trackStatusValue.push_back(active);
  trackStatusValue.push_back(scint);
  trackStatusValue.push_back(absorbed);
  trackStatusValue.push_back(boundaryAbsorbed);
  trackStatusValue.push_back(absorbedLAr);
  trackStatusValue.push_back(inactive);
  trackStatusValue.push_back(hitWLS);
  trackStatusValue.push_back(totalInternal);
  trackStatusValue.push_back(backScatter);
  trackStatusValue.push_back(fresnelRefract);
  trackStatusValue.push_back(fresnelReflect);
  trackStatusValue.push_back(spikeReflect);
  trackStatusValue.push_back(hitPMT);
  trackStatusValue.push_back(hitGe);
  trackStatusValue.push_back(absGe);
  trackStatusValue.push_back(isBad);

  trackStatusNames.clear();
  trackStatusNames.push_back("active");
  trackStatusNames.push_back("scint");
  trackStatusNames.push_back("absorbed");
  trackStatusNames.push_back("boundaryAbsorbed");
  trackStatusNames.push_back("absorbedLAr");
  trackStatusNames.push_back("inactive");
  trackStatusNames.push_back("hitWLS");
  trackStatusNames.push_back("totalInternal");
  trackStatusNames.push_back("backScatter");
  trackStatusNames.push_back("fresnelRefract");
  trackStatusNames.push_back("fresnelReflect");
  trackStatusNames.push_back("spikeReflect");
  trackStatusNames.push_back("hitPMT");
  trackStatusNames.push_back("hitGe");
  trackStatusNames.push_back("absGe");
  trackStatusNames.push_back("isBad");
  return  trackStatusNames.size();
}



void calcRatio(double a, double b, double &r, double &e) 
{
  r=0; e=0;
  if(b<1) return;
  r=a/b;
  if(a<1) return;
  // assume errors are root(n)A
  e = r * sqrt( 1./a + 1./b );
}

void printTrackStatus(int in,int status)
{
  printf("track status:  ");
  for (int i=0; i< in; ++i) if( status& (1<<i) )  printf("\t bit %i %s ",i,trackStatusNames[i].c_str());
  printf("\n");
}


void printTrackStatusNames()
{
  for (unsigned i=0; i< trackStatusNames.size() ; ++i) {
    printf(" bit %i hex val %X %s \n",i,trackStatusValue[i],trackStatusNames[i].c_str());
  }
}



void printBorderNames(int in)
{
  for (int i=0; i< in ; ++i) printf(" bit %i %s \n",i,bnames[i].Data());
}


void anaTrk(TString tag = "quartz1000"){//"2017-7-2-16-24-1"){//"2017-7-2-12-48-47"){//"2017-7-1-11-35-23"){
  int inames =setBorderNames();
  //printBorderNames(inames);
  unsigned itnames = setTrackStatusNames();
  printTrackStatusNames();
  //local file
  TString dir = TString("/home/nmcfadde/doubleBeta/RooT/");
  //TString dir = TString("/data1/nmcfadde/BACon/");
  TString inputFileName = dir+TString("BAConTree-")+tag+TString(".root");
  
  printf(" opening file %s \n",inputFileName.Data()); 
  
  TFile *infile = new TFile(inputFileName,"READONLY");
  TTree *trkTree = NULL;
  unsigned aSize = 0;

  bool GeOnly = true;
  int treeSwitch = -1;
  // tree has to be in file
  if(treeSwitch == 0)
    infile->GetObject("geTree",trkTree);
  else if(treeSwitch == 1)
    infile->GetObject("trkTree",trkTree);
  else
    infile->GetObject("noOptTree",trkTree);

  if(!trkTree) {
    printf(" trkTree not found! \n");
    return;
  }
  if(trkTree) aSize=trkTree->GetEntriesFast();
  printf(" trkTree with %i entries \n",int(aSize));

  LTTrack *trk = new LTTrack();
  if(treeSwitch == 0)
    trkTree->SetBranchAddress("geTrk",&trk);
  else if (treeSwitch == 1)
    trkTree->SetBranchAddress("track",&trk);
  else
    trkTree->SetBranchAddress("noOptTree",&trk);

  if(aSize==0) return;

  // open ouput file and make some histograms
  TString outputFileName = TString("anaTrk-")+tag+TString(".root");
  TFile *outfile = new TFile(outputFileName,"recreate");
  printf(" opening output file %s \n",outputFileName.Data());
  // add histograms here
  double rmax= 160;
  double zmax= 160;

  double xoffset = 200;  // shift xrange -350,50 to -150,+150 
  double yoffset = 0;
  double zoffset = 50;

  double xvoffset = 200;  // shift xrange -350,50 to -150,+150 
  double yvoffset = 0;
  double zvoffset = 50;


  TH1::SetDefaultSumw2(true); // turn on error bars 

  TNtuple *ntele = new TNtuple("ntele","ntele","ke:edep");
  TNtuple *ntopt = new TNtuple("ntopt","ntopt","rvert:zvert:parent:rlog:rtrk:ztrk:lambda:length:time:fstat:lar:status:boundary");
  TNtuple *ntpos = new TNtuple("ntpos","start-end","rstart:xstart:ystart:zstart:rend:xend:yend:zend:wstart:length:fstat:lar:boundary");
  //TNtuple *ntposalpha = new TNtuple("ntposalpha","start-end","rvert:zvert:rstart:zstart:xstart:ystart:rend:zend:xend:yend");
  TNtuple * ntGamma = new TNtuple("ntgamma","ntgamma","edep:ke:x:y:z");
  TNtuple * ntalphaStepE = new TNtuple("ntalphaStep","ntalphaStep","ke:length:step:edep:dEdx:EdepTot:maxKE:x:y:z");
  TNtuple * ntalphaTrackE = new TNtuple("ntalphaTrack","ntalphaTrack","ETotAlpha:minKE:TotalE:length");

  TH1F *hGammaStepLength =  new TH1F("GammaStepLength"," Gamma step length (mm) ",2500,0,0.2);
  TH1F *hEStepGamma =  new TH1F("EStepGamma"," Gamma dE/dx",1000,0,20); 
  TH1F *hGammaMaxKE = new TH1F("hGammaMaxKE","Gamma Intial KE",100,50,3000);
  TH1F *hETotGamma = new TH1F("ETotGamma","Total Energy Deposited for gamma (keV)",100,0,60);
  TH1F *hGammaTrackEdep = new TH1F("hGammaTrackEdep","Total Energy Deposited per track Gamma",100,50,3000);
  
  TH1F *hElectronStepLength =  new TH1F("ElectronStepLength"," e- step length (mm) ",1000,0,0.2);
  TH1F *hEStepElectron =  new TH1F("EStepElectron"," e- step weighted by ke ",100,0,3000); 
  hEStepElectron->SetXTitle(" length (mm) "); 
  hEStepElectron->SetYTitle(" dE/dlength keV/um "); 
  TH1F *hElecMaxKE = new TH1F("hElecMaxKE","Intial KE Electron",100,50,3000);
  TH1F *hETotElec = new TH1F("ETotElec","Total Energy Deposited for e- (keV)",1000,0,6000);
  TH1F *hElecTrackEdep= new TH1F("hElecTrackEdep","Total Energy Depsited per Track electron",100,50,3000);
  
  TH1F *hEStepOptical  =  new TH1F("EStepOptical","  optical step weighted by ke ",1000,0,1000);
  hEStepOptical->SetXTitle(" length (mm) "); 
  hEStepOptical->SetYTitle(" dE/dlength eV/mm ");
  TH1F *hEopticalTrack = new TH1F("hEopticalTrack","Edep per track for optical photons (eV)",100,0,20);
  TH1F *hEopticalTrackScint = new TH1F("hEopticalTrackScint","Number of Scint photons",100,1,500000);
  TH1F *hEopticalTrackWLS = new TH1F("hEopticalTrackWLS","Number of WLS photons",100,1,5000);
  TH1F *hEopticalTrackPMT = new TH1F("hEopticalTrackPMT","Number of PMT photons",100,1,150);

  TH1F *hPMTTimingLocal = new TH1F("hPMTTimingLocal","Timing of Detected Photons Local (since track began)",500,0,.10);//0-10 nanoseconds
  TH1F *hPMTTimingGlobalLong = new TH1F("hPMTTimingGlobalong","Timing of Detected Photons(global)",200,500,10000);//500-2000 ns
  TH1F *hPMTTimingGlobalShort = new TH1F("hPMTTimingGlobashort","Timing of Detected Photons(global)",200,0,50);//0-500 ns
  
  TH1F* hEStepAlpha = new TH1F("AlphaStepEloss","alpha edep/step versus track legnth ", 1000,0,22);
  hEStepAlpha->SetXTitle( "length (um) ");
  hEStepAlpha->SetYTitle(" dE/dx (keV/um)");
  TH1F *hAlphaStepLength = new TH1F("AlphaStepLength"," alpha step length (um) ",100,0,100);
  TH1F *hEdepGoAlpha = new TH1F("AlphaStepEloss_noZeroKE","alpha step deposited energy nonzero KE",100,0,25);
  hEdepGoAlpha->SetXTitle( "length (um) ");
  hEdepGoAlpha->SetYTitle(" dE/dx (keV/um)");
  TH1F *hEdepStopAlpha = new TH1F("AlphaStepEloss_ZeroKE","alpha step deposited energy ZERO KE",100,0,25);
  hEdepStopAlpha->SetXTitle( "length (um) ");
  hEdepStopAlpha->SetYTitle(" dE/dx (keV/um)");
  TH1F *hETotAlpha = new TH1F("ETotAlpha","Total Energy (keV) deposited by Alpha in Ge",100,100,5500);
  TH1F *hAlphaStepKE = new TH1F("AlphaStepKE","Kinetic Energy (keV) of Alpha by step",100,0,5500);
  TH1F *hAlphaMaxKE = new TH1F("AlphaMaxKE","Max KE of Alpha (keV)",100, 5000,9000);
//  TH1F *hAlphaMinKE = new TH1F("AlphaMinKE","Min KE (KE > 0) of Alpha (keV)",100, 0,2000);
//  hAlphaMinKE->SetXTitle("Minimum Kinetic Energy (keV)");
  TH1F *hAlphadEdxKE = new TH1F("AlphadEdxKE","dE/dKE for Alpha (keV)",100,0,5500);
  hAlphadEdxKE->SetXTitle("Kinetic Energy (keV)");
  hAlphadEdxKE->SetYTitle("dE/dKE");
  TH1F *hAlphaTrackEdep = new TH1F("hAlphaTrackEdep","Total Energy deposited per track Alpha",100,50,10000);
  TH1F *hTotalEnergyDepsited = new TH1F("hTotalEnergyDepsited","TotalEnergyDepsited",1000,50,3000);
  hTotalEnergyDepsited->SetXTitle("Total Energy Deposited (kev)");

  TH1F *hEnergySpec = new TH1F("hEnergySpec","Total Energy Deposited in Ge fill for every track",1000,50,3000);

  //Neils getting fancy with those MAPS!
  std::map<Int_t,LTTrack*> trackMap;
  std::map<TString,LTTrack*> particleMap;

  std::vector< std::map<int,LTTrack*> > trackVector;
  
  Double_t alphaCounter = 0,elecCounter = 0,gammaCounter = 0,optCounter = 0,eventCounter = 0;
  Double_t MaxKEAve = 0,maxKE = 0,MinKEAve = 0,minKE = 0,elecKEcounter = 0,preStepKE = 0,PrevKE = 0;
  Double_t ETotElec = 0, ETotGamma = 0, ETotAlpha = 0, ETotOpt = 0,EToTother = 0;
  Double_t maxKEGamma = 0,maxKEElec = 0,maxKEEAlpha = 0;
  Double_t TotalEnergyDeposited = 0;
  Double_t ETotElecAve = 0, ETotGammaAve = 0, ETotAlphaAve = 0, ETotOptAve = 0, ETotOtherAve = 0;
  Double_t TotalEnergyDepositedAve = 0;
  Double_t lengthAlpha = 0;
  Int_t currentevId = -1,prevTrackIdGamma = -1, prevTrackIdElec = -1, prevTrackIdAlpha = -1 ;
  bool optSwitch = false;

  std::vector<int> trkVec,pmtHITvector(1);
  std::vector<Double_t> dEdxVec(hEStepAlpha->GetNbinsX()),lengthVec;

  double sumwElectron=0;
  double sumwOptical=0;
  double fanoLAr = 0.1;
double time0 = 0, timeTemp = -1;
  int barray[8]; // 64 bits
  for(unsigned entry =0; entry < aSize  ; ++entry ) {
    trkTree->GetEntry(entry);
    
    TString name = trk->particleName;
    if(name == "Co60" || entry == (aSize - 1) ){
        if(currentevId > -1){
          cout<<"Found parentId = "<<trk->parentId<<" with particleName "<<name<<"...Event "<<currentevId<<" processed "<<
            "ke "<<trk->ke<<" time "<<trk->time<<" trkid "<<trk->trkId<<endl;
          if(currentevId > -1 ) trackVector.push_back(trackMap);
        }
        currentevId++;//intialized to -1
        trackMap.clear();
        trkVec.clear();
        pmtHITvector.push_back(0);
        time0 = 0;
    }

   

    if(trk->status&inactive)
      continue;
    bool pmtHit = trk->status&hitPMT;
    //if(!pmtHit) continue;
    if(name != "opticalphoton" ){
      //continue;
    }
    bool newTrk = true;
    for(int i = 0; i < trkVec.size() ; i++){
      if(trkVec[i] == trk->trkId){
        newTrk = false;
      }
    }
    trkVec.push_back(trk->trkId);
    
    std::map<int,LTTrack*>::iterator it;
    it = trackMap.find(trk->trkId);
    
    if(newTrk){
      LTTrack *particleTrk = new LTTrack();
      particleTrk->particleName = name;
      particleTrk->parentId = trk->parentId;
      particleTrk->ke = trk->ke;
      particleTrk->edep = trk->edep;
      particleTrk->trkId = trk->trkId;
      particleTrk->stepKE = trk->stepKE;
      particleTrk->trkTime = trk->trkTime;
      particleTrk->time = trk->time;
      particleTrk->status = trk->status;
      particleTrk->process = trk->process;
      particleTrk->positionEnergy = trk->positionEnergy;
      particleTrk->boundaryStatus.resize(trk->positionEnergy.size(),trk->status);
      //particleTrk->boundaryStatus.push_back(trk->status);
      particleTrk->boundaryName = trk->boundaryName;
      particleTrk->postName = trk->postName;
      particleTrk->preName   = trk->preName;
      trackMap[trk->trkId] = particleTrk;
    } else{
      for(int i = 0; i < trk->stepKE.size() ; i++){
        it->second->stepKE.push_back( trk->stepKE[i] );
        it->second->boundaryStatus.push_back(trk->status);
      }
      for(int i = 0; i < trk->positionEnergy.size() ; i++){
        it->second->positionEnergy.push_back( trk->positionEnergy[i] );
      }
      for(int i = 0; i < trk->boundaryName.size() ; i++){
        it->second->boundaryName.push_back(trk->boundaryName[i]);
      }
      it->second->postName = trk->postName;
      it->second->preName  = trk->preName;
    }
    /*
    cout<<"\t"<<trackMap.find(trk->trkId)->second->particleName<<", trkId "<<trackMap.find(trk->trkId)->second->trkId<<endl;
    for(int i = 0; i < trackMap.find(trk->trkId)->second->stepKE.size(); i++){
      cout<<"\t"<<trackMap.find(trk->trkId)->second->stepKE[i]<<endl;
    }
    */
  }

  for(int j = 0 ; j < trackVector.size() ; j++){
    cout<<"Analyzing event "<<j<<" out of "<<trackVector.size()-1<<" events."<<endl;
    std::map<Int_t,LTTrack*> map = trackVector[j];
    Int_t PMTHit = 0,WLSHit = 0,ScintHit = 0;
    std::vector<double> timeVec; double T0 = std::numeric_limits<double>::max();
    bool firstHit = false;
    for (auto& x: map){
     if(x.second->particleName == "opticalphoton"){
        //I don't think this is necessary
        //int obin = hEopticalTrack->FindBin(x.second->edep);
        if(x.second->edep > 0)
          hEopticalTrack->Fill(x.second->edep);
          
        if(x.second->status&hitPMT){
            if(T0> x.second->time) {
              T0 = x.second->time;
            }
            if(T0 == x.second->time){
              //cout<<x.second->particleName<<" "<<x.second->trkId;
              //printf("%10.10f \n",x.second->time);
            }

            timeVec.push_back(x.second->time);
            //printf("deltaT %3.5f \n",x.second->time-T0 + x.second->trkTime);
            //hPMTTimingGlobal->Fill(x.second->time-T0 + x.second->trkTime);//in um
            hPMTTimingLocal->Fill(x.second->trkTime*1000.0);//pmt timing in nanoseconds
            PMTHit++;

          }
        else if(x.second->status&scint){
          ScintHit++;
        }
        else if(x.second->status&hitWLS){
          WLSHit++;
        }
      }

    }//map loop (loop over tracks in current event)
    //cout<<timeVec.size()<<"   "<<T0<<endl;
    for(int i = 0; i < timeVec.size() ; i++){
      double localtime = (timeVec[i] - T0)*1000.0;//converting to nano from micro seocnds
      //printf("deltaT %3.10f T0 %10.10f timVec %10.10f \n",localtime,T0,timeVec[i]);
      if(localtime > 500.)
        hPMTTimingGlobalLong->Fill(localtime);
      else
        hPMTTimingGlobalShort->Fill(localtime);
    }
    
    hEopticalTrackScint->Fill(ScintHit);
    hEopticalTrackWLS->Fill(WLSHit);
    hEopticalTrackPMT->Fill(PMTHit);
  }//vector loop (loop over events in run)

TCanvas* chPMTTimingGlobal = new TCanvas("hPMTTiming Long","hPMTTiming Long");
//TF1 *func = new TF1("func","expo+expo",0,100);
TF1 * func = new TF1("func","expo(0)+expo(1)",0,20);
hPMTTimingGlobalLong->Fit("expo");
hPMTTimingGlobalLong->Draw();



TCanvas* chPMTTiminglocal = new TCanvas("hPMTTiming Short","hPMTTiming Short");
hPMTTimingGlobalShort->Fit("expo");
hPMTTimingGlobalShort->Draw();
  
outfile->Write();
/*
std::map<int,LTTrack*>::iterator it;
            it = map.find(x.second->parentId);
            if(it == map.end()) continue;
            //this will be null if parent is not optical photon, for example Cerenkov from e- creates photon at 380 nm
            if(it->second){
              hPMTTimingGlobal->Fill(it->second->trkTime*1000.0);//in nanoseconds
              hPMTTimingLocal->Fill(x.second->trkTime*1000.0);//pmt timing in nanoseconds
              PMTHit++;
            }

*/
}
