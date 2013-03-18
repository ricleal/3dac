#include "MtzProcessing.h"
#include "Mosflm.h"
#include "MtzProcessing.h"
#include "Reflection.h"
#include "../common/common.h"
#include <ccp4/cmtzlib.h>
#include <ccp4/csymlib.h>

#include <zthread/PoolExecutor.h>

using namespace ZThread;
using namespace CMtz;
using namespace CSym;
using namespace CCP4;

extern InputParameters *input;

MtzProcessing::MtzProcessing(int refl, Mutex * lock_p, Mosflm * _mosflm) :
	myLogger(log4cplus::Logger::getInstance("MtzProcessing")) {
	reflectionNr = refl;
	lock = lock_p;
	mosflm = _mosflm;
}

MtzProcessing::~MtzProcessing() {
}

/*
 * For paralel processing
 */
void MtzProcessing::run() {
	//      for (int j=0; j < 100; j++)
	//      cout << "HERE!\n";
	//
	//    mosflm->volumeDump();

	mosflm->printStatus(mosflm->getMtz()->nref_filein, reflectionNr);

	ReflectionItem *reflectionItem;

	/* now we know how many columns, allocate memory for adata, logmss, colout */
	float *adata = (float *) ccp4_utils_malloc((mosflm->getMtz()->ncol_read) * sizeof(float));
	int *logmss = (int *) ccp4_utils_malloc((mosflm->getMtz()->ncol_read) * sizeof(int));

	float resol; //resolution of reflection (output)
	// read reflection reflectionNr
	ccp4_lrrefl(mosflm->getMtz(), &resol, adata, logmss, reflectionNr + 1);

	// if the columns exist
	if ( !logmss[mosflm->getColBatch()->source - 1] && !logmss[mosflm->getColXdet()->source - 1] && !logmss[mosflm->getColYdet()->source - 1] && !logmss[mosflm->getColH()->source - 1] && !logmss[mosflm->getColK()->source - 1] && !logmss[mosflm->getColL()->source - 1] && !logmss[mosflm->getColMisym()->source - 1]) {
		// reflection details
		reflectionItem = new ReflectionItem((int) adata[mosflm->getColH()->source - 1],
				(int) adata[mosflm->getColK()->source - 1],
				(int) adata[mosflm->getColL()->source - 1],
				(int) adata[mosflm->getColMisym()->source - 1],
				(int) adata[mosflm->getColBatch()->source - 1]);

		//debug
		if ((mosflm->getDEBUG_TYPE() == DRAW_BEAM
				&& !(mosflm->getDebugReflectionItem() == *reflectionItem))
				|| (mosflm->getDEBUG_TYPE() == DUMP_REFLECTION
						&& !(mosflm->getDebugReflectionItem()
								<= *reflectionItem))
				|| (mosflm->getDEBUG_TYPE() == DUMP_BATCH
						&& !(mosflm->getDebugReflectionItem().isBatch(reflectionItem->getBatch())))) {
			free(adata);
			free(logmss);
			free(reflectionItem);
			return;
		}

		// if there are IPR columns in MTZ file
		if (!logmss[mosflm->getColIpr()->source - 1] && !logmss[mosflm->getColSigipr()->source - 1]) {
			reflectionItem->setIprObs(adata[mosflm->getColIpr()->source - 1]);
			reflectionItem->setSigiprObs(adata[mosflm->getColSigipr()->source - 1]);
		}

		reflectionItem->setIObs(adata[mosflm->getColI()->source - 1]);
		reflectionItem->setSigiObs(adata[mosflm->getColSigi()->source - 1]);
		reflectionItem->setXDet(adata[mosflm->getColXdet()->source - 1]);
		reflectionItem->setYDet(adata[mosflm->getColYdet()->source - 1]);

		// crystalVolume / square(unitCellVolume * 10e-12)
		reflectionItem->setCrystalVolume(mosflm->getNormalisedVolumeConstants(reflectionItem->getBatch()));

		// the "phi"-angle the crystal has been rotated about the spindle axis prior to recording data on image number i by
		// phi(i) = STARTING_ANGLE + OSCILLATION_RANGE * (i - STARTING_FRAME)

		float startingAngle = input->getStartAngle();
		float oscillationRange = input->getOscRange();
		float phi = degreeToRad(startingAngle + oscillationRange
				* reflectionItem->getBatch());

		//Attention!!! Because the model doesn't rotate, only the beam the Phi must be -Phi
		phi = -phi;

		vector < CvPoint3D > intersectedCoords =
				mosflm->getDiffractionCoordinates(reflectionItem->getBatch());
		CvPoint3D64f unitDiffractedVector =
				mosflm->getOutputVector(reflectionItem);

		// list of beampaths for each intersection
		vector < BeamPath > beamPathList;
		// for each random point in the beam intersection
		for (unsigned int i = 0; i < intersectedCoords.size(); i++) {

			BeamPath b = mosflm->calculateBeamPath(intersectedCoords[i], phi,
					unitDiffractedVector, reflectionItem);
			beamPathList.push_back(b);
		}

		// Calculate Transmitance
		mosflm->calculateTransmittance(beamPathList, reflectionItem);

		reflectionItem->setICorr(mosflm->
		calculateRealIntensity(reflectionItem->getTotalBeamTransmittance(),
				reflectionItem->getIObs(), reflectionItem->getCrystalVolume()));

		reflectionItem->setSigiCorr(mosflm->calculateRealSigma(
				reflectionItem->getTotalBeamTransmittance(),
				reflectionItem->getSigiObs(),
				reflectionItem->getCrystalVolume()));

		adata[mosflm->getColI()->source - 1] = reflectionItem->getICorr(); // * (reflectionItem->getCrystalIncidentBeamMagnitude() * -0.014 + 1.64);
		adata[mosflm->getColSigi()->source - 1] = reflectionItem->getSigiCorr(); // * (reflectionItem->getCrystalIncidentBeamMagnitude() * -0.014 + 1.64);

		// if there are IPR columns in MTZ file correct also this columns
		if (reflectionItem->getIprObs() != 0 && reflectionItem->getSigiprObs()
				!= 0) {
			reflectionItem->setIprCorr(mosflm->
			calculateRealIntensity(reflectionItem->getTotalBeamTransmittance(),
					reflectionItem->getIprObs(),
					reflectionItem->getCrystalVolume()));

			reflectionItem->setSigiprCorr(mosflm->calculateRealSigma(
					reflectionItem->getTotalBeamTransmittance(),
					reflectionItem->getSigiprObs(),
					reflectionItem->getCrystalVolume()));
			adata[mosflm->getColIpr()->source - 1] = reflectionItem->getIprCorr();
			adata[mosflm->getColSigipr()->source - 1] = reflectionItem->getSigiprCorr();

		}

	} else // if columns don't exist!
	{
		LOG4CPLUS_WARN(myLogger, "Some Columns Don't exist!");
		adata[mosflm->getColI()->source - 1] = mosflm->getUf().f;
		adata[mosflm->getColSigi()->source - 1] = mosflm->getUf().f;
	}

	ccp4_lwrefl(mosflm->getMtz(), adata, mosflm->getColout(), mosflm->getMtz()->ncol_read, reflectionNr + 1);
	free(adata);
	free(logmss);

	// append reflection details
	lock->acquire();
	mosflm->getReflectionData()->appendItem(reflectionItem);
	lock->release();
}
