#ifndef INPUTPARAMETERS_H_
#define INPUTPARAMETERS_H_

#include "InputImageParameters.h"
#include "../common/common.h"

class InputParameters {
public:

	InputParameters(char *);

	virtual ~ InputParameters();

	int parseConfigFile();
	int getDiffractedBeams() const {
		return nDiffractedBeams;
	}

	char *getOffCrystalModelFilePath() const {
		return offCrystalModelFilePath;
	}

	char *getOffLoopModelFilePath() const {
		return offLoopModelFilePath;
	}

	char *getOffVolumeModelFilePath() const {
		return offVolumeModelFilePath;
	}

	void setDiffractedBeams(int nDiffractedBeams) {
		this->nDiffractedBeams = nDiffractedBeams;
	}

	void setOffCrystalModelFilePath(char *offCrystalModelFilePath) {
		this->offCrystalModelFilePath = offCrystalModelFilePath;
	}

	void setOffLoopModelFilePath(char *offLoopModelFilePath) {
		this->offLoopModelFilePath = offLoopModelFilePath;
	}

	void setOffVolumeModelFilePath(char *offVolumeModelFilePath) {
		this->offVolumeModelFilePath = offVolumeModelFilePath;
	}

	//static int stringToInt(char *s);

	//Getter and Setter for visibleSet
	//	InputImageParameters *getVisibleSet() const {
	//		return visibleSet;
	//	}
	//	;
	//	void setVisibleSet(InputImageParameters * visibleSet_p) {
	//		visibleSet = visibleSet_p;
	//	}
	//	;
	//
	//	//Getter and Setter for laserSet
	//	InputImageParameters *getLaserSet() const {
	//		return laserSet;
	//	}
	//	;
	//	void setLaserSet(InputImageParameters * laserSet_p) {
	//		laserSet = laserSet_p;
	//	}
	//	;

	//Getter and Setter for volumeSet
	InputImageParameters *getVolumeSet() const {
		return volumeSet;
	}
	;
	void setVolumeSet(InputImageParameters * volumeSet_p) {
		volumeSet = volumeSet_p;
	}
	;

	//Getter and Setter for crystalSet
	InputImageParameters *getCrystalSet() const {
		return crystalSet;
	}
	;
	void setCrystalSet(InputImageParameters * crystalSet_p) {
		crystalSet = crystalSet_p;
	}
	;

	//Getter and Setter for loopSet
	InputImageParameters *getLoopSet() const {
		return loopSet;
	}
	;
	void setLoopSet(InputImageParameters * loopSet_p) {
		loopSet = loopSet_p;
	}
	;


	//Getter and Setter for outputReflectionsFilePath
	char *getOutputReflectionsFilePath() const {
		return outputReflectionsFilePath;
	}
	;
	void setOutputReflectionsFilePath(char *outputReflectionsFilePath_p) {
		outputReflectionsFilePath = outputReflectionsFilePath_p;
	}
	;


	//Getter and Setter for outputCsvFilePath
	char *getOutputCsvFilePath() const {
		return outputCsvFilePath;
	}
	;
	void setOutputCsvFilePath(char *outputCsvFilePath_p) {
		outputCsvFilePath = outputCsvFilePath_p;
	}
	;

	//Getter and Setter for beamSizeX
	int getBeamSizeX() const {
		return beamSizeX;
	}
	;
	void setBeamSizeX(int beamSizeX_p) {
		beamSizeX = beamSizeX_p;
	}
	;

	//Getter and Setter for beamSizeY
	int getBeamSizeY() const {
		return beamSizeY;
	}
	;
	void setBeamSizeY(int beamSizeY_p) {
		beamSizeY = beamSizeY_p;
	}
	;

	//Getter and Setter for nDiffractedBeams
	int getNDiffractedBeams() const {
		return nDiffractedBeams;
	}
	;
	void setNDiffractedBeams(int nDiffractedBeams_p) {
		nDiffractedBeams = nDiffractedBeams_p;
	}
	;

	//Getter and Setter for absorptionCoefficientLiquid
	float getAbsorptionCoefficientLiquid() const {
		return absorptionCoefficientLiquid;
	}
	;
	void setAbsorptionCoefficientLiquid(float absorptionCoefficientLiquid_p) {
		absorptionCoefficientLiquid = absorptionCoefficientLiquid_p;
	}
	;

	//Getter and Setter for absorptionCoefficientLoop
	float getAbsorptionCoefficientLoop() const {
		return absorptionCoefficientLoop;
	}
	;
	void setAbsorptionCoefficientLoop(float absorptionCoefficientLoop_p) {
		absorptionCoefficientLoop = absorptionCoefficientLoop_p;
	}
	;

	//Getter and Setter for absorptionCoefficientCrystal
	float getAbsorptionCoefficientCrystal() const {
		return absorptionCoefficientCrystal;
	}
	;
	void setAbsorptionCoefficientCrystal(float absorptionCoefficientCrystal_p) {
		absorptionCoefficientCrystal = absorptionCoefficientCrystal_p;
	}
	;

	//Getter and Setter for steps
	char *getSteps() const {
		return steps;
	}
	;
	void setSteps(char *steps_p) {
		steps = steps_p;
	}
	;

	//Getter and Setter for pixelsPerMicromX
	float getPixelsPerMicromX() const {
		return pixelsPerMicromX;
	}
	;
	void setPixelsPerMicromX(float pixelsPerMicromX_p) {
		pixelsPerMicromX = pixelsPerMicromX_p;
	}
	;

	//Getter and Setter for pixelsPerMicromY
	float getPixelsPerMicromY() const {
		return pixelsPerMicromY;
	}
	;
	void setPixelsPerMicromY(float pixelsPerMicromY_p) {
		pixelsPerMicromY = pixelsPerMicromY_p;
	}
	;

	//Getter and Setter for pixelsPerMicromZ
	float getPixelsPerMicromZ() const {
		return pixelsPerMicromZ;
	}
	;
	void setPixelsPerMicromZ(float pixelsPerMicromZ_p) {
		pixelsPerMicromZ = pixelsPerMicromZ_p;
	}
	;

	//Getter and Setter for pixelSize
	float getPixelSize() const {
		return pixelSize;
	}
	;
	void setPixelSize(float pixelSize_p) {
		pixelSize = pixelSize_p;
	}
	;

	//Getter and Setter for distance
	float getDistance() const {
		return distance;
	}
	;
	void setDistance(float distance_p) {
		distance = distance_p;
	}
	;

	//Getter and Setter for oscRange
	float getOscRange() const {
		return oscRange;
	}
	;
	void setOscRange(float oscRange_p) {
		oscRange = oscRange_p;
	}
	;

	//Getter and Setter for startAngle
	float getStartAngle() const {
		return startAngle;
	}
	;
	void setStartAngle(float startAngle_p) {
		startAngle = startAngle_p;
	}
	;

	//Getter and Setter for beamCenterX
	float getBeamCenterX() const {
		return beamCenterX;
	}
	;
	void setBeamCenterX(float beamCenterX_p) {
		beamCenterX = beamCenterX_p;
	}
	;

	//Getter and Setter for beamCenterY
	float getBeamCenterY() const {
		return beamCenterY;
	}
	;
	void setBeamCenterY(float beamCenterY_p) {
		beamCenterY = beamCenterY_p;
	}
	;

	//Getter and Setter for absorptionCoefficientAir
	float getAbsorptionCoefficientAir() const {
		return absorptionCoefficientAir;
	}
	;
	void setAbsorptionCoefficientAir(float absorptionCoefficientAir_p) {
		absorptionCoefficientAir = absorptionCoefficientAir_p;
	}
	;

	//Getter and Setter for totalBatches
	int getTotalBatches() const {
		return totalBatches;
	}
	;
	void setTotalBatches(int totalBatches_p) {
		totalBatches = totalBatches_p;
	}
	;

	//Getter and Setter for negativeIntensities
	int getNegativeIntensities() const {
		return negativeIntensities;
	}
	;
	void setNegativeIntensities(int negativeIntensities_p) {
		negativeIntensities = negativeIntensities_p;
	}
	;
	//Getter and Setter for modelSizeX
	int getModelSizeX() const {
		return modelSizeX;
	}
	;
	void setModelSizeX(int modelSizeX_p) {
		modelSizeX = modelSizeX_p;
	}
	;

	//Getter and Setter for modelSizeY
	int getModelSizeY() const {
		return modelSizeY;
	}
	;
	void setModelSizeY(int modelSizeY_p) {
		modelSizeY = modelSizeY_p;
	}
	;

	//Getter and Setter for modelSizeZ
	int getModelSizeZ() const {
		return modelSizeZ;
	}
	;
	void setModelSizeZ(int modelSizeZ_p) {
		modelSizeZ = modelSizeZ_p;
	}
	;

	//Getter and Setter for worldDimX
	int getWorldDimX() const {
		return worldDimX;
	}
	;
	void setWorldDimX(int worldDimX_p) {
		worldDimX = worldDimX_p;
	}
	;

	//Getter and Setter for worldDimY
	int getWorldDimY() const {
		return worldDimY;
	}
	;
	void setWorldDimY(int worldDimY_p) {
		worldDimY = worldDimY_p;
	}
	;

	//Getter and Setter for worldDimZ
	int getWorldDimZ() const {
		return worldDimZ;
	}
	;
	void setWorldDimZ(int worldDimZ_p) {
		worldDimZ = worldDimZ_p;
	}
	;

	//Getter and Setter for inrCrystalModelFilePath
	char *getInrCrystalModelFilePath() const {
		return inrCrystalModelFilePath;
	}
	;
	void setInrCrystalModelFilePath(char *inrCrystalModelFilePath_p) {
		inrCrystalModelFilePath = inrCrystalModelFilePath_p;
	}
	;

	//Getter and Setter for inrLoopModelFilePath
	char *getInrLoopModelFilePath() const {
		return inrLoopModelFilePath;
	}
	;
	void setInrLoopModelFilePath(char *inrLoopModelFilePath_p) {
		inrLoopModelFilePath = inrLoopModelFilePath_p;
	}
	;

	//Getter and Setter for inrVolumeModelFilePath
	char *getInrVolumeModelFilePath() const {
		return inrVolumeModelFilePath;
	}
	;
	void setInrVolumeModelFilePath(char *inrVolumeModelFilePath_p) {
		inrVolumeModelFilePath = inrVolumeModelFilePath_p;
	}
	;

	//Getter and Setter for modelToProcess
	char *getModelToProcess() const {
		return modelToProcess;
	}
	;
	void setModelToProcess(char *modelToProcess_p) {
		modelToProcess = modelToProcess_p;
	}
	;
	//Getter and Setter for outputInrGlobalmodelFilePath
	char *getOutputInrGlobalmodelFilePath() const { return outputInrGlobalmodelFilePath; };
	void setOutputInrGlobalmodelFilePath(char *outputInrGlobalmodelFilePath_p) { outputInrGlobalmodelFilePath = outputInrGlobalmodelFilePath_p; };

	//Getter and Setter for inputInrGlobalmodelFilePath
	char *getInputInrGlobalmodelFilePath() const { return inputInrGlobalmodelFilePath; };
	void setInputInrGlobalmodelFilePath(char *inputInrGlobalmodelFilePath_p) { inputInrGlobalmodelFilePath = inputInrGlobalmodelFilePath_p; };

	//Getter and Setter for inputReflectionsFilePath
	char *getInputReflectionsFilePath() const { return inputReflectionsFilePath; };
	void setInputReflectionsFilePath(char *inputReflectionsFilePath_p) { inputReflectionsFilePath = inputReflectionsFilePath_p; };


private:
	//
	dictionary * d;
	log4cplus::Logger myLogger; // = log4cplus::Logger::getInstance("parameters");
	//image sets
	//	InputImageParameters *visibleSet;
	//	InputImageParameters *laserSet;

	InputImageParameters *volumeSet;
	InputImageParameters *crystalSet;
	InputImageParameters *loopSet;
	//global parameters common to all program


	char *inputReflectionsFilePath;
	char *outputReflectionsFilePath;
	char *outputInrGlobalmodelFilePath;
	char *inputInrGlobalmodelFilePath;
	char *outputCsvFilePath;
	char *offCrystalModelFilePath;
	char *offLoopModelFilePath;
	char *offVolumeModelFilePath;
	char *inrCrystalModelFilePath;
	char *inrLoopModelFilePath;
	char *inrVolumeModelFilePath;
	char *modelToProcess;
	int beamSizeX;
	int beamSizeY;
	int nDiffractedBeams;
	// Linear absorption coefficients
	float absorptionCoefficientLiquid;
	float absorptionCoefficientLoop;
	float absorptionCoefficientCrystal;
	float absorptionCoefficientAir;
	float pixelsPerMicromX;
	float pixelsPerMicromY;
	float pixelsPerMicromZ;
	int modelSizeX;
	int modelSizeY;
	int modelSizeZ;
	int worldDimX;
	int worldDimY;
	int worldDimZ;
	char *steps;
	//constants
	float pixelSize;
	float distance;
	float oscRange;
	float startAngle;
	float beamCenterX;
	float beamCenterY;
	int totalBatches;

	int negativeIntensities;

};

#endif /*INPUTPARAMETERS_H_ */
