// ESSN a Geant-4 Based Model of the Lund ESS Neutron Test Facility
// J.R.M Annand, University of Glasgow
// Main program
// Startup the simulation
// 20/05/13 JRMA adapted from SBS equivalent, under construction
//
#include "GarfieldPhysics.hh"
#include "PhysicsList.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#include "Randomize.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif
#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "SteppingVerbose.hh"

//#include "LHEP_BIC.hh"

int main(int argc,char** argv) {


  //Get instance of singleton GarfieldPhysics before creation of the physics list
  GarfieldPhysics* garfieldPhysics = GarfieldPhysics::GetInstance();


  // Enable GarfieldModel for different particle types and energy ranges
  double minEnergy_keV = 0.0;//0;//orig 0.01
  double maxEnergy_keV = 500000.0;//1;//orig 1

  garfieldPhysics->AddParticleName("e-", minEnergy_keV, maxEnergy_keV);
  //  garfieldPhysics->AddParticleName("proton", minEnergy_keV, maxEnergy_keV);

  //garfieldPhysics->EnableCreateSecondariesInGeant4();
  // garfieldPhysics->DisableCreateSecondariesInGeant4();

        
  
  G4int IsInteractive = 0;
  if(argc == 1) IsInteractive = 1; //no parameters so interactive
  if(argc > 1){
    if( !strcmp(argv[1],"NULL") ) IsInteractive = 1;
  }
  long rseed = -1;
  if(argc > 2)
    sscanf(argv[2],"%ld",&rseed);
  // choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  long seed[2];
  if(rseed == 0){
    time_t systime = time(NULL);
    seed[0] = (long)systime;
    seed[1] = seed[0] + systime;
  }
  else{
    seed[0] = rseed;
    seed[1] = 2*rseed;
  }
  if(rseed >= 0){
    CLHEP::HepRandom::setTheSeeds(seed);
    printf("RanecuEngine seeded with %ld  %ld\n", seed[0], seed[1]);
  }
  //my Verbose output class
  G4VSteppingVerbose::SetInstance(new SteppingVerbose);
  // Construct the default run manager
  G4RunManager * runManager = new G4RunManager;
  // set mandatory initialization classes
  DetectorConstruction* detector = new DetectorConstruction( );
  detector->SetIsInteractive(IsInteractive);
  runManager->SetUserInitialization(detector);
  //use below insted if cannot install physics_list
  runManager->SetUserInitialization(new PhysicsList);
  
#ifdef G4VIS_USE
  // visualization manager
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif
    
  // set user action classes
  PrimaryGeneratorAction* pga=new PrimaryGeneratorAction(detector);
  runManager->SetUserAction(pga);

  RunAction* runaction = new RunAction;  
  runManager->SetUserAction(runaction);
  EventAction* eventaction = new EventAction(runaction);
  eventaction->SetIsInteractive(IsInteractive);
  runManager->SetUserAction(eventaction);
  runManager->SetUserAction(new SteppingAction(detector, eventaction));
  eventaction->SetDet(detector);
  
  G4UImanager * UImanager = G4UImanager::GetUIpointer();  
  if( argc == 1 ){           // interactive
#ifdef G4UI_USE
    G4UIExecutive * ui = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
    UImanager->ApplyCommand("/control/execute macros/vis.mac");     
#endif
    ui->SessionStart();
    delete ui;
#endif
  }
  else           // Batch mode
    { 
#ifdef G4VIS_USE
      visManager->SetVerboseLevel("quiet");
#endif
      //Run set up macro
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UImanager->ApplyCommand(command+fileName);
      //do ROOT run
      if(pga->GetMode()==EPGA_ROOT)
	runManager->BeamOn(pga->GetNEvents());
    }

  // job termination
#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;
  GarfieldPhysics::Dispose();

  return 0;
}
