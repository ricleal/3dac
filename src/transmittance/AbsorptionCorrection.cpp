#include "AbsorptionCorrection.h"
#include <newran03/newran.h>
#include "../common/common.h"
#include "Rotation.h"
#include "Reflection.h"
#include "VolumeProcessing.h"

#include <zthread/PoolExecutor.h>

using namespace ZThread;

#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

extern InputParameters *input;

AbsorptionCorrection::AbsorptionCorrection(Model * s):
    //
myLogger(log4cplus::Logger::getInstance("absorption")),
    //calculated externly. They depend on: wavelength and material
    absorptionCoefficientLiquid(input->getAbsorptionCoefficientLiquid()),
    //
    absorptionCoefficientLoop(input->getAbsorptionCoefficientLoop()),
    //
    absorptionCoefficientCrystal(input->getAbsorptionCoefficientCrystal()), 
    //
    executor(4) //4 processors
{

    sample = s;
    reflectionData = new Reflection();
    averageIlluminatedVolume = 0;
    unitCellVolume = 0;

    // DEBUG INFORMATION
    DEBUG_TYPE = NONE;          //NONE, DRAW_BEAM, DUMP_REFLECTION, DUMP_BATCH
    debugReflectionItem = ReflectionItem(7,	12,	25,	276,	1);     //ReflectionItem(8, 28, 20, 270, 140); 

    if (DEBUG_TYPE == DRAW_BEAM)
    {
    	// steps=3,4,5
        input->setSteps(strcat(input->getSteps(), strdup(",5")));
        myLogger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
        input->setOutputCsvFilePath(strcat(input->getOutputCsvFilePath(), strdup(".beam.csv")));
    }
    else if (DEBUG_TYPE == DUMP_REFLECTION) 
    {
        myLogger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
        input->setOutputCsvFilePath(strcat(input->getOutputCsvFilePath(), strdup(".refl.csv")));
    }
    else if (DEBUG_TYPE == DUMP_BATCH)
    {
        myLogger.setLogLevel(log4cplus::INFO_LOG_LEVEL);
        input->setOutputCsvFilePath(strcat(input->getOutputCsvFilePath(), strdup(".batch20.csv")));
    }

    /* Initialize the Volume Settings: diffracted points / iluminated */
    volumeInit();
    //volumeDump();
 
}

AbsorptionCorrection::~AbsorptionCorrection()
{
    free(reflectionData);
}

/*
 * Function to calculate crystal volume
 * iluminated volume
 * and diffraction points
 */
void AbsorptionCorrection::volumeInit()
{

    //PoolExecutor executor(4);
    // for all batches

    float startingAngle = input->getStartAngle();
    float oscillationRange = input->getOscRange();
    int numberOfBatches = input->getTotalBatches();

    LOG4CPLUS_INFO(myLogger, "Volume: Launching threads...");
    for (int i = 1; i <= numberOfBatches; ++i)
    {

        float phi = degreeToRad(startingAngle + oscillationRange * i);

        //Attention!!! Because the model doesn't rotate, only the beam the Phi must be -Phi
        phi = -phi;

        try
        {
            //Thread t(new VolumeProcessing(phi, &diffractionCoordinates[i], &illuminatedVolume[i], sample));
            executor.execute(new VolumeProcessing(phi, &diffractionCoordinates[i], &illuminatedVolume[i], sample));
        }
        catch(Synchronization_Exception & e)
        {
            LOG4CPLUS_ERROR(myLogger, "Volume: Launching threads: " << e.what());
        }

    }

    LOG4CPLUS_INFO(myLogger, "Volume: Waiting for the remaining threads to finish...");
    executor.wait();

}

/*
 * Calculate a normalized value for
 *  IlluminatedVolume / (Vunitcell)^2
 */
double AbsorptionCorrection::getNormalisedVolumeConstants(int batchNr)
{
    
	// just exeuted once
    if (averageIlluminatedVolume == 0)
    {
        float cont = 0;
        map < int, float >::iterator iter;
        for (iter = illuminatedVolume.begin(); iter != illuminatedVolume.end(); iter++)
        {
            //cout << "Batch: " << iter->first << " Volume: " << iter->second << endl;
            cont += iter->second;
        }
         // convert unitCellVolume from A^3 to um^3
        averageIlluminatedVolume = cont / illuminatedVolume.size() / square(unitCellVolume * 10e-12);
    }
    
    
    double volumeConstants = illuminatedVolume[batchNr] / square(unitCellVolume * 10e-12) / averageIlluminatedVolume;
    
    return volumeConstants;

}

/* 
 * Dump 
 * For debug purposes
 */
void AbsorptionCorrection::volumeDump()
{

    map < int, vector < CvPoint3D > >::iterator it;
    for (it = diffractionCoordinates.begin(); it != diffractionCoordinates.end(); it++)
    {
        cout << "Batch: " << it->first << endl;

        vector < CvPoint3D >::iterator it3;
        for (it3 = it->second.begin(); it3 != it->second.end(); it3++)
            cout << " Coords: " << *it3 << endl;
    }

    map < int, float >::iterator it2;
    for (it2 = illuminatedVolume.begin(); it2 != illuminatedVolume.end(); it2++)
    {
        cout << "Batch: " << it2->first << " Volume: " << it2->second << endl;
    }

}

// TODO
// for a diffractionPoint (inflection point in the intersection)
// for an angle phi
// for a diffracted beam with the direction of the output vector
// calculate the beam paths for both: directed and diffracted beam
// phi in degrees
BeamPath AbsorptionCorrection::
calculateBeamPath(const CvPoint3D diffractionPoint, float phi, CvPoint3D64f unitDiffractedVector,
                  const ReflectionItem * reflectionItem)
{

    LOG4CPLUS_TRACE(myLogger,
                    "CalculateBeamPath: Analizing point: " << diffractionPoint << " Angle: (" <<
                    radToDegree(phi) << " = " << fmod(radToDegree(phi), 360) << ") degrees");

    //NOTHING=0, CRAP, LOOP, BUFFER, CRYSTAL,
    DataType zoneOfInterest = CRYSTAL;
    CvPoint3D pointOfInterest = diffractionPoint;

        /**
	 * 3D model coordinates
	 * The image plane is X-Y
	 * The beam go into x-y following the positive sense of Z
	 * Crystal rotates along X
	 * 
	 * 
	 *    / +Z
	 *   /
	 *  /___________ +X
	 *  |
	 *  |
	 *  |
	 *  | +Y
	 */
    BeamPath beam = beamPath(diffractionPoint);

    /**
     * Incident beam
     */

    Rotation rot(phi);

    // point where the cristal diffracts
    CvPoint3D center = cvPoint3D(diffractionPoint.x, diffractionPoint.y, diffractionPoint.z);

    //x is fixed
    //y = R(y), y = y 
    //z = R(z)

    for (int z = diffractionPoint.z;; z--)
    {
        CvPoint3D point = cvPoint3D(diffractionPoint.x, diffractionPoint.y, z);
        CvPoint3D pRotated = rot.rotateX(point, center);

        LOG4CPLUS_TRACE(myLogger, "Incident beam: Point:" << point << " Rotated: " << pRotated);

        // rotated point out of bounds        
        if ((pRotated.y <= 0 || pRotated.y >= sample->getModelSizeY()) ||
            (pRotated.z <= 0 || pRotated.z >= sample->getModelSizeZ()))
            break;

        // the beam leaving from a zone to other 
        else if (sample->getModelCoordinate(pRotated.x, pRotated.y, pRotated.z) != zoneOfInterest)
        {
            beam = fillInputBeamPath(pointOfInterest, zoneOfInterest, beam);
            zoneOfInterest = sample->getModelCoordinate(pRotated.x, pRotated.y, pRotated.z);
            if (zoneOfInterest == NOTHING && DEBUG_TYPE == NONE)
            {

                break;
            }
        }
        // in the same zone
        else
        {
            pointOfInterest = pRotated;
        }
        // just for visual effect
        if (DEBUG_TYPE == DRAW_BEAM && *reflectionItem == debugReflectionItem)
            sample->setModelCoordinate(pRotated.x, pRotated.y, pRotated.z, INCIDENT);
    }

     /**
     * Diffracted beam
     */
    //zoneOfInterest = CRYSTAL;
    zoneOfInterest = sample->getModelCoordinate(diffractionPoint.x, diffractionPoint.y, diffractionPoint.z);
    int ro = 0;
    while (1)
    {
        CvPoint3D point = cvPoint3D(cvRound(diffractionPoint.x + ro * unitDiffractedVector.x),
                                    cvRound(diffractionPoint.y + ro * unitDiffractedVector.y),
                                    cvRound(diffractionPoint.z + ro * unitDiffractedVector.z));

        CvPoint3D pRotated = rot.rotateX(point, center);

        LOG4CPLUS_TRACE(myLogger, "Output beam: Point:" << point << " Rotated: " << pRotated);

        if ((pRotated.x < 0 || pRotated.x >= sample->getModelSizeX()) ||
            (pRotated.y < 0 || pRotated.y >= sample->getModelSizeY()) ||
            (pRotated.z < 0 || pRotated.z >= sample->getModelSizeZ()))
        {
            break;
        }
        else
        {

            // the beam leaving from a zone to other 
            if (sample->getModelCoordinate(pRotated.x, pRotated.y, pRotated.z) != zoneOfInterest)
            {
                beam = fillOutputBeamPath(pointOfInterest, zoneOfInterest, beam);
                zoneOfInterest = sample->getModelCoordinate(pRotated.x, pRotated.y, pRotated.z);
                if (zoneOfInterest == NOTHING && DEBUG_TYPE == NONE)
                {
                    break;
                }
            }
            else
            {
                pointOfInterest = pRotated;
            }

            if (DEBUG_TYPE == DRAW_BEAM && *reflectionItem == debugReflectionItem)
                sample->setModelCoordinate(pRotated.x, pRotated.y, pRotated.z, DIFFRACTED);
            ro++;
        }
    }
    return beam;
}

// from the beamPathList (model coordinates of the beam changing surfaces)
// calculate the paths and the total absorption 
void AbsorptionCorrection::calculateTransmittance(vector < BeamPath > beamPathList, ReflectionItem * reflectionItem)
{

    float totalTransmittance = 0;

    float totalCrystalTransmittance = 0;
    float totalLoopTransmittance = 0;
    float totalLiquidTransmittance = 0;

    float totalIncidentCrystalMagnitude = 0;
    float totalIncidentLoopMagnitude = 0;
    float totalIncidentLiquidMagnitude = 0;

    float totalDiffractedCrystalMagnitude = 0;
    float totalDiffractedLoopMagnitude = 0;
    float totalDiffractedLiquidMagnitude = 0;

    /*
     * Air 
     * Done here because will be +/- the same for every beam
     */
    // mm to um
    float diffractedAirMagnitude = reflectionItem->getAirBeamMagnitude() * 1000;
    float airTransmittance = input->getAbsorptionCoefficientAir() * diffractedAirMagnitude;
    float totalAirTransmittance = exp(-airTransmittance);

    for (unsigned int i = 0; i < beamPathList.size(); i++)
    {
        BeamPath b = beamPathList[i];
        //ATTENTION: 
        // IT'S ASSUMED THAT THERE'S ALWAYS LIQUID BETWEEN THE LOOP AND THE CRYSTAL
        // Crystal never touches the loop

        float incidentBeamLength;
        float diffractedBeamLength;

        /*
         * loop
         */

        float loopTransmittance;
        if (!nullVector(b.loopStart))
        {
            incidentBeamLength = magnitudeInMicrometers(b.loopStart, b.liquidStart);
        }
        else
            incidentBeamLength = 0;
        if (!nullVector(b.loopEnd))
        {
            diffractedBeamLength = magnitudeInMicrometers(b.loopEnd, b.liquidEnd);
        }
        else
            diffractedBeamLength = 0;

        loopTransmittance = input->getAbsorptionCoefficientLoop() * (incidentBeamLength + diffractedBeamLength);

        totalIncidentLoopMagnitude += incidentBeamLength;
        totalDiffractedLoopMagnitude += diffractedBeamLength;

        /*
         * liquid
         */
        float liquidTransmittance;
        if (!nullVector(b.liquidStart))
        {
            incidentBeamLength = magnitudeInMicrometers(b.liquidStart, b.crystalStart);
        }
        else
            incidentBeamLength = 0;
        if (!nullVector(b.liquidEnd))
        {
            diffractedBeamLength = magnitudeInMicrometers(b.crystalEnd, b.liquidEnd);
        }
        else
            diffractedBeamLength = 0;

        liquidTransmittance = input->getAbsorptionCoefficientLiquid() * (incidentBeamLength + diffractedBeamLength);

        totalIncidentLiquidMagnitude += incidentBeamLength;
        totalDiffractedLiquidMagnitude += diffractedBeamLength;

        /*
         * crystal
         */
        float crystalTransmittance;
        if (!nullVector(b.crystalStart))
        {
            incidentBeamLength = magnitudeInMicrometers(b.crystalStart, b.diffractionPoint);
        }
        else
            incidentBeamLength = 0;
        if (!nullVector(b.crystalEnd))
        {
            diffractedBeamLength = magnitudeInMicrometers(b.crystalEnd, b.diffractionPoint);
        }
        else
            diffractedBeamLength = 0;

        crystalTransmittance = input->getAbsorptionCoefficientCrystal() * (incidentBeamLength + diffractedBeamLength);

        totalIncidentCrystalMagnitude += incidentBeamLength;
        totalDiffractedCrystalMagnitude += diffractedBeamLength;

        /*
         * total
         */
        totalTransmittance += exp(-(crystalTransmittance + liquidTransmittance + loopTransmittance + airTransmittance));
        totalCrystalTransmittance += exp(-crystalTransmittance);
        totalLoopTransmittance += exp(-loopTransmittance);
        totalLiquidTransmittance += exp(-liquidTransmittance);

    }

    //     float diffractedAirMagnitude = reflectionItem->getAirBeamMagnitude() * 1000;
    //    float airTransmittance = input->getAbsorptionCoefficientAir() * diffractedAirMagnitude;
    //    float totalAirTransmittance = exp(-airTransmittance);
    //    

    reflectionItem->setAirBeamTransmittance(totalAirTransmittance);

    reflectionItem->setCrystalIncidentBeamMagnitude(totalIncidentCrystalMagnitude / beamPathList.size());
    reflectionItem->setCrystalDiffractedBeamMagnitude(totalDiffractedCrystalMagnitude / beamPathList.size());
    reflectionItem->setCrystalBeamTransmittance(totalCrystalTransmittance / beamPathList.size());

    reflectionItem->setLoopBeamTransmittance(totalLoopTransmittance / beamPathList.size());
    reflectionItem->setLoopDiffractedBeamMagnitude(totalDiffractedLoopMagnitude / beamPathList.size());
    reflectionItem->setLoopIncidentBeamMagnitude(totalIncidentLoopMagnitude / beamPathList.size());

    reflectionItem->setLiquidBeamTransmittance(totalLiquidTransmittance / beamPathList.size());
    reflectionItem->setLiquidDiffractedBeamMagnitude(totalDiffractedLiquidMagnitude / beamPathList.size());
    reflectionItem->setLiquidIncidentBeamMagnitude(totalIncidentLiquidMagnitude / beamPathList.size());

    reflectionItem->setTotalBeamTransmittance(totalTransmittance / beamPathList.size());
    reflectionItem->setTotalDiffractedBeamMagnitude((totalDiffractedCrystalMagnitude +
                                                     totalDiffractedLoopMagnitude +
                                                     totalDiffractedLiquidMagnitude) / beamPathList.size());
    reflectionItem->setTotalIncidentBeamMagnitude((totalIncidentCrystalMagnitude + totalIncidentLoopMagnitude +
                                                   totalIncidentLiquidMagnitude) / beamPathList.size());

}

BeamPath AbsorptionCorrection::fillInputBeamPath(CvPoint3D p, DataType zoneOfInterest, BeamPath & beamPath)
{

    //LOOP, BUFFER, CRYSTAL,
    if (zoneOfInterest == CRYSTAL)
        beamPath.crystalStart = p;
    else if (zoneOfInterest == BUFFER)
        beamPath.liquidStart = p;
    else if (zoneOfInterest == LOOP)
        beamPath.loopStart = p;

    return beamPath;
}

BeamPath AbsorptionCorrection::fillOutputBeamPath(CvPoint3D p, DataType zoneOfInterest, BeamPath & beamPath)
{

    //LOOP, BUFFER, CRYSTAL,
    if (zoneOfInterest == CRYSTAL)
    {
        beamPath.crystalEnd = p;
    }
    else if (zoneOfInterest == BUFFER)
    {
        beamPath.liquidEnd = p;
    }
    else if (zoneOfInterest == LOOP)
    {
        beamPath.loopEnd = p;
    }

    return beamPath;
}

float AbsorptionCorrection::calculateRealIntensity(float transmittance, float iobs, float crystalVolume = 1)
{
    /*

       Imeas = exp(-u*l) * I0 ; T = exp(-u*l) 
       Imeas = T * I0       <=> I0 = Imeas / T

       I = P * L * T * CrystVolume * I0

     */

    //return (iobs / (transmittance * (crystalVolume / averageIlluminatedVolume )));

	if (iobs < 0 && input->getNegativeIntensities() == 0) {
		return 0;
	} 
	else if (iobs < 0 && input->getNegativeIntensities() == 1) {
		return iobs;
	}  
	else if (iobs < 0 && input->getNegativeIntensities() == 2) {
		float moreNegativeI = iobs / (transmittance * crystalVolume);
		
		return iobs + fabs(moreNegativeI - iobs);
	}
	// positive Is
	else { 
		return (iobs / (transmittance * crystalVolume));
	}

}
float AbsorptionCorrection::calculateRealSigma(float transmittance, float sigmaobs, float crystalVolume = 1)
{
    /*

       Imeas = exp(-u*l) * I0 ; T = exp(-u*l) 
       Imeas = T * I0       <=> I0 = Imeas / T

       I = P * L * T * CrystVolume * I0

     */

    //return (iobs / (transmittance * (crystalVolume / averageIlluminatedVolume )));


	return (sigmaobs / (transmittance * crystalVolume));


}


/*
 *
 */
float AbsorptionCorrection::magnitudeInMicrometers(CvPoint3D p1, CvPoint3D p2)
{

    float x = (p1.x - p2.x) / (input->getPixelsPerMicromX());
    float y = (p1.y - p2.y) / (input->getPixelsPerMicromY());
    float z = (p1.z - p2.z) / (input->getPixelsPerMicromZ());

    return sqrt(square(x) + square(y) + square(z));
}

/*
 * Calculate air path from distances in milimeters
 *  -150 < detReal < 150
 * 
 */
float AbsorptionCorrection::calculateAirPath(float detectorDistance, float xDetReal, float yDetReal)
{
    // Let's work in milimiters

    float radius = sqrt(square(xDetReal) + square(yDetReal));

    return sqrt(square(radius) + square(yDetReal));

}

/*
 * Function which shows on the console the status bar
 */
void AbsorptionCorrection::printStatus(int total, int current)
{

    static int cont;
    static string s;

    int div = (total - 1) / 50;

    if (current % div == 0)
    {
        if (cont == 0)
            s = "";
        else
            s.append("#");

        printf("\r[%-50s] %3d%%", s.c_str(), cont * 2);
        fflush(stdout);         // Force printf to print to screen even though no \n

        cont++;

    }

    if (current == total - 1)
        printf("\n");
}
