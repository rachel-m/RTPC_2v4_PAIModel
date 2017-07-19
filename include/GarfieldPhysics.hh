#ifndef GarfieldPhysics_h
#define GarfieldPhysics_h 1

#include <map>
#include <vector>
#include <iostream>

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


typedef std::pair<double, double> EnergyRange_keV;
typedef std::map< const std::string, EnergyRange_keV> MapParticlesEnergy;

class GarfieldElectron{
public:
  // GarfieldParticle(std::string particleName, double ekin_eV,double time, double x_cm,double y_cm,double z_cm, double dx,double dy,double dz):fParticleName(particleName), fEkin_MeV(ekin_eV/1000000), fTime(time), fx_mm(10*x_cm),fy_mm(10*y_cm), fz_mm(10*z_cm),  fdx(dx), fdy(dy), fdz(dz){}
	// ~GarfieldParticle(){};

  GarfieldElectron( double ekin_eV,double time, double x_cm,double y_cm,double z_cm, double dx,double dy,double dz):fEkin_MeV(ekin_eV/1000000), fTime(time), fx_mm(10*x_cm),fy_mm(10*y_cm), fz_mm(10*z_cm),  fdx(dx), fdy(dy), fdz(dz){}
	~GarfieldElectron(){};

	// std::string getParticleName(){return fParticleName;}
	double getX_mm() {return fx_mm;}
	double getY_mm(){return fy_mm;}
	double getZ_mm(){return fz_mm;}
	double getEkin_MeV(){return fEkin_MeV;}
	double getTime(){return fTime;}
	double getDX(){return fdx;}
	double getDY(){return fdy;}
	double getDZ(){return fdz;}


private:
	std::string fParticleName;
	double fEkin_MeV, fTime, fx_mm,fy_mm,fz_mm,fdx,fdy,fdz;


};


class GarfieldPhysics {
public:
	static GarfieldPhysics* GetInstance();
	static void Dispose();

	void InitializePhysics();
	void CreateGeometry();

	// void DoIt(std::string particleName, double ekin_eV,double time,
	// 		double x_cm, double y_cm, double z_cm, double dx, double dy, double dz);
  
  void DoIt(std::vector<double> drifttime, double energydepheed, double nclustersheed, double nelectronsheed, std::vector<double> radiustravelled, std::vector<double> finalX, std::vector<double> finalY, std::vector<double> finalZ);

	void AddParticleName(const std::string particleName, double ekin_min_keV, double ekin_max_keV);
	bool FindParticleName(const std::string name);
	bool FindParticleNameEnergy(std::string name, double ekin_keV);
	std::vector<GarfieldElectron*>* GetSecondaryElectrons();
	void DeleteSecondaryElectrons();
	// inline void EnableCreateSecondariesInGeant4() {createSecondariesInGeant4 = true;};
	// inline void DisableCreateSecondariesInGeant4() {createSecondariesInGeant4 = false;};
	// inline bool GetCreateSecondariesInGeant4() {return createSecondariesInGeant4;};
	inline double GetEnergyDeposit_MeV() {return fEnergyDeposit/1000000;};
	inline double GetAvalancheSize() {return fAvalancheSize;};
	inline double GetGain() {return fGain;};
	// void AddParticleName(const std::string particleName, double ekin_min_keV, double ekin_max_keV, std::string program);
	// bool FindParticleName(const std::string name, std::string program = "garfield");
	// bool FindParticleNameEnergy(std::string name, double ekin_MeV, std::string program = "garfield");
	// double GetMinEnergyMeVParticle(std::string name, std::string program = "garfield");
	// double GetMaxEnergyMeVParticle(std::string name, std::string program = "garfield");
	// void SetIonizationModel(std::string model, bool useDefaults=true);
	// std::string GetIonizationModel();
	// std::vector<GarfieldParticle*>* GetSecondaryParticles();
	// void DeleteSecondaryParticles();
	// inline void EnableCreateSecondariesInGeant4(bool flag) {createSecondariesInGeant4 = flag;};
	// inline bool GetCreateSecondariesInGeant4() {return createSecondariesInGeant4;};
	// inline double GetEnergyDeposit_MeV() {return fEnergyDeposit/1000000;};
	// inline double GetAvalancheSize() {return fAvalancheSize;};
	// inline double GetGain() {return fGain;};
        inline std::vector<double> GetDriftTime() {return fDriftTime;};
        inline double GetEnergyDepHeed() {return fEnergyDepHeed;};
        inline int GetNClustersHeed() {return fNClustersHeed;};
        inline int GetNElectronsHeed() {return fNElectronsHeed;};
        inline  std::vector<double> GetRadiusTravelled() {return fRadiusTravelled;};
        inline  std::vector<double> GetFinalX() {return fFinalX;};
        inline  std::vector<double> GetFinalY() {return fFinalY;};
        inline  std::vector<double> GetFinalZ() {return fFinalZ;};

	// inline void Clear() {fEnergyDeposit=0;fAvalancheSize=0;fGain=0;nsum=0;}
  inline void Clear() {fEnergyDeposit=0;fAvalancheSize=0;fGain=0;nsum=0;fDriftTime.clear();fEnergyDepHeed=0;fNClustersHeed=0;fNElectronsHeed=0;fRadiusTravelled.clear();fFinalX.clear();fFinalY.clear();fFinalZ.clear();}

private:
	GarfieldPhysics();
	~GarfieldPhysics();

	// std::string fIonizationModel;

	static GarfieldPhysics* fGarfieldPhysics;
  	MapParticlesEnergy* fMapParticlesEnergy;

	// MapParticlesEnergy* fMapParticlesEnergyGeant4;
	// MapParticlesEnergy* fMapParticlesEnergyGarfield;
  
	std::vector<GarfieldElectron*>* fSecondaryElectrons;

        // std::vector<GarfieldParticle*>* fSecondaryParticles;
        double fEnergyDeposit;
        double fAvalancheSize;
        double fGain;
        std::vector<double> fDriftTime;
        double fEnergyDepHeed;
        int fNClustersHeed;
        int fNElectronsHeed;
        int nsum;
        std::vector<double> fRadiusTravelled;
        std::vector<double> fFinalX;
        std::vector<double> fFinalY;
        std::vector<double> fFinalZ;


	// bool createSecondariesInGeant4;



};
#endif /* GARFIELDMODELCONFIG_HH_ */



