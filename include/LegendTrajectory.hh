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
// $Id: LegendTrajectory.hh 72349 2013-07-16 12:13:16Z gcosmo $
//
/// \file optical/Legend/include/LegendTrajectory.hh
/// \brief Definition of the LegendTrajectory class
//
#ifndef LegendTrajectory_h
#define LegendTrajectory_h 1

#include "UserTrackInformation.hh"
#include "G4Trajectory.hh"
#include "G4Allocator.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4TrajectoryPoint.hh"
#include "G4Track.hh"
#include "G4Step.hh"

class G4Polyline;                   // Forward declaration.

typedef std::vector<G4VTrajectoryPoint*>  TrajectoryPointContainer;

class LegendTrajectory : public G4Trajectory
{
  public:

    LegendTrajectory();
    LegendTrajectory(const G4Track* aTrack);
    LegendTrajectory(LegendTrajectory &);
    virtual ~LegendTrajectory();
 
    virtual void DrawTrajectory() const;
 
    inline void* operator new(size_t);
    inline void  operator delete(void*);

    void SetDrawTrajectory(G4bool b){fDrawit=b;}
    void SetTrackStatus(G4int status){fTrackStatus=status;}
    G4int GetTrackStatus(){ return fTrackStatus;}
    bool IsPmtHit() const { return fTrackStatus&TrackStatus::hitPMT;}
    bool IsGeHit() const { return fTrackStatus&TrackStatus::hitGe;}
    bool IsWLS() const { return fTrackStatus&TrackStatus::hitWLS;}
    bool IsPrimary() { return fPrimary; }
    void SetPrimary() { fPrimary=true;}

    void SetForceDrawTrajectory(G4bool b){fForceDraw=b;}
    void SetForceNoDrawTrajectory(G4bool b){fForceNoDraw=b;}
    const G4Track* GetTrack() { return fTrack;};
    void SetParentId( G4int id) { fParentId = id;}
    G4int GetParentId() { return fParentId;}

  private:

    TrajectoryPointContainer *positionRecord;
    G4int fParentId;
    G4bool fPrimary;
    const G4Track* fTrack;
    G4int fTrackStatus;
    G4bool fDrawit;
    G4bool fForceNoDraw;
    G4bool fForceDraw;
    G4ParticleDefinition* fParticleDefinition;
};

extern G4ThreadLocal G4Allocator<LegendTrajectory>* LegendTrajectoryAllocator;

inline void* LegendTrajectory::operator new(size_t)
{
  if(!LegendTrajectoryAllocator)
      LegendTrajectoryAllocator = new G4Allocator<LegendTrajectory>;
  return (void*)LegendTrajectoryAllocator->MallocSingle();
}

inline void LegendTrajectory::operator delete(void* aTrajectory)
{
  LegendTrajectoryAllocator->FreeSingle((LegendTrajectory*)aTrajectory);
}

#endif
