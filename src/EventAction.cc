// RecTagg a Geant-4 Based Model of RTPC for recoil-proton tagging
// J.R.M Annand, University of Glasgow
// Class EventAction
// Data saving procedures event by event
// 05/05/14 JRMA
//
#include "EventAction.hh"
#include "GarfieldPhysics.hh"
#include "GarfieldAnalysis.hh"
#include "PrimaryGeneratorAction.hh"
#include "RootIO.hh"
#include "ArrayHit.hh"
//#include "SBSVisHit.hh"
#include "RunAction.hh"
#include "EventActionMessenger.hh"
#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4VVisManager.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"
#include "G4RunManager.hh"
#include <iomanip>

Hparm H3p[] = {
  {200, -200, 200, 200, -200, 200, 200, -200, 200},
  {200, -200, 200, 200, -200, 200, 200, -200, 200},
  {200, -200, 200, 200, -200, 200, 200, -200, 200},
  {200, -200, 200, 200, -200, 200, 200, -200, 200},
  {200, -200, 200, 200, -200, 200, 200,    0,8000},
  {200, -200, 200, 200,    0,8000, 200,    0, 200},
  {200, -200, 200, 200, -200, 200, 200,    0, 200},
  {200, -200, 200, 200, -200, 200, 200,    0, 200}
};
Hparm H2p[] = {
  {400, 0, 400, 400, 0, 10, 0,0,0},
  {400, 0, 400, 400, 0, 100, 0,0,0},
  {200, -200, 200, 200, -200, 200, 0,0,0},
  {200, -200, 200, 200, -200, 200, 0,0,0},
  {200, -200, 200, 200, -200, 200, 0,0,0},
  {200, -200, 200, 200, -200, 200, 0,0,0},
  {200, -200, 200, 200, -200, 200, 0,0,0},
  {200, -200, 200, 200, -200, 200, 0,0,0}
};
Hparm H1p[] = {
  {11000, 0000, 11000, 0,0,0,0,0,0},
  {11000, 0000, 11000, 0,0,0,0,0,0},
  {11000, 0000, 11000, 0,0,0,0,0,0},
  {11000, 0000, 11000, 0,0,0,0,0,0},
  {11000, 0000, 11000, 0,0,0,0,0,0},
  {11000, 0000, 11000, 0,0,0,0,0,0},
  {11000, 0000, 11000, 0,0,0,0,0,0},
  {11000, 0000, 11000, 0,0,0,0,0,0}
};


//
//------------------------------------------------------------------------------
EventAction::EventAction(RunAction* run)
:frunAct(run),fdrawFlag("all"),fprintModulo(1),feventMessenger(0)
{
  feventMessenger = new EventActionMessenger(this);
  fIsInteractive=1;
  // hits collections
  fCollID = -1;
  fHitDrawOpt="edep";
  fOutFileName=G4String("NULL");
  fprintModulo=1000;
  fRTPCOut=NULL;
  //f4Vector = NULL;
  fPGA = (PrimaryGeneratorAction*)
    (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
}
//
//------------------------------------------------------------------------------
EventAction::~EventAction()
{
  delete feventMessenger;
}

//
//------------------------------------------------------------------------------
void EventAction::BeginOfEventAction(const G4Event* evt)
{  
  G4int evtNb = evt->GetEventID();
  if (evtNb%fprintModulo ==0) { 
    G4cout <<evtNb%fprintModulo<< "---> Begin of event: " << evtNb << G4endl;
  }


  G4cout  << G4endl << G4endl << G4endl << G4endl << G4endl << G4endl
	  << "-------------------------------------------------> Begin of event: " << evtNb << G4endl;

  fEnergyAbs = 0;
  fEnergyGas = 0;
  fTrackLAbs = 0;
  fAvalancheSize = 0;
  fGain = 0;
  fDriftTime.clear();
  fEnergyDepHeed = 0.0;
  fNClustersHeed = 0;
  fNElectronsHeed = 0;

  GarfieldPhysics* garfieldPhysics = GarfieldPhysics::GetInstance();
  garfieldPhysics->Clear();

  
}
//
//------------------------------------------------------------------------------
void EventAction::EndOfEventAction(const G4Event* evt)
{

	// Accumulate statistics
	//
	GarfieldPhysics* garfieldPhysics = GarfieldPhysics::GetInstance();

	// get analysis manager
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	//fEnergyGas += garfieldPhysics->GetEnergyDeposit_MeV();
	fAvalancheSize = garfieldPhysics->GetAvalancheSize();
	fGain = garfieldPhysics->GetGain();
	fDriftTime =  garfieldPhysics->GetDriftTime();
	fEnergyDepHeed = garfieldPhysics->GetEnergyDepHeed();
	fNClustersHeed = garfieldPhysics->GetNClustersHeed();
	fNElectronsHeed = garfieldPhysics->GetNElectronsHeed();
	fRadiusTravelled = garfieldPhysics->GetRadiusTravelled();
	fFinalX = garfieldPhysics->GetFinalX();
    	fFinalY = garfieldPhysics->GetFinalY();
    	fFinalZ = garfieldPhysics->GetFinalZ();
    
	// fill histograms
	analysisManager->FillH1(1, fEnergyAbs);
	analysisManager->FillH1(2, fTrackLAbs);
	analysisManager->FillH1(3, fEnergyDepHeed);
	analysisManager->FillH1(4, fNClustersHeed);
	analysisManager->FillH1(5, fNElectronsHeed);

	
        const int DriftTimeSize = fDriftTime.size();
	// printf("\n\ndrift time size %d\n", DriftTimeSize);

	int nStripsPerTrack = 0;
	for(int i=0; i<DriftTimeSize; i++){
	  analysisManager->FillH1(6, fDriftTime[i]);
	  analysisManager->FillH1(7, fRadiusTravelled[i]);

	  analysisManager->FillH2(1, fDriftTime[i], fRadiusTravelled[i]);

	  analysisManager->FillNtupleDColumn(5, fDriftTime[i]);


	  
	}



	// fill ntuple
	analysisManager->FillNtupleDColumn(0, fEnergyAbs);
	analysisManager->FillNtupleDColumn(1, fTrackLAbs);
	analysisManager->FillNtupleDColumn(2, fEnergyGas);
	analysisManager->FillNtupleDColumn(3, fAvalancheSize);
	analysisManager->FillNtupleDColumn(4, fGain);
	// analysisManager->FillNtupleDColumn(5, fDriftTime);

	// Print per event (modulo n)
	//
	G4int eventID = evt->GetEventID();
	G4int printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
	if ((printModulo > 0) && (eventID % printModulo == 0)) {
		G4cout << "---> End of event: " << eventID << G4endl;

		G4cout << "   Absorber: total energy: " << std::setw(7)
				<< G4BestUnit(fEnergyAbs, "Energy")
				<< "       total track length: " << std::setw(7)
				<< G4BestUnit(fTrackLAbs, "Length") << G4endl;

		G4cout << "        Gas: total energy: " << std::setw(7)
				<< G4BestUnit(fEnergyGas, "Energy") <<  "       avalanche size: " << fAvalancheSize <<  "       gain: " << fGain << G4endl;
	}
	



  
  //write to the output ntuple if it exists
  //if not need to set file via /ESSN/event/setOutputFile XXX.root
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  if(fRTPCOut){
    fRTPCOut->WriteHit(HCE);
  }
}  
//
//------------------------------------------------------------------------------
G4int EventAction::PrepareOutput(){
  if(fOutFileName == "NULL") {
    G4cout<<"G4int EventAction::PrepareOutput() no output file"
	  <<G4endl;
    G4cout<<"Please supply filename if output required..."<<G4endl;
    G4cout<<"/RTPC/event/SetOutputFile XXX.root"<<G4endl;
    return 0;
  }
  printf("EventAction Output will be written to %s\n",fOutFileName.data());
  //Create output tree
  //This is curently made in the same format as the cbsim output
  if( fPGA->GetRootIO() ){
    fRTPCOut = fPGA->GetRootIO();
    fRTPCOut->SetOutput(fOutFileName.data());
    for(G4int i=0; i<6; i++){ fRTPCOut->SetH3(i,H3p+i); }
    for(G4int i=0; i<3; i++){
      fRTPCOut->SetH1(i,H1p+i);
      fRTPCOut->SetH2(i,H2p+i);
    }
    fRTPCOut->SetHits(fDet->GetNhits(), fDet->GetHitID(), fDet->GetHits());
  }
  else{
    fRTPCOut = new RootIO(fOutFileName.data(),EOutputFile);
    for(G4int i=0; i<3; i++){ fRTPCOut->SetH3(i,H3p+i); }
  }
  return 1;
}
//
//------------------------------------------------------------------------------
void  EventAction::CloseOutput(){
  if(!fRTPCOut) return;
  fRTPCOut->Close();
}
