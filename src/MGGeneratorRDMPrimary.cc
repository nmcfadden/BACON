//---------------------------------------------------------------------------//
//bb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nubb0nu//
//                                                                           //
//                                                                           //
//                         MaGe Simulation                                   //
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
// $Id: MGGeneratorRDMPrimary.cc,v 1.2 2007-02-21 09:31:33 mgmarino Exp $ 
//      
// CLASS IMPLEMENTATION:  MGGeneratorPrimary.cc
//
//---------------------------------------------------------------------------//
/**
 * SPECIAL NOTES:
 * 
 *
 */
// 
//---------------------------------------------------------------------------//
/**
 * AUTHOR:  y. Chan
 * CONTACT: ydc@lbl.gov
 * FIRST SUBMISSION: Mon Apr 19 14:55:56 PDT 2004
 * 
 * REVISION:
 *
 * 06-11-2004, for interfacing to MGGeneratorRDM  
 *
 */
//---------------------------------------------------------------------------//
//

//---------------------------------------------------------------------------//

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Material.hh"
#include "Randomize.hh"

#include "generators/MGGeneratorRDMPrimary.hh"
#include "io/MGLogger.hh"

using namespace CLHEP;


class G4Event;

//---------------------------------------------------------------------------//

MGGeneratorRDMPrimary::MGGeneratorRDMPrimary()
{
  //Inititialize particle gun with RDM
  fDecayGun = new MGGeneratorRDMRadioactiveDecayGun();
   MGLog(trace) << "MGGeneratorRDM Created. " << endlog;
}

//---------------------------------------------------------------------------//

MGGeneratorRDMPrimary::MGGeneratorRDMPrimary(const MGGeneratorRDMPrimary & other) : G4VUserPrimaryGeneratorAction(other)
{
}

//---------------------------------------------------------------------------//

MGGeneratorRDMPrimary::~MGGeneratorRDMPrimary()
{
  G4cout<<"Yup"<<G4endl;
  //delete fParticleGun;
  G4cout<<"hey"<<G4endl;
  delete fDecayGun;
  G4cout<<"What did ti do"<<G4endl;
}

//---------------------------------------------------------------------------//

void MGGeneratorRDMPrimary::GeneratePrimaries(G4Event *anevent)
{
  /*
  //Set the nucleus at default position (0.,0.,0.)
  //fParticleGun->SetParticlePosition(G4ThreeVector(0.0*cm, 0.0*cm, 0.0*cm));
  //center of Group1Physical
  MGGeneratorRDMRadioactiveDecayGun* DecayGun = new MGGeneratorRDMRadioactiveDecayGun(); 
  MGGeneratorRDMNucleus ion(228,90,0);
  //MGGeneratorRDMNucleus ion(226,88,0);
  DecayGun->SetNucleus(ion);
  fParticleGun = DecayGun;
  fParticleGun->SetParticleEnergy(0.0);

  
  G4cout<<"Momentum Direction = "<<fParticleGun->GetParticleMomentumDirection()<<G4endl;
  G4cout<<"Momentum = "<<fParticleGun->GetParticleMomentum()<<G4endl;
  G4cout<<"Energy = "<<fParticleGun->GetParticleEnergy()<<G4endl;
  
  //1*cm away from surface of a random Ge detector in cylinder 1
  fParticleGun->SetParticlePosition(G4ThreeVector(-305.091*mm,12.4607*mm,-53.6964*mm ) );
  //1*mm away from surface of a random Ge detector in cylinder 1
  //fParticleGun->SetParticlePosition(G4ThreeVector(-162.639*mm,-73.3526*mm,-105.487*mm ) );
  //On surface of Random Ge 
  //fParticleGun->SetParticlePosition(G4ThreeVector(-159.603*mm,114.286*mm,-1.61391*mm) );
  //Center of Cylinder 1
  //fParticleGun->SetParticlePosition(G4ThreeVector(-205.166*millimeter,2.82928*millimeter,-55.0003*millimeter) ) ;
  */
  //fParticleGun->GeneratePrimaryVertex(anevent);

  MGGeneratorRDMNucleus ion(228,90,0);
  fDecayGun->SetNucleus(ion);
  fDecayGun->SetParticleEnergy(0.0);
  fDecayGun->SetParticlePosition(G4ThreeVector(-305.091*mm,12.4607*mm,-53.6964*mm ) );
  fDecayGun->GeneratePrimaryVertex(anevent);
G4cout<<"MGGeneratorRDMPrimary:: Primary Vertex Generated!"<<G4endl; 
}

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
