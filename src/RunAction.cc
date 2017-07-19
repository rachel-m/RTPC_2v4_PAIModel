// ESSN a Geant-4 Based Model of the Lund ESS Neutron Test Facility
// J.R.M Annand, University of Glasgow
// Class RunAction
// MC run control
// 20/05/13 JRMA adapted from SBS equivalent, under construction
//
#include "RunAction.hh"
#include "GarfieldAnalysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

RunAction::RunAction()
{
  fEventAction=NULL;


  // set printing event number per each event
  // G4RunManager::GetRunManager()->SetPrintProgress(1);
  
  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in Garfieldnalysis.hh
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;
  
  // Create directories
  //analysisManager->SetHistoDirectoryName("histograms");
  //analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetFirstHistoId(1);
  
  // Book histograms, ntuple
  //
  
  // Creating histograms
  analysisManager->CreateH1("1", "Edep in absorber", 100, 0., 800 * MeV);
  analysisManager->CreateH1("2", "trackL in absorber", 100, 0., 1 * m);  
  analysisManager->CreateH1("3", "Edep in gas (heed) [keV]", 5001, 0., 10000);
  analysisManager->CreateH1("4", "Number of Clusters created per track", 10001,0, 100000);
  analysisManager->CreateH1("5", "Number of electrons created per track", 1001, 0., 10000);
  analysisManager->CreateH1("6", "drifttime", 200, 0., 56000.0);
  analysisManager->CreateH1("7", "radiustravelled", 150, 0., 15.0);
  analysisManager->CreateH1("8", "ztravelled", 401, 0., 40.0);
  analysisManager->CreateH1("9", "deltaphi", 360, -180., 180.0);
  analysisManager->CreateH1("10", "phiID", 900, 0., 899.0);
  analysisManager->CreateH1("11", "NoPhiStripsPerE", 900, 0., 899.0);
  analysisManager->CreateH1("12", "NoPhiStripsPerProton", 900, 0., 900.0);
  analysisManager->CreateH1("13", "ID", 18001, 0., 18001.0);

  analysisManager->CreateH2("1", "radiusVStime", 200, 0., 56000.0, 150, 0., 15.0);
  analysisManager->CreateH2("2", "timeVSphi", 900, 0., 899.0, 200, 0., 56000.0);
  analysisManager->CreateH2("3", "phiVSz", 361, -180., 180.0, 361, -180., 180.0);
  analysisManager->CreateH2("4", "timeVSid", 18001, 0., 18001.0, 200, 0., 56000.0);

  // analysisManager->CreateH2("2", "zVStime", 50000, 0., 50000.0,  401, 0., 40.0);

  analysisManager->CreateH3("1", "Track position",200, -10*cm, 10*cm, 29, -1.45*cm, 1.45*cm, 29,-1.45*cm, 1.45*cm);


  
  // Creating ntuple
  //
  analysisManager->CreateNtuple("Garfield", "Edep and TrackL");
  analysisManager->CreateNtupleDColumn("Eabs");
  analysisManager->CreateNtupleDColumn("Labs");
  analysisManager->CreateNtupleDColumn("Egas");
  analysisManager->CreateNtupleDColumn("AvalancheSize");
  analysisManager->CreateNtupleDColumn("Gain");
  analysisManager->CreateNtupleDColumn("DriftTime");
  analysisManager->FinishNtuple();
  
}

RunAction::~RunAction()
{
  delete G4AnalysisManager::Instance();
}

void RunAction::BeginOfRunAction(const G4Run* aRun)
{ 
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
  fEventAction= (EventAction*)(G4RunManager::GetRunManager()->GetUserEventAction());
  fEventAction->PrepareOutput();

  // Get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  
  // Open an output file
  //
  G4String fileName = "Garfield";
  analysisManager->OpenFile(fileName);
  
}

void RunAction::EndOfRunAction(const G4Run* aRun)
{
  G4int NbOfEvents = aRun->GetNumberOfEvent();
  if (NbOfEvents == 0) return;
  fEventAction->CloseOutput();


  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	// if (analysisManager->GetH1(1)) {
	// 	G4cout << G4endl << " ----> print histograms statistic ";
	// 	if (isMaster) {
	// 		G4cout << "for the entire run " << G4endl << G4endl;
	// 	} else {
	// 		G4cout << "for the local thread " << G4endl << G4endl;
	// 	}

	// 	G4cout << " EAbs : mean = "
	// 			<< G4BestUnit(analysisManager->GetH1(1)->mean(), "Energy")
	// 			<< " rms = "
	// 			<< G4BestUnit(analysisManager->GetH1(1)->rms(), "Energy")
	// 			<< G4endl;

	// 	G4cout << " LAbs : mean = "
	// 			<< G4BestUnit(analysisManager->GetH1(2)->mean(), "Length")
	// 			<< " rms = "
	// 			<< G4BestUnit(analysisManager->GetH1(2)->rms(), "Length")
	// 			<< G4endl;

	// 	G4cout << " EGas : mean = "
	// 			<< G4BestUnit(analysisManager->GetH1(3)->mean(), "Energy")
	// 			<< " rms = "
	// 			<< G4BestUnit(analysisManager->GetH1(3)->rms(), "Energy")
	// 			<< G4endl;

	// 	G4cout << " Avalanche size : mean = "
	// 					<< analysisManager->GetH1(4)->mean()
	// 					<< " rms = "
	// 					<< analysisManager->GetH1(4)->rms()
	// 					<< G4endl;

	// 	G4cout << " Gain : mean = "
	// 					<< analysisManager->GetH1(5)->mean()
	// 					<< " rms = "
	// 					<< analysisManager->GetH1(5)->rms()
	// 					<< G4endl;
	// }

	// save histograms & ntuple
	//
	analysisManager->Write();
	analysisManager->CloseFile();

  

}

