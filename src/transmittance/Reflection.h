#ifndef REFLECTION_H_
#define REFLECTION_H_

#include <vector>
#include <iostream>
#include <ostream>
#include <string>

class ReflectionItem {
public:
	ReflectionItem();
	virtual ~ ReflectionItem();
	ReflectionItem(int h_p, int k_p, int l_p, int misym_p, int batch_p);

	bool operator==(const ReflectionItem & r) const;
	bool operator<=(const ReflectionItem & r) const;
	bool isBatch(int);

	//TODO
	friend std::ostream & operator<<(std::ostream & os,
			const ReflectionItem & ri) {

		//os.precision(4);
		os << ri.h << ", ";
		os << ri.k << ", ";
		os << ri.l << ", ";
		os << ri.misym << ", ";
		os << ri.batch << ", ";
		os << ri.xDet << ", ";
		os << ri.yDet << ", ";
		os << ri.iObs << ", ";
		os << ri.sigiObs << ", ";
		os << ri.iprObs << ", ";
		os << ri.sigiprObs << ", ";
		os << ri.resolution << ", ";
		os << ri.iCorr << ", ";
		os << ri.sigiCorr << ", ";
		os << ri.iprCorr << ", ";
		os << ri.sigiprCorr << ", ";
		os << ri.xDetReal << ", ";
		os << ri.yDetReal << ", ";
		os << ri.totalIncidentBeamMagnitude << ", ";
		os << ri.totalDiffractedBeamMagnitude << ", ";
		os << ri.totalBeamTransmittance << ", ";
		os << ri.crystalIncidentBeamMagnitude << ", ";
		os << ri.crystalDiffractedBeamMagnitude << ", ";
		os << ri.crystalBeamTransmittance << ", ";
		os << ri.liquidIncidentBeamMagnitude << ", ";
		os << ri.liquidDiffractedBeamMagnitude << ", ";
		os << ri.liquidBeamTransmittance << ", ";
		os << ri.loopIncidentBeamMagnitude << ", ";
		os << ri.loopDiffractedBeamMagnitude << ", ";
		os << ri.loopBeamTransmittance << ", ";
		os << ri.airIncidentBeamMagnitude << ", ";
		os << ri.airDiffractedBeamMagnitude << ", ";
		os << ri.airBeamTransmittance << ", ";
		os << ri.crystalVolume;
		os << "\n";

		return os;
	}

	//Getter and Setter for h 
	int getH() const {
		return h;
	}
	;
	void setH(int h_p) {
		h = h_p;
	}
	;

	//Getter and Setter for k
	int getK() const {
		return k;
	}
	;
	void setK(int k_p) {
		k = k_p;
	}
	;

	//Getter and Setter for l
	int getL() const {
		return l;
	}
	;
	void setL(int l_p) {
		l = l_p;
	}
	;

	//Getter and Setter for misym
	int getMisym() const {
		return misym;
	}
	;
	void setMisym(int misym_p) {
		misym = misym_p;
	}
	;

	//Getter and Setter for batch
	int getBatch() const {
		return batch;
	}
	;
	void setBatch(int batch_p) {
		batch = batch_p;
	}
	;

	//Getter and Setter for xDet
	float getXDet() const {
		return xDet;
	}
	;
	void setXDet(float xDet_p) {
		xDet = xDet_p;
	}
	;

	//Getter and Setter for yDet
	float getYDet() const {
		return yDet;
	}
	;
	void setYDet(float yDet_p) {
		yDet = yDet_p;
	}
	;

	//Getter and Setter for iObs
	double getIObs() const {
		return iObs;
	}
	;
	void setIObs(double iObs_p) {
		iObs = iObs_p;
	}
	;

	//Getter and Setter for sigmaObs
	double getSigiObs() const {
		return sigiObs;
	}
	;
	void setSigiObs(double sigmaObs_p) {
		sigiObs = sigmaObs_p;
	}
	;

	//Getter and Setter for iCorr
	double getICorr() const {
		return iCorr;
	}
	;
	void setICorr(double iCorr_p) {
		iCorr = iCorr_p;
	}
	;

	//Getter and Setter for sigmaCorr
	double getSigiCorr() const {
		return sigiCorr;
	}
	;
	void setSigiCorr(double sigmaCorr_p) {
		sigiCorr = sigmaCorr_p;
	}
	;

	//Getter and Setter for totalIncidentBeamMagnitude
	double getTotalIncidentBeamMagnitude() const {
		return totalIncidentBeamMagnitude;
	}
	;
	void setTotalIncidentBeamMagnitude(double totalIncidentBeamMagnitude_p) {
		totalIncidentBeamMagnitude = totalIncidentBeamMagnitude_p;
	}
	;

	//Getter and Setter for totalDiffractedBeamMagnitude
	double getTotalDiffractedBeamMagnitude() const {
		return totalDiffractedBeamMagnitude;
	}
	;
	void setTotalDiffractedBeamMagnitude(double totalDiffractedBeamMagnitude_p) {
		totalDiffractedBeamMagnitude = totalDiffractedBeamMagnitude_p;
	}
	;

	//Getter and Setter for totalBeamMagnitude
	double getTotalBeamMagnitude() const {
		return totalDiffractedBeamMagnitude + totalIncidentBeamMagnitude;
	}
	;

	//Getter and Setter for totalBeamTransmittance
	double getTotalBeamTransmittance() const {
		return totalBeamTransmittance;
	}
	;
	void setTotalBeamTransmittance(double totalBeamTransmittance_p) {
		totalBeamTransmittance = totalBeamTransmittance_p;
	}
	;

	//Getter and Setter for crystalIncidentBeamMagnitude
	double getCrystalIncidentBeamMagnitude() const {
		return crystalIncidentBeamMagnitude;
	}
	;
	void setCrystalIncidentBeamMagnitude(double crystalIncidentBeamMagnitude_p) {
		crystalIncidentBeamMagnitude = crystalIncidentBeamMagnitude_p;
	}
	;

	//Getter and Setter for crystalDiffractedBeamMagnitude
	double getCrystalDiffractedBeamMagnitude() const {
		return crystalDiffractedBeamMagnitude;
	}
	;
	void setCrystalDiffractedBeamMagnitude(
			double crystalDiffractedBeamMagnitude_p) {
		crystalDiffractedBeamMagnitude = crystalDiffractedBeamMagnitude_p;
	}
	;

	//Getter and Setter for crystalBeamMagnitude
	double getCrystalBeamMagnitude() const {
		return crystalIncidentBeamMagnitude + crystalDiffractedBeamMagnitude;
	}
	;

	//Getter and Setter for crystalBeamTransmittance
	double getCrystalBeamTransmittance() const {
		return crystalBeamTransmittance;
	}
	;
	void setCrystalBeamTransmittance(double crystalBeamTransmittance_p) {
		crystalBeamTransmittance = crystalBeamTransmittance_p;
	}
	;

	//Getter and Setter for liquidIncidentBeamMagnitude
	double getLiquidIncidentBeamMagnitude() const {
		return liquidIncidentBeamMagnitude;
	}
	;
	void setLiquidIncidentBeamMagnitude(double liquidIncidentBeamMagnitude_p) {
		liquidIncidentBeamMagnitude = liquidIncidentBeamMagnitude_p;
	}
	;

	//Getter and Setter for liquidDiffractedBeamMagnitude
	double getLiquidDiffractedBeamMagnitude() const {
		return liquidDiffractedBeamMagnitude;
	}
	;
	void setLiquidDiffractedBeamMagnitude(double liquidDiffractedBeamMagnitude_p) {
		liquidDiffractedBeamMagnitude = liquidDiffractedBeamMagnitude_p;
	}
	;

	//Getter and Setter for liquidBeamMagnitude
	double getLiquidBeamMagnitude() const {
		return liquidIncidentBeamMagnitude + liquidDiffractedBeamMagnitude;
	}
	;

	//Getter and Setter for liquidBeamTransmittance
	double getLiquidBeamTransmittance() const {
		return liquidBeamTransmittance;
	}
	;
	void setLiquidBeamTransmittance(double liquidBeamTransmittance_p) {
		liquidBeamTransmittance = liquidBeamTransmittance_p;
	}
	;

	//Getter and Setter for loopIncidentBeamMagnitude
	double getLoopIncidentBeamMagnitude() const {
		return loopIncidentBeamMagnitude;
	}
	;
	void setLoopIncidentBeamMagnitude(double loopIncidentBeamMagnitude_p) {
		loopIncidentBeamMagnitude = loopIncidentBeamMagnitude_p;
	}
	;

	//Getter and Setter for loopDiffractedBeamMagnitude
	double getLoopDiffractedBeamMagnitude() const {
		return loopDiffractedBeamMagnitude;
	}
	;
	void setLoopDiffractedBeamMagnitude(double loopDiffractedBeamMagnitude_p) {
		loopDiffractedBeamMagnitude = loopDiffractedBeamMagnitude_p;
	}
	;

	//Getter and Setter for loopBeamMagnitude
	double getLoopBeamMagnitude() const {
		return loopIncidentBeamMagnitude + loopDiffractedBeamMagnitude;
	}
	;

	//Getter and Setter for loopBeamTransmittance
	double getLoopBeamTransmittance() const {
		return loopBeamTransmittance;
	}
	;
	void setLoopBeamTransmittance(double loopBeamTransmittance_p) {
		loopBeamTransmittance = loopBeamTransmittance_p;
	}
	;

	//Getter and Setter for airIncidentBeamMagnitude
	double getAirIncidentBeamMagnitude() const {
		return airIncidentBeamMagnitude;
	}
	;
	void setAirIncidentBeamMagnitude(double airIncidentBeamMagnitude_p) {
		airIncidentBeamMagnitude = airIncidentBeamMagnitude_p;
	}
	;

	//Getter and Setter for airDiffractedBeamMagnitude
	double getAirDiffractedBeamMagnitude() const {
		return airDiffractedBeamMagnitude;
	}
	;
	void setAirDiffractedBeamMagnitude(double airDiffractedBeamMagnitude_p) {
		airDiffractedBeamMagnitude = airDiffractedBeamMagnitude_p;
	}
	;

	//Getter and Setter for airBeamMagnitude
	double getAirBeamMagnitude() const {
		return airIncidentBeamMagnitude + airDiffractedBeamMagnitude;
	}
	;

	//Getter and Setter for airBeamTransmittance
	double getAirBeamTransmittance() const {
		return airBeamTransmittance;
	}
	;
	void setAirBeamTransmittance(double airBeamTransmittance_p) {
		airBeamTransmittance = airBeamTransmittance_p;
	}
	;

	//Getter and Setter for resolution
	double getResolution() const {
		return resolution;
	}
	;
	void setResolution(double resolution_p) {
		resolution = resolution_p;
	}
	;

	//Getter and Setter for nBeams
	float getCrystalVolume() const {
		return crystalVolume;
	}
	;
	void setCrystalVolume(float crystalVolume_p) {
		crystalVolume = crystalVolume_p;
	}
	;

	//Getter and Setter for xDetReal
	double getXDetReal() const {
		return xDetReal;
	}
	;
	void setXDetReal(double xDetReal_p) {
		xDetReal = xDetReal_p;
	}
	;

	//Getter and Setter for yDetReal
	double getYDetReal() const {
		return yDetReal;
	}
	;
	void setYDetReal(double yDetReal_p) {
		yDetReal = yDetReal_p;
	}
	;

	//Getter and Setter for iprObs
	double getIprObs() const {
		return iprObs;
	}
	;
	void setIprObs(double iprObs_p) {
		iprObs = iprObs_p;
	}
	;

	//Getter and Setter for sigiprObs
	double getSigiprObs() const {
		return sigiprObs;
	}
	;
	void setSigiprObs(double sigiprObs_p) {
		sigiprObs = sigiprObs_p;
	}
	;

	//Getter and Setter for iprCorr
	double getIprCorr() const {
		return iprCorr;
	}
	;
	void setIprCorr(double iprCorr_p) {
		iprCorr = iprCorr_p;
	}
	;

	//Getter and Setter for sigiprCorr
	double getSigiprCorr() const {
		return sigiprCorr;
	}
	;
	void setSigiprCorr(double sigiprCorr_p) {
		sigiprCorr = sigiprCorr_p;
	}
	;

private:
	// red from mtzfile
	int h;
	int k;
	int l;
	int misym;
	int batch;
	float xDet;
	float yDet;

	double iObs;
	double sigiObs;
	double iprObs;
	double sigiprObs;

	double resolution;

	double iCorr;
	double sigiCorr;
	double iprCorr;
	double sigiprCorr;

	double xDetReal;
	double yDetReal;

	double totalIncidentBeamMagnitude;
	double totalDiffractedBeamMagnitude;
	double totalBeamTransmittance;

	double crystalIncidentBeamMagnitude;
	double crystalDiffractedBeamMagnitude;
	double crystalBeamTransmittance;

	double liquidIncidentBeamMagnitude;
	double liquidDiffractedBeamMagnitude;
	double liquidBeamTransmittance;

	double loopIncidentBeamMagnitude;
	double loopDiffractedBeamMagnitude;
	double loopBeamTransmittance;

	double airIncidentBeamMagnitude;
	double airDiffractedBeamMagnitude;
	double airBeamTransmittance;

	float crystalVolume;

};

class Reflection {
public:
	Reflection();
	virtual ~ Reflection();

	void appendItem(ReflectionItem * r);
	void csvDump(char *filename);


	friend std::ostream & operator<<(std::ostream & os, Reflection & r) {

		std::vector < ReflectionItem * >::iterator iter;

		os
				<< "h, k, l, misym, batch, xDet, yDet, iObs, sigiObs, iprObs, sigiprObs, resolution, iCorr, sigiCorr, iprCorr, sigiprCorr, xDetReal, yDetReal, totalIncidentBeamMagnitude, totalDiffractedBeamMagnitude, totalBeamTransmittance, crystalIncidentBeamMagnitude, crystalDiffractedBeamMagnitude, crystalBeamTransmittance, liquidIncidentBeamMagnitude, liquidDiffractedBeamMagnitude, liquidBeamTransmittance, loopIncidentBeamMagnitude, loopDiffractedBeamMagnitude, loopBeamTransmittance, airIncidentBeamMagnitude, airDiffractedBeamMagnitude, airBeamTransmittance, crystalVolume\n";

		for (iter = r.reflectionList.begin(); iter != r.reflectionList.end(); iter++) {
			ReflectionItem *item = *iter;
			//            if (item->getBatch() == 1)
			os << *item;
		};

		return os;
	}
	;

private:
	std::vector < ReflectionItem * > reflectionList;

};

#endif /*REFLECTION_H_ */
