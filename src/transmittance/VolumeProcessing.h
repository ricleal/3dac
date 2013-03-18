#ifndef VOLUMEPROCESSING_H_
#define VOLUMEPROCESSING_H_

#include <zthread/PoolExecutor.h>
#include <zthread/Runnable.h>
#include <zthread/Mutex.h>
#include "../common/common.h"
#include "AbsorptionCorrection.h"
#include <map>

using namespace ZThread;

class VolumeProcessing:public ZThread::Runnable {
  public:
    VolumeProcessing(float, vector<CvPoint3D> *, float*, Model*);
    virtual ~ VolumeProcessing();
    void run();
    
  private:
  	log4cplus::Logger myLogger;
    

    float phi;
    vector<CvPoint3D> *diffractionPoints;
    float *volume;
    Model *sample;
    
    CvPoint beamDimension; //in pixels
    int xScale;
    int yScale;
    int zScale;
    
    int xTranslate;
    int yTranslate;
    int zTranslate;
    
    CvPoint3D center;
    
    void fitsInTheIluminatedVolume(CvPoint3D64f point);
    int micrometersToPixelsX(float micros);
	int micrometersToPixelsY(float micros);
	void calculateIluminatedVolume();

};

#endif /*VOLUMEPROCESSING_H_ */
