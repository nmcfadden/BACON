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
// $Id: LegendTrajectory.cc 72349 2013-07-16 12:13:16Z gcosmo $
//
/// \file optical/Legend/src/LegendTrajectory.cc
/// \brief Implementation of the LegendTrajectory class
//
//
#include "LegendTrajectory.hh"
#include "G4TrajectoryPoint.hh"
#include "G4Trajectory.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleTypes.hh"
#include "G4ThreeVector.hh"
#include "G4Polyline.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4VVisManager.hh"
#include "G4Polymarker.hh"

G4ThreadLocal G4Allocator<LegendTrajectory>* LegendTrajectoryAllocator = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LegendTrajectory::LegendTrajectory()
  :G4Trajectory(),fDrawit(false),fForceNoDraw(false),fForceDraw(false)
{
  fParticleDefinition=0;
  fTrack=NULL;
  fTrackStatus=0;
  fPrimary=false;
  positionRecord = new TrajectoryPointContainer();
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LegendTrajectory::LegendTrajectory(const G4Track* aTrack)
  :G4Trajectory(aTrack),fDrawit(false)
{
  fTrack=aTrack;
  fTrackStatus=0;
  fPrimary=false;
  fParticleDefinition=aTrack->GetDefinition();
  positionRecord = new TrajectoryPointContainer();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LegendTrajectory::LegendTrajectory(LegendTrajectory &right)
  :G4Trajectory(right),fDrawit(right.fDrawit)
{
  fParticleDefinition=right.fParticleDefinition;
  positionRecord = right.positionRecord;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LegendTrajectory::~LegendTrajectory() {}


void LegendTrajectory::DrawTrajectory() const
{
  // Consider using commands /vis/modeling/trajectories.
  
  //Taken from G4VTrajectory and modified to select colours based on particle
  //type and to selectively eliminate drawing of certain trajectories.

  //G4cout << " drawing force " << fForceDraw << " drawit "<< fDrawit  << "forcenodraw " << fForceNoDraw << G4endl;
  //if(!fForceDraw && (!fDrawit || fForceNoDraw))
  //  return;

  // not sure where ForceDraw, ForceNoDraw are being set
  if(!fDrawit) return;

 
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (!pVVisManager) return;
 
  G4double markerSize = 0.05; // std::abs(i_mode)/1000;  // 50/1000 = 0.05
  G4bool lineRequired (true);
  G4bool markersRequired (true);
 
  G4Polyline trajectoryLine;
  G4Polymarker stepPoints;
  G4Polymarker auxiliaryPoints;
 
  for (G4int i = 0; i < GetPointEntries() ; i++) {
    G4VTrajectoryPoint* aTrajectoryPoint = GetPoint(i);
    const std::vector<G4ThreeVector>* auxiliaries
      = aTrajectoryPoint->GetAuxiliaryPoints();
    if (auxiliaries) {
      for (size_t iAux = 0; iAux < auxiliaries->size(); ++iAux) {
        const G4ThreeVector pos((*auxiliaries)[iAux]);
        if (lineRequired) {
          trajectoryLine.push_back(pos);
        }
        if (markersRequired) {
          auxiliaryPoints.push_back(pos);
        }
      }
    }
    const G4ThreeVector pos(aTrajectoryPoint->GetPosition());
    if (lineRequired) {
      trajectoryLine.push_back(pos);
    }
    if (markersRequired) {
      stepPoints.push_back(pos);
    }
  }
 
  if (lineRequired) {
    G4Colour colour;
 
    if(fPrimary) {//primary
        G4cout << " LegendTrajectory Primary  "  << G4endl;
        colour = G4Colour::Red();
        //ShowTrajectory(); 
        markerSize = 3.0; 
    }
    
    if(fParticleDefinition==G4OpticalPhoton::OpticalPhotonDefinition()){
      
      if(IsWLS()) {//WLS photons are red
        G4cout << " LegendTrajectory optical photon WLS "  << G4endl;
        colour = G4Colour::Red();
      } else if(IsPmtHit()) {
        G4cout << " LegendTrajectory optical photon hit pmt "  << G4endl;
        colour = G4Colour::Blue();
      } else {//Scintillation and Cerenkov photons are green
        G4cout << " LegendTrajectory optical photon other " << fDrawit   << G4endl;
        colour = G4Colour::Green();
      }
    }
    else //All other particles 
      colour = G4Colour::White();
 
    G4VisAttributes trajectoryLineAttribs(colour);
    trajectoryLine.SetVisAttributes(&trajectoryLineAttribs);
    pVVisManager->Draw(trajectoryLine);
  }
  if (markersRequired) {
    auxiliaryPoints.SetMarkerType(G4Polymarker::squares);
    auxiliaryPoints.SetScreenSize(markerSize);
    auxiliaryPoints.SetFillStyle(G4VMarker::filled);
    G4VisAttributes auxiliaryPointsAttribs(G4Colour::Yellow()); 
    auxiliaryPoints.SetVisAttributes(&auxiliaryPointsAttribs);
    pVVisManager->Draw(auxiliaryPoints);

    stepPoints.SetMarkerType(G4Polymarker::circles);
    stepPoints.SetScreenSize(markerSize);
    stepPoints.SetFillStyle(G4VMarker::filled);
    G4VisAttributes stepPointsAttribs(G4Colour::Magenta());
    stepPoints.SetVisAttributes(&stepPointsAttribs);
    pVVisManager->Draw(stepPoints);
  }
}
