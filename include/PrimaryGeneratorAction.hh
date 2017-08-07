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
// $Id: PrimaryGeneratorAction.hh 68752 2013-04-05 10:23:47Z gcosmo $
//
/// \file optical/Legend/include/PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class
//
//
#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "LegendParticleSource.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:

    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();
    G4double pGun_nrg = 511*keV;
     
  public:

    void SetAngDistTypeByName(G4String name) {fParticleSource->SetAngDistTypeByName(name);}
    void SetPhysicalVolumeByName(G4String name) {fParticleSource->SetPhysicalVolumeByName(name);}
    void SetSourcePositionByName(G4String name) { fParticleSource->SetSourcePositionByName(name);}
    void SetEnergyDistTypeByName(G4String name) { fParticleSource->SetEnergyDistTypeByName(name);}
    void SetIon(G4double Z, G4double A, G4double E)             {fParticleSource->SetIon(Z,A,E); }
    void SetParticleByName(G4String name); 
    void Show() { fParticleSource->Show();}
    virtual void GeneratePrimaries(G4Event* anEvent);

  private:

    LegendParticleSource* fParticleSource;
    G4ParticleGun* fParticleGun;
};

#endif
