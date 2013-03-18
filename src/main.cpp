#include <iostream>             // Stream declarations
#include "ini_parser/iniparser.h"
#include "resources/InputParameters.h"
#include "model/Model.h"
#include "transmittance/Mesh.h"
#include "transmittance/Mosflm.h"
#include "common/common.h"

using namespace std;

InputParameters *input;

//log4cplus::SharedAppenderPtr mainAppender;
static log4cplus::Logger logger = log4cplus::Logger::getInstance("main");

int main(int argc, char **argv) {
	/*
	 * global definition for the log
	 */

	char *iniFile;
	char *logFile;

	if (argc < 2) {
		printf("\nUsage : %s <ini_file> [logfile]\n", argv[0]);
		printf("log4cplus.properties in the current folder will be used as default log configuration.\n");
		exit(1);
	} else if (argc == 3) {
		iniFile = argv[1];
		logFile = argv[2];
	} else { //argc == 2
		iniFile = argv[1];
		char *path = getcwd(NULL,1024);
		//logFile = (char *) "/home/leal/workspace/3dac/src/log4cplus.properties";
		logFile = strcat(path,"/log4cplus.properties");
	}

	cout << "iniFile: " << iniFile << endl;
	cout << "logFile: " << logFile << endl;
	cout << "Starting....." << endl;


	try {
		log4cplus::PropertyConfigurator::doConfigure(logFile);
	} catch (...) {
		fprintf(stderr, "ERROR: Unable to load logging configuration: %s", logFile);
		return -1;
	}

	LOG4CPLUS_INFO(logger, "Starting the Application");
	LOG4CPLUS_INFO(logger,"Memory Usage: " << memUsage());

	// read input parameters file
	input = new InputParameters(iniFile);
	input->parseConfigFile();

	char *steps = NULL;

	steps = input->getSteps();

	Model *model = NULL;
	Model *volumeModel = NULL;
	Model *crystalModel = NULL;
	Model *loopModel = NULL;

	Mesh *volumeMesh = NULL;
	Mesh *crystalMesh = NULL;
	Mesh *loopMesh = NULL;

	// build model and save
	if (strchr(steps, '1')) {
		LOG4CPLUS_INFO(logger, "Task 1 - Build Model from silhouettes");

		// build volume
		if (strcmp(strToUpper(input->getVolumeSet()->getProcess()), "NO") != 0) {
			LOG4CPLUS_INFO(logger, "Building Buffer...");
			volumeModel = new Model(input->getVolumeSet(), BUFFER);
			volumeModel->buildBackProjection();//build();
			//volumeModel->dumpModel();
			LOG4CPLUS_INFO(logger, "Saving Buffer to: " << input->getVolumeSet()->getModelFilePath());
			volumeModel->saveINR(input->getVolumeSet()->getModelFilePath());
		} else
			LOG4CPLUS_INFO(logger, "Buffer not being built...");

		//build crystal
		if (strcmp(strToUpper(input->getCrystalSet()->getProcess()), "NO") != 0) {
			LOG4CPLUS_INFO(logger, "Building Crystal...");
			crystalModel = new Model(input->getCrystalSet(), CRYSTAL);
			crystalModel->buildBackProjection();//build();
			LOG4CPLUS_INFO(logger, "Saving Crystal to: " << input->getCrystalSet()->getModelFilePath());
			crystalModel->saveINR(input->getCrystalSet()->getModelFilePath());
		} else
			LOG4CPLUS_INFO(logger, "Crystal not being built...");

		//build loop
		if (strcmp(strToUpper(input->getLoopSet()->getProcess()), "NO") != 0) {
			LOG4CPLUS_INFO(logger, "Building Loop...");
			loopModel = new Model(input->getLoopSet(), LOOP);
			loopModel->buildBackProjection();
			LOG4CPLUS_INFO(logger, "Saving Loop to: " << input->getLoopSet()->getModelFilePath());
			loopModel->saveINR(input->getLoopSet()->getModelFilePath());
		} else
			LOG4CPLUS_INFO(logger, "Loop not being built...");

		// merging models
		if (loopModel != NULL && crystalModel != NULL && volumeModel != NULL) {
			LOG4CPLUS_INFO(logger, "Merging models the 3 models...");
			volumeModel->mergeModels(loopModel, crystalModel);
			//save
			LOG4CPLUS_INFO(logger, "Saving merged models to: " << input->getOutputInrGlobalmodelFilePath());
			volumeModel->saveINR(input->getOutputInrGlobalmodelFilePath());
		} else if (crystalModel != NULL && volumeModel != NULL) {
			LOG4CPLUS_INFO(logger, "Merging models only Volume and Crystal models...");
			volumeModel->mergeModel(crystalModel, CRYSTAL);
			//save
			LOG4CPLUS_INFO(logger, "Saving 2 models merged models to: " << input->getOutputInrGlobalmodelFilePath());
			volumeModel->saveINR(input->getOutputInrGlobalmodelFilePath());
		}
		else {
			LOG4CPLUS_INFO(logger, "Merging no models (Crystal and Volume must be defined!)");
		}

		// If no model is selected for processing, it performs this test!
		if (strcmp(strToUpper(input->getVolumeSet()->getProcess()), "NO") == 0 &&
				strcmp(strToUpper(input->getCrystalSet()->getProcess()), "NO") == 0 &&
				strcmp(strToUpper(input->getLoopSet()->getProcess()), "NO") == 0) {
			LOG4CPLUS_INFO(logger, "Building empty model...");
			const char *tempPath = "/tmp/test.inr";
			loopModel = new Model();
			loopModel->buildTestAxis();
			LOG4CPLUS_INFO(logger, "Saving empty model to:" << tempPath );
			loopModel->saveINR((char *)tempPath);

		}


	}
	// load model
	if (strchr(steps, '2')) {
		LOG4CPLUS_INFO(logger, "Task 2 - Loading mesh Off files... ");

		// todo
		volumeMesh = new Mesh(input->getOffVolumeModelFilePath());
		crystalMesh = new Mesh(input->getOffCrystalModelFilePath());
		loopMesh = new Mesh(input->getOffLoopModelFilePath());

		//meshProcessing->testTriangleIntersection();

	}
	// calculate absorption using meshes
	if (strchr(steps, '3')) {
		LOG4CPLUS_INFO(logger, "Task 3 - Calculate Absorption");

		model = new Model(input->getInputInrGlobalmodelFilePath());

		//calculate the transmitance
		Mosflm *abs = new Mosflm(model);

		// Parallel processing
		abs->calculateParallel();

		delete abs;

	}
	if (model != NULL)
			delete model;

	if (volumeModel != NULL)
		delete volumeModel;

	if (loopModel != NULL)
		delete loopModel;

	if (crystalModel != NULL)
		delete crystalModel;

	delete input;

	LOG4CPLUS_INFO(logger,"Memory Usage: " << memUsage());
	LOG4CPLUS_INFO(logger, "Application Finished");

	return 0;
} ///:~
