#ifndef ABSORTIONCORRECTION_H_
#define ABSORTIONCORRECTION_H_

#include <opencv/cxcore.h>
#include <vector>
#include <map>

#include "../model/Model.h"
#include "../resources/InputParameters.h"
#include "../common/common.h"
#include "Reflection.h"
#include <zthread/PoolExecutor.h>

typedef enum DebugType { NONE = 0, DRAW_BEAM, DUMP_REFLECTION, DUMP_BATCH } DebugType;

typedef struct BeamPath {
    CvPoint3D diffractionPoint;
    //direct beam
    CvPoint3D loopStart;
    CvPoint3D liquidStart;
    CvPoint3D crystalStart;
    //diffracted beam
    CvPoint3D crystalEnd;
    CvPoint3D liquidEnd;
    CvPoint3D loopEnd;
} BeamPath;

inline BeamPath beamPath(CvPoint3D dp)
{
    BeamPath b;
    b.diffractionPoint = dp;
    b.loopStart = cvPoint3D(0, 0, 0);
    b.liquidStart = cvPoint3D(0, 0, 0);
    b.crystalStart = cvPoint3D(0, 0, 0);
    b.crystalEnd = cvPoint3D(0, 0, 0);
    b.liquidEnd = cvPoint3D(0, 0, 0);
    b.loopEnd = cvPoint3D(0, 0, 0);
    return b;
}

class AbsorptionCorrection {
  public:
    AbsorptionCorrection();
    AbsorptionCorrection(Model *);
    virtual ~ AbsorptionCorrection();

    BeamPath calculateBeamPath(CvPoint3D, float, CvPoint3D64f, const ReflectionItem *);
    void calculateTransmittance(vector < BeamPath >, ReflectionItem *);
    float calculateRealIntensity(float, float, float);
    float calculateRealSigma(float, float, float);
    float calculateAirPath(float, float, float);
    double getNormalisedVolumeConstants(int);

    Reflection *getReflectionData() const {
        return reflectionData;
    };
    void setReflectionData(Reflection * reflectionData_p) {
        reflectionData = reflectionData_p;
    };

    //Getter and Setter for DEBUG_TYPE
    DebugType getDEBUG_TYPE() const {
        return DEBUG_TYPE;
    };
    void setDEBUG_TYPE(DebugType DEBUG_TYPE_p) {
        DEBUG_TYPE = DEBUG_TYPE_p;
    };

    //Getter and Setter for debugReflectionItem
    ReflectionItem getDebugReflectionItem() const {
        return debugReflectionItem;
    };
    void setDebugReflectionItem(ReflectionItem debugReflectionItem_p) {
        debugReflectionItem = debugReflectionItem_p;
    };

    vector < CvPoint3D > getDiffractionCoordinates(int batch) {
        return diffractionCoordinates[batch];
    };

    float getIlluminatedVolume(int batch) {
        return illuminatedVolume[batch];
    };

    //Getter and Setter for unitCellVolume
    float getUnitCellVolume() const {
        return unitCellVolume;
    };
    void setUnitCellVolume(float unitCellVolume_p) {
        unitCellVolume = unitCellVolume_p;
    };

    void volumeInit();
    void volumeDump();
    void printStatus(int, int);

  protected:

    log4cplus::Logger myLogger; // = log4cplus::Logger::getInstance("abs");
    // sample associated to this absorption
    Model *sample;

    //calculated externly. They depend on: wavelength and materialparseIntegrateFileHeader
    const float absorptionCoefficientLiquid;
    const float absorptionCoefficientLoop;
    const float absorptionCoefficientCrystal;

    // number of pixels in a row to define the transitions between zones (e.g. crystal to liquid);

    BeamPath fillInputBeamPath(CvPoint3D, DataType, BeamPath &);
    BeamPath fillOutputBeamPath(CvPoint3D, DataType, BeamPath &);

    float magnitudeInMicrometers(CvPoint3D p1, CvPoint3D p2);

    Reflection *reflectionData;

    // For every phi, keeps an array of points that diffract 
    map < int, vector < CvPoint3D > >diffractionCoordinates;
    // For every phi, keeps the volume of the crystal that causes diffraction 
    map < int, float >illuminatedVolume;

    float averageIlluminatedVolume;
    float unitCellVolume;

    ZThread::PoolExecutor executor;

    DebugType DEBUG_TYPE;
    ReflectionItem debugReflectionItem;

};

#endif /*ABSORTIONCORRECTION_H_ */
