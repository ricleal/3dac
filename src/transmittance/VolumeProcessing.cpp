#include "VolumeProcessing.h"
#include <newran03/newran.h>
#include <zthread/PoolExecutor.h>
#include <opencv/cv.h>
#include <opencv/cxcore.h>

#include <math.h>



using namespace ZThread;


extern InputParameters *input;

/*
 * Constructor
 */
VolumeProcessing::VolumeProcessing(float i, vector < CvPoint3D > *v, float *f,
                                   Model * s):myLogger(log4cplus::Logger::getInstance("VolumeProcessing"))
{

    phi = i;
    diffractionPoints = v;
    volume = f;
    sample = s;

    // in pixels
    beamDimension = cvPoint(micrometersToPixelsX(input->getBeamSizeX()), micrometersToPixelsY(input->getBeamSizeY()));
    beamDimension.x > sample->getModelSizeX()? xScale = sample->getModelSizeX() : xScale = beamDimension.x;
    beamDimension.y > sample->getModelSizeY()? yScale = sample->getModelSizeY() : yScale = beamDimension.y;
    zScale = sample->getModelSizeZ();

    xTranslate = sample->getModelCenterX() - (xScale / 2);
    yTranslate = sample->getModelCenterY() - (yScale / 2);
    zTranslate = sample->getModelCenterZ() - (zScale / 2);      //1;
    
    center = cvPoint3D(sample->getModelCenterX(), sample->getModelCenterY(),
                                 sample->getModelCenterZ());

    MotherOfAll urng;                    // declare uniform random number generator

    Random::Set(urng);                   // set urng as generator to be used

    //Random::Set(0.46875);
}

VolumeProcessing::~VolumeProcessing()
{
}

/*
 * Main Cycle
 */
void VolumeProcessing::run()
{

    if (diffractionPoints->empty())
    {
    	// in the case that we have just one diffraction point, this is going to be in the center of the model.
    	if ( input->getNDiffractedBeams() == 1) {
    		 diffractionPoints->push_back(center);
    	}
    	else {
	        Uniform uniform;
	        while (diffractionPoints->size() < (unsigned) input->getNDiffractedBeams())
	        {
	            double x = (double) uniform.Next();
	            double y = (double) uniform.Next();
	            double z = (double) uniform.Next();
	            CvPoint3D64f point = cvPoint3D64f(x, y, z);
	            fitsInTheIluminatedVolume(point);
	
	        }
    	}
    }
    if (*volume == 0)
        calculateIluminatedVolume();

}


/*
 * Checks if the random point fits in the illuminated volume
 * If yes, add it to the list of points that will cause diffraction
 */ 
void VolumeProcessing::fitsInTheIluminatedVolume(CvPoint3D64f point)
{

    int xScaled = cvRound(xScale * point.x);
    int yScaled = cvRound(yScale * point.y);
    int zScaled = cvRound(zScale * point.z);

    //translate to the center of the model
    int xTranslated = xScaled + xTranslate;
    int yTranslated = yScaled + yTranslate;
    int zTranslated = zScaled + zTranslate;

    CvPoint3D p = cvPoint3D(xTranslated, yTranslated, zTranslated);

    Rotation rot(phi);
    

    CvPoint3D pRotated = rot.rotateX(p, center);

    // if rotated point is in the boundaries and inside the crystal
    if (pRotated.y > 0 && pRotated.y < sample->getModelSizeY() && pRotated.z > 0
        && pRotated.z < sample->getModelSizeZ() && pRotated.x > 0 && pRotated.x < sample->getModelSizeX()
        && sample->getModelCoordinate(pRotated.x, pRotated.y, pRotated.z) == CRYSTAL)
    {
        diffractionPoints->push_back(pRotated);
    }

}

/*
 * Calculate the iluminated volume
 * Iterates over the whole model, and adds up the point inside the beamsize crossing
 */
void VolumeProcessing::calculateIluminatedVolume()
{
    Rotation rot(phi);

	int numberOfPixelsInTheCrystal = 0;

	if (beamDimension.x != 0 || beamDimension.y != 0) {

		for (int i = sample->getModelCenterX() - (beamDimension.x / 2); i
				< sample->getModelCenterX() + (beamDimension.x / 2); i++) {

			for (int j = sample->getModelCenterY() - (beamDimension.y / 2); j
					< sample->getModelCenterZ() + (beamDimension.y / 2); j++) {

				for (int k = 0; k < sample->getModelSizeZ(); k++) {
					CvPoint3D pRotated =
							rot.rotateX(cvPoint3D(i, j, k), center);
					// if rotated point is in the boundaries and inside the crystal
					if (pRotated.y > 0 && pRotated.y < sample->getModelSizeY() && pRotated.z > 0 &&
							pRotated.z < sample->getModelSizeZ() && pRotated.x > 0 && pRotated.x
							< sample->getModelSizeX()
							&& sample->getModelCoordinate(pRotated.x,
									pRotated.y, pRotated.z) == CRYSTAL) {
						numberOfPixelsInTheCrystal++;
					}
				}
			}
		}

		// return volume in um3
		*volume = (numberOfPixelsInTheCrystal / (input->getPixelsPerMicromX()
				* input->getPixelsPerMicromY() * input->getPixelsPerMicromZ()) );

	}

	else {
		*volume = 1;
	}
    
}


int VolumeProcessing::micrometersToPixelsX(float micros)
{
    return (int) round(micros * input->getPixelsPerMicromX());
}

int VolumeProcessing::micrometersToPixelsY(float micros)
{
    return (int) round(micros * input->getPixelsPerMicromY());
}
