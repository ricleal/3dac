#ifndef MOSFLM_H_
#define MOSFLM_H_
#include <iostream>
#include <zthread/Runnable.h>
#include "AbsorptionCorrection.h"
#include "../common/common.h"
#include <ccp4/cmtzlib.h>
#include <ccp4/csymlib.h>

using namespace ZThread;
using namespace CMtz;
using namespace CSym;
using namespace CCP4;

class Mosflm:public AbsorptionCorrection {
  public:

    Mosflm();
    Mosflm(Model * model);
    virtual ~ Mosflm();

    void calculateParallel();

    MTZCOL *getColH() const {
        return col_h;
    };
    MTZCOL *getColK() const {
        return col_k;
    };
    MTZCOL *getColL() const {
        return col_l;
    };
    MTZCOL *getColI() const {
        return col_i;
    };
    MTZCOL *getColSigi() const {
        return col_sigi;
    };
    MTZCOL *getColIpr() const {
        return col_ipr;
    };
    MTZCOL *getColSigipr() const {
        return col_sigipr;
    };
    MTZCOL *getColXdet() const {
        return col_xdet;
    };
    MTZCOL *getColYdet() const {
        return col_ydet;
    };
    MTZCOL *getColBatch() const {
        return col_batch;
    };
    MTZCOL *getColMisym() const {
        return col_misym;
    };

    MTZCOL **getColout() const {
        return colout;
    };

    MTZ *getMtz() const {
        return mtz;
    };


    union float_uint_uchar getUf() const {
        return uf;
    };

    CvPoint3D64f getOutputVector(ReflectionItem *);
    bool isPositiveFriedelPair(int misym);

  private:

    //Mtz file parameters
    MTZCOL * col_h;
    MTZCOL *col_k;
    MTZCOL *col_l;
    MTZCOL *col_i;
    MTZCOL *col_sigipr;
    MTZCOL *col_ipr;
    MTZCOL *col_sigi;
    MTZCOL *col_xdet;
    MTZCOL *col_ydet;
    MTZCOL *col_batch;
    MTZCOL *col_misym;


    MTZ *mtz;
    MTZCOL **colout;

    union float_uint_uchar uf;


};

#endif /*MOSFLM_H_ */
