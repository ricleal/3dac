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

extern InputParameters *input;

// Images Parameters to Trypsin
//float PIXEL_SIZE = 0.102588; //mm
//float DISTANCE = 109.1; //mm
//float OSC_RANGE = 1.000; // degrees
//float START_ANGLE = 72;
//float BEAM_CENTER_X = 158.953; //mm
//float BEAM_CENTER_Y = 158.207; //mm

// Images Parameters to Insulin
//float PIXEL_SIZE = 0.1026;
//float DISTANCE = 119.942;
//float OSC_RANGE = 1.000;
//float START_ANGLE = 0.0;        //OSC_START=0.000;
//float BEAM_CENTER_X = 159.162;
//float BEAM_CENTER_Y = 158.030;

Mosflm::Mosflm(Model * model):AbsorptionCorrection(model)
{

}

Mosflm::~Mosflm()
{
}

/**
 * Get output unit vector in real values
 * reviewed 28/05/2007
 */
CvPoint3D64f Mosflm::getOutputVector(ReflectionItem * reflectionItem)
{

    // convert from mosflm:
    //
    //       |+Ys
    //       |
    //  ------
    //  +Xs
    //
    // to Model coordinate system
    //     / +Z
    //    /
    //   /___________ +X
    //   |
    //   |
    //   |
    //   | +Y

    // Convert mosflm detector coordinates (0 --> 3070 pixels) to real numbers in mm (e.g.  -150 to 150 mm)
    // Attention: the coordinate system remains the same here: Negative part is the right!
    // To change this x axis has to be inverted

    double xDet = reflectionItem->getXDet();
    double yDet = reflectionItem->getYDet();
    double distance = input->getDistance();

    LOG4CPLUS_DEBUG(myLogger, "Beam Center (" << input->getBeamCenterX()
                    << "," << input->getBeamCenterY() << ")mm = (" << input->getBeamCenterX() /
                    input->getPixelSize() << "," << input->getBeamCenterY() /
                    input->getPixelSize() << ")pixels in the mosflm system axis _|, pixel size=" << input->
                    getPixelSize());

    float x = -((xDet * input->getPixelSize()) - input->getBeamCenterX());
    float y = (yDet * input->getPixelSize()) - input->getBeamCenterY();
    float z = distance;

    LOG4CPLUS_DEBUG(myLogger, "Mosflm X,Y det coords: (" << xDet << "," << yDet << ")pixels");
    LOG4CPLUS_DEBUG(myLogger,
                    "Mosflm coord system with (0,0) at beam center: " << x << "," << y <<
                    ")mm = in pixels (" << x / input->getPixelSize() << "," << y /
                    input->getPixelSize() << "); Distance: " << distance << "mm");

    //because the in the mosflm the backspot appears in the bottom rather than in left side,
    // the image must me rotated by -90 degrees
    //  x' = cos(a)x - sin(a)y
    //  y' = sin(a)x + cos(a)y
    // sin(-90) = -1
    // cos(90) = 0
    // x' = y
    // y' = -x

    float xPrime = y;
    float yPrime = -x;
    x = xPrime;
    y = yPrime;

    // and then the image is flipped along Y => Y changes
    y = -y;

    LOG4CPLUS_DEBUG(myLogger, "Real Position in the dectector: (" << x << "," << y << ")mm");
    reflectionItem->setXDetReal(x);
    reflectionItem->setYDetReal(y);

    // set air path on the diffracted beam
    float airBeamDistance = calculateAirPath(input->getDistance(), x, y);
    reflectionItem->setAirDiffractedBeamMagnitude(airBeamDistance);
    reflectionItem->setAirIncidentBeamMagnitude(0);

    // Attention this is not true in this system!! Y points down
    y = -y;

    double vecLength = sqrt(square(x) + square(y) + square(z));
    x = x / vecLength;
    y = y / vecLength;
    z = z / vecLength;

    CvPoint3D64f res = cvPoint3D64f(x, y, z);

    LOG4CPLUS_DEBUG(myLogger, "Unit output Vector in the model/image system : " << x << "," << y << "," << z);

    return res;
}

/*
 * ISYM  is pair for I-
 */
bool Mosflm::isPositiveFriedelPair(int misym)
{

    int modulus = misym % 256;

    int isItPair = modulus % 2; // if it's pair returns 0

    if (isItPair == 0)
        return false;
    else
        return true;
}

 /******************************************************************************************************
 *
 * New version of calculating using threads
 *
 * Initializa MTZ struct, column labels, starts readin the file
 *
 */
void Mosflm::calculateParallel()
{
    /* read in MTZ file, leaving reflections on disk */
    LOG4CPLUS_INFO(myLogger, "Reading mtz file from here: " << input->getInputReflectionsFilePath());

    mtz = MtzGet(input->getInputReflectionsFilePath(), 0);
    /* open output file */
    mtz->fileout = MtzOpenForWrite(input->getOutputReflectionsFilePath());

    //hkl
    col_h = MtzColLookup(mtz, "H");
    col_k = MtzColLookup(mtz, "K");
    col_l = MtzColLookup(mtz, "L");
    col_batch = MtzColLookup(mtz, "BATCH");
    col_misym = MtzColLookup(mtz, "M_ISYM");

    col_i = MtzColLookup(mtz, "I");
    col_sigi = MtzColLookup(mtz, "SIGI");
    col_ipr = MtzColLookup(mtz, "IPR");
    col_sigipr = MtzColLookup(mtz, "SIGIPR");
    col_xdet = MtzColLookup(mtz, "XDET");
    col_ydet = MtzColLookup(mtz, "YDET");

    /* get value of MNF, for later use */
    if (strncmp(mtz->mnf.amnf, "NAN", 3) == 0)
    {
        uf = ccp4_nan();
    }
    else
    {
        uf.f = mtz->mnf.fmnf;
    }

    colout = (MTZCOL **) ccp4_utils_malloc((mtz->ncol_read) * sizeof(MTZCOL *));

    unitCellVolume = 0;
    /* set up colout array for lwrefl */
    for (int i = 0; i < mtz->nxtal; ++i)
    {
        if (unitCellVolume == 0 && mtz->xtal[i]->cell != NULL)
            unitCellVolume = mtz->xtal[i]->cell[0] * mtz->xtal[i]->cell[1] * mtz->xtal[i]->cell[2];
        for (int j = 0; j < mtz->xtal[i]->nset; ++j)
            for (int k = 0; k < mtz->xtal[i]->set[j]->ncol; ++k)
                if (mtz->xtal[i]->set[j]->col[k]->source)
                    colout[mtz->xtal[i]->set[j]->col[k]->source - 1] = mtz->xtal[i]->set[j]->col[k];
    }

    // main loop: for all reflexions
    //spawns a set of threads that are used to run tasks that are submitted in parallel.
    // n number of threads to service tasks with
    //PoolExecutor poolExecutor(4);
    Mutex lock;
    LOG4CPLUS_INFO(myLogger, "MtzProcessing: Launching threads...");
    for (int i = 0; i < mtz->nref_filein; ++i)
    {
        try
        {
            //Mosflm *m(this);
            //Thread t(new MtzProcessing(0, &lock, this));
            executor.execute(new MtzProcessing(i, &lock, this));
        }
        catch(Synchronization_Exception & e)
        {
            LOG4CPLUS_ERROR(myLogger, "MtzProcessing: Launching threads: " << e.what());
        }

    }
    //LOG4CPLUS_INFO(myLogger, "MtzProcessing: Waiting for the remaining threads to finish...");
    executor.wait();

    //    cout << "Debug: Launching th\n";
    //        Mutex lock;
    //    Thread t(new MtzProcessing(0, &lock, this));
    //    cout << "Debug: Launched th\n";
    //    t.wait();
    //    cout << "Debug: End wait th\n";

    /* dump new structure to HKLOUT */
    MtzPut(mtz, "HKLOUT");
    MtzFree(mtz);
    LOG4CPLUS_INFO(myLogger, "New mtz saved in: " << input->getOutputReflectionsFilePath());

    reflectionData->csvDump(input->getOutputCsvFilePath());
    LOG4CPLUS_INFO(myLogger, "Dump csv file saved in: " << input->getOutputCsvFilePath());

}
