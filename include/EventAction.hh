// RecTagg a Geant-4 Based Model of RTPC for recoil-proton tagging
// J.R.M Annand, University of Glasgow
// Class EventAction
// Data saving procedures event by event
// 05/05/14 JRMA
//
#ifndef EventAction_h
#define EventAction_h 1
#include "G4UserEventAction.hh"
#include "globals.hh"
#include "DetectorConstruction.hh"

class RunAction;
class EventActionMessenger;
class PrimaryGeneratorAction;
class RootIO;

class EventAction : public G4UserEventAction
{
public:
  EventAction(RunAction*);
  ~EventAction();
public:
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  void SetDrawFlag   (G4String val)  {fdrawFlag = val;}
  void SetPrintModulo(G4int    val)  {fprintModulo = val;}
  void SetCollID(G4int val){fCollID=val;}
  void SetIsInteractive(G4int is){fIsInteractive=is;}
  void SetHitDrawOpt(G4String val){fHitDrawOpt=val;}
  void SetOutFileName(G4String name){fOutFileName=name;}
  G4int PrepareOutput();
  void CloseOutput();
  void SetDet(DetectorConstruction* det){ fDet = det; }
  RootIO* GetRootIO(){ return fRTPCOut; }

  void AddAbs(G4double de, G4double dl);
  void AddGas(G4double de);
  
private:
    G4double fEnergyAbs;
  G4double fEnergyGas;
  G4double fTrackLAbs;
  G4double fAvalancheSize;
  G4double fGain;
  std::vector<G4double> fDriftTime;
  G4double fEnergyDepHeed;
  G4int fNClustersHeed;
  G4int fNElectronsHeed;
  std::vector<G4double> fRadiusTravelled;
  std::vector<G4double> fzTravelled;
  std::vector<G4double> fphitravelled;
  std::vector<G4double> fID;
  std::vector<G4double> fphiID;
  std::vector<double> fFinalPhi;
  std::vector<double> fFinalX;
  std::vector<double> fFinalY;
  std::vector<double> fFinalZ;
  std::vector<double> fTimeID;
  std::vector<double> fPhiCover;
  std::vector<int> vStripsPerTrack;
  RunAction*  frunAct;
  RootIO* fRTPCOut;
  DetectorConstruction* fDet;
  G4int fIsInteractive;        
  G4String  fdrawFlag;
  G4int     fprintModulo;
  G4int     fDrawMode;
  G4String fHitDrawOpt;
  EventActionMessenger*  feventMessenger;
  G4int fCollID;
  G4String fOutFileName;
  PrimaryGeneratorAction* fPGA;
};

// inline functions

inline void EventAction::AddAbs(G4double de, G4double dl) {
	fEnergyAbs += de;
	fTrackLAbs += dl;
}

inline void EventAction::AddGas(G4double de) {
	fEnergyGas += de;

}




#endif

    
