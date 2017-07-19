/*
 * GarfieldModel.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: dpfeiffe
 */
#include <iostream>
#include "GarfieldG4FastSimulationModel.hh"
#include "GarfieldPhysics.hh"
#include "G4VPhysicalVolume.hh"
#include "SolidBox.hh"
#include "SolidTube.hh"
#include "G4GDMLParser.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "TMath.h"


#include "G4SystemOfUnits.hh"

#include "TOSCAField2D.hh"


GarfieldG4FastSimulationModel::GarfieldG4FastSimulationModel(G4String modelName,
							     G4Region* envelope) :
  G4VFastSimulationModel(modelName, envelope) {
  fGarfieldPhysics = GarfieldPhysics::GetInstance();
  fGarfieldPhysics->InitializePhysics();

}

GarfieldG4FastSimulationModel::GarfieldG4FastSimulationModel(G4String modelName) :
  G4VFastSimulationModel(modelName) {
  fGarfieldPhysics = GarfieldPhysics::GetInstance();
  fGarfieldPhysics->InitializePhysics();

}


GarfieldG4FastSimulationModel::~GarfieldG4FastSimulationModel() {
}

void GarfieldG4FastSimulationModel::WriteGeometryToGDML(
							G4VPhysicalVolume* physicalVolume) {

  G4GDMLParser* parser = new G4GDMLParser();
  remove("garfieldGeometry.gdml");
  parser->Write("garfieldGeometry.gdml", physicalVolume, false);
  delete parser;
}



// std::vector<GarfieldElectron*>* GarfieldPhysics::GetSecondaryElectrons() {
// 	return fSecondaryElectrons;
// }

// void GarfieldPhysics::DeleteSecondaryElectrons() {
// 	if (!fSecondaryElectrons->empty()) {
// 		fSecondaryElectrons->erase(fSecondaryElectrons->begin(),
// 				fSecondaryElectrons->end());
// 	}
// }



G4bool GarfieldG4FastSimulationModel::IsApplicable(
						   const G4ParticleDefinition& particleType) {
  G4String particleName = particleType.GetParticleName();
  // if (fGarfieldPhysics->FindParticleName(particleName,"garfield")) {
  if (fGarfieldPhysics->FindParticleName(particleName)) {
    G4cout << "\nGarfield model applicable for " << particleName << G4endl;
    return true;
  }
  return false;
  
}

G4bool GarfieldG4FastSimulationModel::ModelTrigger(const G4FastTrack& fastTrack) {

  double ekin_keV = fastTrack.GetPrimaryTrack()->GetKineticEnergy() / keV;

  G4String particleName = fastTrack.GetPrimaryTrack()->GetParticleDefinition()->GetParticleName();

  G4ThreeVector LocalPositionTrack = fastTrack.GetPrimaryTrackLocalPosition();
  // Double_t RadiusLocalPosition = LocalPositionTrack.perp();
  Double_t RadiusLocalPosition = sqrt(LocalPositionTrack.x()*LocalPositionTrack.x()+LocalPositionTrack.y()*LocalPositionTrack.y());//LocalPositionTrack.perp();
  //if (fastTrack.GetPrimaryTrack()->GetParentID() != 0) {
  G4int ParticleParent = fastTrack.GetPrimaryTrack()->GetParentID();

  // if (fGarfieldPhysics->FindParticleNameEnergy(particleName, ekin_keV) && RadiusLocalPosition>=50.0 && ParticleParent==1) {
  if (fGarfieldPhysics->FindParticleNameEnergy(particleName, ekin_keV) && RadiusLocalPosition>=50.0 && RadiusLocalPosition<=150.0) {
    G4cout << "Garfield model triggered for " << particleName << " with "
    	   << ekin_keV << " keV and radius " << RadiusLocalPosition << " mm" <<
      " local position x y z mm " << LocalPositionTrack.x() << " " <<  LocalPositionTrack.y() << " " <<  LocalPositionTrack.z() <<
      " Parent ID is " << ParticleParent << G4endl;
    
    return true;
  }
  // else {
  return false;
  // }
  
}

void GarfieldG4FastSimulationModel::DoIt(const G4FastTrack& fastTrack,
		G4FastStep& fastStep) {


  int StripID[817][22] = {0};
  int stripidcounter = -1;
  for(int pinc = 0; pinc<817; pinc++){
    for(int zinc = 0; zinc<22; zinc++){
      stripidcounter++;
      StripID[pinc][zinc] = stripidcounter;
    }
  }


  G4TouchableHandle theTouchable = fastTrack.GetPrimaryTrack()->GetTouchableHandle();
  G4String name = theTouchable->GetVolume()->GetName();

  G4ThreeVector pdirection = fastTrack.GetPrimaryTrack()->GetMomentum().unit();
  // printf("\n pdirection %g %g %g \n", pdirection.x(), pdirection.y(), pdirection.z());
  G4ThreeVector localdir = fastTrack.GetPrimaryTrackLocalDirection();

  G4ThreeVector worldPosition = fastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector localPosition = fastTrack.GetPrimaryTrackLocalPosition();
  
  double ekin_keV = fastTrack.GetPrimaryTrack()->GetKineticEnergy() / keV;
  double ekin_eV = fastTrack.GetPrimaryTrack()->GetKineticEnergy() / eV;
  G4ThreeVector mom = fastTrack.GetPrimaryTrack()->GetMomentum() /eV;
  double mommageV = mom.r();
  // cout <<" mom mag in keV " << mommageV/1000.00 << endl;
  // cout << "energy in keV " << ekin_eV/1000.00 << endl;
  

  
  G4double globalTime = fastTrack.GetPrimaryTrack()->GetGlobalTime();
  // printf("globalTime %g\n",globalTime);
  // printf("track ID is %d and parent ID is %d\n", fastTrack.GetPrimaryTrack()->GetTrackID(),  fastTrack.GetPrimaryTrack()->GetParentID());


  G4String particleName = fastTrack.GetPrimaryTrack()->GetParticleDefinition()->GetParticleName();
  if (particleName == "e-")printf("We have a delta electron\n");
  else printf("ITS NOT AN ELECTRON\n");
  
  fMediumMagboltz = 0;
  fSensor = 0;
  fAvalanche = 0;
  fDrift = 0;
  fComponentAnalyticField = 0;
  fTrackHeed = 0;
  fGeoManager = 0;
  fGeometryRoot = 0;
  fGeometrySimple = 0;
  fTube = 0;
  
  
  // Kill the original electron
  fastStep.KillPrimaryTrack();
  fastStep.SetPrimaryTrackPathLength(0.0);
  fastStep.SetTotalEnergyDeposited(fastTrack.GetPrimaryTrack()->GetKineticEnergy());
  
  
  //---------------------------------------------------------------- set up gas and sensors
    fMediumMagboltz = new Garfield::MediumMagboltz();
    fMediumMagboltz->SetComposition("helium", 90., "methane", 10.);
    // fMediumMagboltz->PrintGas();
    fMediumMagboltz->SetTemperature(77.0);// 77K
    fMediumMagboltz->SetPressure(114.0);// 114torr is 0.15atm
    
    fMediumMagboltz->EnableDebugging();
    fMediumMagboltz->Initialise(true);
    fMediumMagboltz->DisableDebugging();
    
    fMediumMagboltz->LoadGasFile("helium_90_methane_10_77K_114Torr.gas");
    //fMediumMagboltz->LoadGasFile("/home/rachel/geant4/RTPC_GarfieldVaryingB/GarfieldOutput060317/helium_90_dme_10_293K_434Torr.gas");    
    fSensor = new Garfield::Sensor();
    fDrift = new Garfield::AvalancheMC();
    fAvalanche = new Garfield::AvalancheMicroscopic();
    fComponentAnalyticField = new Garfield::ComponentAnalyticField();
    fdriftline =  new DriftLineRKF();
    
    // set up geometry and field
    // Inner radius of the tube [cm]
    const double rTubeIn = 5.0;
    
    // Outer radius of the tube [cm]
    const double rTube = 15.0;
    
    // Half-length of the tube [cm]
    const double lTube = 24.;
    
    // Make a tube (centered at the origin, inner radius, outer radius, half-length).
    fTube = new Garfield::SolidTube(0., 0., 4.0, rTubeIn, rTube, lTube);
    
    fGeometrySimple = new Garfield::GeometrySimple();
    // Add the solid to the geometry, together with the medium inside.
    fGeometrySimple->AddSolid(fTube, fMediumMagboltz);
    
    const double voltagetube = 0.0;
    //    const double voltagewire = -7700.0;//-7700 brings exactly to match the result from farm and 38us upper limit ch4
    
    // Get magnetic field value from tosca map
    G4double pos[4];
    G4double b[3];
    
    TOSCAField2D *magFieldTemp = new TOSCAField2D("sol_map_03.dat", 0.0, 0.0, 200.0);
    
    pos[0] = (worldPosition.x() / CLHEP::cm) * 10.0;
    pos[1] = (worldPosition.y() / CLHEP::cm) * 10.0;
    pos[2] = (worldPosition.z() / CLHEP::cm) * 10.0;
    pos[3] = globalTime;
    
    magFieldTemp->GetFieldValue(pos,b);
    printf("mag field is %g %g %g\n",b[0]*1000.00, b[1]*1000.00,b[2]*1000.00);
    
    fComponentAnalyticField->SetMagneticField(b[0]*1000.00, b[1]*1000.00, b[2]*1000.00);  
    // fComponentAnalyticField->SetMagneticField(0.0,0.0,4.7);
    
    //for first sense wires
    double wirethickness = 0.0127;
    double wireradius = 5.00000;
    double wirepotential = -9000;
    
    double angleIncrement = TMath::Pi()/25.0000000;
    for(int inc=0; inc<25; inc++){
       fComponentAnalyticField->AddWire( wireradius*TMath::Cos(inc*angleIncrement), wireradius*TMath::Sin(inc*angleIncrement), wirethickness, wirepotential, "s");
       fComponentAnalyticField->AddWire( wireradius*TMath::Cos(-1.000*(inc+1.0)*angleIncrement), wireradius*TMath::Sin(-1.000*(inc+1)*angleIncrement), wirethickness, wirepotential, "s");
     }
    
    fComponentAnalyticField->AddTube(rTube, voltagetube, 0, "t");
    
    ////for concentric cylinder
    //fComponentAnalyticField->AddWire(0., 0., rTubeIn, voltagewire, "s");
    //fComponentAnalyticField->AddTube(rTube, voltagetube, 0, "t");
    
  fComponentAnalyticField->SetGeometry(fGeometrySimple);
  fSensor->AddComponent(fComponentAnalyticField);
  
  // driftline->SetSensor(fSensor);

  // driftline->SetIntegrationAccuracy(1.e-3);//9
  // // driftline->SetMaximumStepSize(0.0020);

  fDrift->SetSensor(fSensor); //drift to drift electrons using avalancheMC method
  fDrift->SetTimeSteps(100.0); // Do the drift line calculation in time steps of ns. // standard  0.05
  
  fDrift->EnableMagneticField();
  
  fAvalanche->SetSensor(fSensor);
  
  fTrackHeed = new Garfield::TrackHeed();
  fTrackHeed->SetSensor(fSensor);
  
  fTrackHeed->EnableElectricField();
  fTrackHeed->EnableMagneticField();

  // fTrackHeed->EnableDeltaElectronTransport();


  // DeleteSecondaryElectrons();

  fEnergyDeposit1 = 0.0;;
  fAvalancheSize1 = 0.0;
  fGain1 = 0.0;
  fDriftTime1.clear();
  fEnergyDepHeed1 = 0.0;
  fNClustersHeed1 = 0;
  fNElectronsHeed1 = 0;
  nsum1 = 0;
  fRadiusTravelled1.clear();
  fFinalX1.clear();
  fFinalY1.clear();
  fFinalZ1.clear();
  
  // Calculate a track with initial position and direction
  double x_cm = localPosition.x() / CLHEP::cm;
  double y_cm = localPosition.y() / CLHEP::cm;
  double z_cm = localPosition.z() / CLHEP::cm;
  // double x_cm = worldPosition.x() / CLHEP::cm;
  // double y_cm = worldPosition.y() / CLHEP::cm;
  // double z_cm = worldPosition.z() / CLHEP::cm;

  // printf("\n local positions %g %g %g\n", localPosition.x() / CLHEP::cm, localPosition.y() / CLHEP::cm, localPosition.z() / CLHEP::cm);
  // printf("\n world positions %g %g %g\n", worldPosition.x() / CLHEP::cm, worldPosition.y() / CLHEP::cm, worldPosition.z() / CLHEP::cm);

  double time = globalTime;

  double dx = localdir.x();
  double dy = localdir.y();
  double dz = localdir.z();
  
  double eKin_eV = ekin_keV * 1000;

  // Cluster coordinates
  double xc = 0., yc = 0., zc = 0., tc = 0.;
  // Number of electrons produced in a collision
  int nc = 0;
  // Energy loss in a collision
  double ec = 0.;
  // Dummy variable (not used at present)
  double extra = 0.;

  fEnergyDeposit1 += eKin_eV;
    
  // fTrackHeed->SetParticle(particleName);
  // fTrackHeed->SetKineticEnergy(ekin_eV);
  // fTrackHeed->SetMomentum(mommageV);
  // fTrackHeed->NewTrack(x_cm, y_cm, z_cm, time, dx, dy, dz);

  // printf("\nMAGNETIC FIELD %g %g %g, %g, %g, %g\n",pos[0], pos[1], pos[2],b[0]*1000.00,b[1]*1000.00,b[2]*1000.00);
  // printf("New heed track %g %g %g %g %g %g %g %g %g\n",x_cm, y_cm, z_cm, time, dx, dy, dz, ekin_eV, TMath::Sqrt(x_cm*x_cm+y_cm*y_cm));

  // Total energy loss along the track
  double esum = 0.;
  // Total number of electrons produced along the track
  int nsum = 0;
  int electroncounter = 0;

  // for (int cl = 0; cl < nc; cl++) {
    double xe, ye, ze, te;
    double ee, dxe, dye, dze;
    // fTrackHeed->GetElectron(cl, xe, ye, ze, te, ee, dxe, dye, dze);
    // nsum++;

    // fDrift->DriftElectron(xe, ye, ze, te);
    fDrift->DriftElectron(x_cm, y_cm, z_cm, time);

    // int NDriftPoints = fDrift->GetNumberOfDriftLinePoints();    
    double xf, yf, zf, tf = 0.0;
    // fDrift->GetDriftLinePoint(NDriftPoints-1,xf,yf,zf,tf);
    
    double xe1, ye1, ze1, te1;
    double xe2, ye2, ze2, te2;
    int status;
    fDrift->GetElectronEndpoint(0, xe1, ye1, ze1, te1, xe2, ye2, ze2,
    				te2, status);


    // // Drift electron using driftline rkf method
    // driftline->DriftElectron(x_cm,y_cm,z_cm,time);
    // double xf, yf, zf, tf = 0.0;
    // Int_t statusf=0;
    // driftline->GetEndPoint(xf, yf, zf, tf, statusf);
    // printf("Electron end point %g, %g, %g, %g\n",xf, yf, zf, tf);



    xf = xe2; yf = ye2; zf = ze2; tf = te2;
    
    double drifttime = tf-time;
    fDriftTime1.push_back(drifttime);
    double radius_start = TMath::Sqrt(x_cm*x_cm + y_cm*y_cm);
    double radius_end = TMath::Sqrt(xf*xf + yf*yf);
    double deltar = radius_end - radius_start;
    fRadiusTravelled1.push_back(deltar);
    electroncounter += 1;

    //calculate phi travelled and phi strip id
    G4ThreeVector vEStart;
    vEStart.setX(x_cm); vEStart.setY(y_cm); vEStart.setZ(z_cm);
    double EStartPhi = vEStart.phi();
    G4ThreeVector vEEnd;
    vEEnd.setX(xf); vEEnd.setY(yf); vEEnd.setZ(zf);
    double EEndPhi = vEEnd.phi();

    fFinalX1.push_back(xf);
    fFinalY1.push_back(yf);
    fFinalZ1.push_back(zf);


  //   }
  // }
  fEnergyDepHeed1 = esum/1.0e3;
  // printf("esum is %g", esum);
  fNClustersHeed1 = nsum;
  fNElectronsHeed1 = electroncounter;
  
      
  fGarfieldPhysics->DoIt(fDriftTime1, fEnergyDepHeed1, fNClustersHeed1, fNElectronsHeed1, fRadiusTravelled1, fFinalX1, fFinalY1, fFinalZ1);
  

  // }
  
  delete fMediumMagboltz;
  delete fSensor;
  delete fAvalanche;
  delete fDrift;
  delete fdriftline;
  delete fComponentAnalyticField;
  // delete fComponentConstantE;
  // delete fComponentConstantM;
  delete fTrackHeed;
  delete fGeometryRoot;
  delete fGeometrySimple;
  delete magFieldTemp;
  

  }

