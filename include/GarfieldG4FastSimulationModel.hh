/*
 * GarfieldModel.h
 *
 *  Created on: Apr 9, 2014
 *      Author: dpfeiffe
 */

#ifndef GARFIELDG4FASTSIMULATIONMODEL_H_
#define GARFIELDG4FASTSIMULATIONMODEL_H_

#include <map>
#include <vector>
#include <iostream>

#include "G4VFastSimulationModel.hh"
#include "GarfieldPhysics.hh"

#include "ViewField.hh"
#include "ViewCell.hh"
#include "ViewGeometry.hh"
#include "ComponentAnalyticField.hh"
#include "MediumMagboltz.hh"
#include "SolidBox.hh"
#include "SolidTube.hh"
#include "GeometryRoot.hh"
#include "GeometrySimple.hh"
#include "Sensor.hh"
#include "ViewDrift.hh"
#include "FundamentalConstants.hh"
#include "DriftLineRKF.hh"
#include "AvalancheMC.hh"
#include "AvalancheMicroscopic.hh"
#include "ViewMedium.hh"
#include "ViewSignal.hh"
#include "Random.hh"
#include "TrackHeed.hh"
#include "Plotting.hh"
#include "ComponentConstant.hh"
#include "ComponentAnalyticField.hh"

class G4VPhysicalVolume;

class GarfieldG4FastSimulationModel : public G4VFastSimulationModel
{
public:
  //-------------------------
  // Constructor, destructor
  //-------------------------
	GarfieldG4FastSimulationModel (G4String, G4Region*);
	GarfieldG4FastSimulationModel (G4String);
  ~GarfieldG4FastSimulationModel ();

  void SetPhysics(GarfieldPhysics* fGarfieldPhysics);
  void WriteGeometryToGDML(G4VPhysicalVolume* physicalVolume);

  virtual G4bool IsApplicable(const G4ParticleDefinition&);
  virtual G4bool ModelTrigger(const G4FastTrack &);
  virtual void DoIt(const G4FastTrack&, G4FastStep&);
  std::vector<GarfieldElectron*>* GetSecondaryElectrons();
  void DeleteSecondaryElectrons();
  inline void EnableCreateSecondariesInGeant4() {createSecondariesInGeant4 = true;};
  inline void DisableCreateSecondariesInGeant4() {createSecondariesInGeant4 = false;};
  inline bool GetCreateSecondariesInGeant4() {return createSecondariesInGeant4;};
  
private:

  GarfieldPhysics* fGarfieldPhysics;
  
  TGeoManager* fGeoManager;
  Garfield::MediumMagboltz* fMediumMagboltz;
  Garfield::Sensor* fSensor;
  Garfield::AvalancheMC* fDrift;
  Garfield::AvalancheMicroscopic* fAvalanche;
  Garfield::DriftLineRKF* fdriftline;
  Garfield::TrackHeed* fTrackHeed;
  Garfield::GeometryRoot* fGeometryRoot;
  Garfield::GeometrySimple* fGeometrySimple;
  Garfield::ComponentAnalyticField* fComponentAnalyticField;
  // Garfield::ComponentConstant* fComponentConstantE;
  // Garfield::ComponentConstant* fComponentConstantM;
  Garfield::SolidTube* fTube;
  
  double fEnergyDeposit1;
  double fAvalancheSize1;
  double fGain1;
  std::vector<double> fDriftTime1;
  double fEnergyDepHeed1;
  int fNClustersHeed1;
  int fNElectronsHeed1;
  int nsum1;
  std::vector<double> fRadiusTravelled1;
  std::vector<double> fFinalX1;
  std::vector<double> fFinalY1;
  std::vector<double> fFinalZ1;

  bool createSecondariesInGeant4;


};


#endif /* GARFIELDMODEL_H_ */
