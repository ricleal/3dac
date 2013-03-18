#ifndef MTZPROCESSING_H_
#define MTZPROCESSING_H_
#include "Mosflm.h"
#include "MtzProcessing.h"
#include "../common/common.h"
#include <ccp4/cmtzlib.h>
#include <ccp4/csymlib.h>

#include <zthread/PoolExecutor.h>
using namespace ZThread;

using namespace ZThread;
using namespace CMtz;
using namespace CSym;
using namespace CCP4;

class MtzProcessing:public ZThread::Runnable {
  public:
    MtzProcessing(int, Mutex*, Mosflm *);
    virtual ~ MtzProcessing();
    void run();
  private:
  //use:
  //lock.acquire();
  //set something
  //lock.release();
  	Mutex *lock;
  	log4cplus::Logger myLogger;
    int reflectionNr;
    Mosflm *mosflm;

};

#endif /*MTZPROCESSING_H_*/
