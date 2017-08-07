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
/// \file firstTest.cc
/// \brief Main program of the  example

#include "DetectorConstruction.hh"
#include "LegendAnalysis.hh"
#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionMessenger.hh"

//#include "generators/MGGeneratorRDMPrimary.hh"

#include "RunAction.hh"
#include "EventAction.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
//#include "QBBC.hh"
//#include "QGSP_BERT_HP.hh"

#include "PhysicsList.hh"
/* from MaGe */
//#include "processes/MGProcessesList.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // analysis
  G4cout << " init analysis manager " << G4endl;
  LegendAnalysis* ana = LegendAnalysis::Instance();
  // Choose the Random engine
  //
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  CLHEP::HepRandom::setTheSeed(time(0));
  // Construct the default run manager
  //
  G4RunManager * runManager = new G4RunManager;
  runManager->SetVerboseLevel(0);

  // Set mandatory initialization classes
  //
  // Detector construction
  DetectorConstruction* detector = new DetectorConstruction();
  runManager->SetUserInitialization(detector);
  // Physics list
  /******** from MaGe/processes/ ***********/
  //MGProcessesList* physicsList = new MGProcessesList();
  //runManager->SetUserInitialization(physicsList);
  PhysicsList* physicsList = new PhysicsList();
  runManager->SetUserInitialization(physicsList);

  // Primary generator action
  PrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction();
  //MGGeneratorRDMPrimary* gen_action = new MGGeneratorRDMPrimary();
  runManager->SetUserAction(gen_action);
  new PrimaryGeneratorActionMessenger(gen_action);

  // Set user action classes

  RunAction* run_action = new RunAction(detector,gen_action);
  runManager->SetUserAction(run_action);

  EventAction* event_action = new EventAction(run_action);
  runManager->SetUserAction(event_action);

  TrackingAction* tracking_action = new TrackingAction();
  runManager->SetUserAction(tracking_action);
  
  G4UserSteppingAction* stepping_action = new SteppingAction(detector, event_action);
  runManager->SetUserAction(stepping_action);

  // Initialize G4
  runManager->Initialize();

  // this has to be done after detector construction 
  gen_action->SetPhysicalVolumeByName("phys_Argon");
  gen_action->SetParticleByName("gamma");
  gen_action->Show();
  
#ifdef G4VIS_USE
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if (argc!=1) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else {
    // interactive mode : define UI session
#ifdef G4UI_USE
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
    UImanager->ApplyCommand("/control/execute Macro/vis.mac");
#endif
    ui->SessionStart();
    delete ui;
#endif
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;
  delete ana;
  
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
