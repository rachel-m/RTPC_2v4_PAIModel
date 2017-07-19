/// \brief Implementation of the GarfieldPhysics class
#include "GarfieldPhysics.hh"

#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TMath.h"

#include "GarfieldAnalysis.hh"
#include "G4SystemOfUnits.hh"

#include "TOSCAField2D.hh"

GarfieldPhysics* GarfieldPhysics::fGarfieldPhysics = 0;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

GarfieldPhysics* GarfieldPhysics::GetInstance() {
	if (!fGarfieldPhysics) {
		fGarfieldPhysics = new GarfieldPhysics();
	}
	return fGarfieldPhysics;
}

void GarfieldPhysics::Dispose() {
	delete fGarfieldPhysics;
	fGarfieldPhysics = 0;
}

GarfieldPhysics::GarfieldPhysics() {
	// fMapParticlesEnergyGeant4 = new MapParticlesEnergy();
	// fMapParticlesEnergyGarfield = new MapParticlesEnergy();
	// fSecondaryParticles = new std::vector<GarfieldParticle*>();
	fMapParticlesEnergy = new MapParticlesEnergy();
	fSecondaryElectrons = new std::vector<GarfieldElectron*>();

	// fMediumMagboltz = 0;
	// fSensor = 0;
	// fAvalanche = 0;
	// fDrift = 0;
	// fComponentAnalyticField = 0;
	// // fComponentConstantE = 0;
	// // fComponentConstantM = 0;
	// fTrackHeed = 0;
	// fGeoManager = 0;
	// fGeometryRoot = 0;
	// fGeometrySimple = 0;
	// fTube = 0;
	
	// createSecondariesInGeant4 = false;
	
	// fIonizationModel = "Heed";
	// // fIonizationModel = "PAIPhot";

}

GarfieldPhysics::~GarfieldPhysics() {
	// delete fMapParticlesEnergyGeant4;
	// delete fMapParticlesEnergyGarfield;
	// DeleteSecondaryParticles();
	// delete fSecondaryParticles;
	delete fMapParticlesEnergy;
	// DeleteSecondaryElectrons();
	delete fSecondaryElectrons;
	// delete fMediumMagboltz;
	// delete fSensor;
	// delete fAvalanche;
	// delete fDrift;
	// delete fComponentAnalyticField;
	// // delete fComponentConstantE;
	// // delete fComponentConstantM;
	// delete fTrackHeed;
	// delete fGeometryRoot;
	// delete fGeometrySimple;

	std::cout << "Deconstructor GarfieldPhysics" << std::endl;
}



void GarfieldPhysics::AddParticleName(const std::string particleName,
		double ekin_min_keV, double ekin_max_keV) {
	if (ekin_min_keV >= ekin_max_keV) {
		std::cout << "Ekin_min=" << ekin_min_keV
				<< " keV is larger than Ekin_max=" << ekin_max_keV << " keV"
				<< std::endl;
		return;
	}
	std::cout << "Garfield model is applicable for G4Particle " << particleName
			<< " between " << ekin_min_keV << " keV and " << ekin_max_keV
			<< " keV" << std::endl;
	fMapParticlesEnergy->insert(
			std::make_pair(particleName,
					std::make_pair(ekin_min_keV, ekin_max_keV)));

}


bool GarfieldPhysics::FindParticleName(std::string name) {
	MapParticlesEnergy::iterator it;
	it = fMapParticlesEnergy->find(name);
	if (it != fMapParticlesEnergy->end()) {
		return true;
	}
	return false;
}

bool GarfieldPhysics::FindParticleNameEnergy(std::string name,
		double ekin_keV) {
	MapParticlesEnergy::iterator it;
	it = fMapParticlesEnergy->find(name);
	if (it != fMapParticlesEnergy->end()) {
		EnergyRange_keV range = it->second;
		if (range.first <= ekin_keV && range.second >= ekin_keV) {
			return true;
		}
	}
	return false;
}


void GarfieldPhysics::InitializePhysics() {


}

void GarfieldPhysics::CreateGeometry() {

  
}



void GarfieldPhysics::DoIt(std::vector<double> DriftTime, double EnergyDepHeed, double NClustersHeed, double NElectronsHeed, std::vector<double> RadiusTravelled, std::vector<double>, std::vector<double> finalx, std::vector<double> finaly, std::vector<double> finalz ) {

  // fDriftTime = DriftTime;
  int drifttimesize = DriftTime.size();
  for(int dt=0; dt<drifttimesize; dt++){
    fDriftTime.push_back(DriftTime[dt]);
  }
  // printf("garfphysics drift time size = %d\n",drifttimesize);
  
  fEnergyDepHeed = EnergyDepHeed;
  fNClustersHeed = NClustersHeed;
  fNElectronsHeed = NElectronsHeed;
  
  // fRadiusTravelled = RadiusTravelled;
  // fzTravelled = zTravelled;
  int radiustravelledsize = RadiusTravelled.size();
  for(int rt=0; rt<radiustravelledsize; rt++){
    fRadiusTravelled.push_back(RadiusTravelled[rt]);
  }
  // printf("garfphysics radius travelled size = %d\n",radiustravelledsize);
  
  int finalxsize = finalx.size();
  for(int ix=0; ix<finalxsize; ix++){
    fFinalX.push_back(finalx[ix]);
  }
  
  int finalysize = finaly.size();
  for(int iy=0; iy<finalysize; iy++){
    fFinalY.push_back(finaly[iy]);
  }
  
  int finalzsize = finalz.size();
  for(int iz=0; iz<finalzsize; iz++){
    fFinalZ.push_back(finalz[iz]);
  }
   
  
}


std::vector<GarfieldElectron*>* GarfieldPhysics::GetSecondaryElectrons() {
	return fSecondaryElectrons;
}

void GarfieldPhysics::DeleteSecondaryElectrons() {
	if (!fSecondaryElectrons->empty()) {
		fSecondaryElectrons->erase(fSecondaryElectrons->begin(),
				fSecondaryElectrons->end());
	}
}

