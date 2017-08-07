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
// $Id$
//
/// \file EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EventAction_h
#define EventAction_h 1

#include "LegendAnalysis.hh"
//#include "LTTrack.hxx"
#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4VTrajectory.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

class RunAction;

/// Event action class
///
/// It holds data member fEnergySum and fEnergy2Sum for accumulating
/// the event energy deposit its square event by event.
/// These data are then used in the run action to compute the dose.
/// The accumulated energy and enrgy square sums are reset for each
/// new run via the Reset() function from the run action.

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction*);
    virtual ~EventAction();

    // static access method

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);


    // get methods
    G4ThreeVector direction;
		G4ThreeVector position;
		void FillDetector (G4int,G4double);

  private:
		RunAction* runAct;
    TDirectory *fDir;
    TH1F *hEventPhotonLambda;
    TH1F *hEventPhotonCount;
    G4int     fPrintModulo;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif



