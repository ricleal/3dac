#include "Model.h"
#include "../resources/InputParameters.h"
#include "../resources/InputImageParameters.h"
#include "../common/common.h"
#include "../transmittance/Rotation.h"
#include "MultipleViewGeom.h"

#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <fstream>
#include <zthread/PoolExecutor.h>

using namespace std;
using namespace ZThread;

extern InputParameters *input;

/*
 * Defaut Contructor:
 *
 * It fills the volume with nothing inside.
 *
 */
Model::Model() :
	myLogger(log4cplus::Logger::getInstance("model")),convMat(4, 4) {
	model = NULL;
	dataType = NOTHING;
	initialiseModel();
}

/*
 * Constructor to build the model from visual images
 */
Model::Model(InputImageParameters * iip, DataType fillWith) :
	myLogger(log4cplus::Logger::getInstance("model")),convMat(4, 4) {
	model = NULL;
	inputImageParameters = iip;
	dataType = fillWith;
	initialiseModel();
}

/**
 *
 * Constructor to load the model from a file
 */
Model::Model(char *path) :
	myLogger(log4cplus::Logger::getInstance("model")) {
	model = NULL;
	inputImageParameters = NULL;
	loadINR(path);
}

/*
 * When the model is loaded from a file
 *
 * initializes the volume with data parameters: size of the model
 * Coordinates always in pixels
 *
 */
void Model::initialiseModelFromFile() {

	modelCenterX = modelSizeX / 2 - 1;
	modelCenterY = modelSizeY / 2 - 1;
	modelCenterZ = modelSizeZ / 2 - 1;

	blocksize = modelSizeX * modelSizeY * modelSizeZ;

	// initialise model
	model = new DataType **[modelSizeX];
	for (int i = 0; i < modelSizeX; i++) {
		model[i] = new DataType *[modelSizeY];
		for (int j = 0; j < modelSizeY; j++) {
			model[i][j] = new DataType[modelSizeZ];
			//for (int k = 0; k < modelSizeZ; k++) {
			//	model[i][j][k] = dataType;
			//}
		}
	}

	LOG4CPLUS_INFO(myLogger, "Model initialised: (" << modelSizeX << "x" << modelSizeY << "x" <<
			modelSizeZ << ") with centre at (" << modelCenterX << "," <<
			modelCenterY << "," << modelCenterZ << ") pixels");

	LOG4CPLUS_INFO(myLogger, "Model in world coordinates -> voxelSizeX: " << voxelSizeX << " microns/voxel; WorldDimX: " << input->getWorldDimX() << " microns");

	//pixels per unit of measurement in Y e.g. 1 pixel/microm
	// Relation between model coordinates and real world coordinates
	float convContents[] = { (float) 1 / voxelSizeX, 0, 0, modelCenterX, 0,
			(float) -1 / voxelSizeY, 0, modelCenterY, 0, 0, (float) -1
					/ voxelSizeZ, modelCenterZ, 0, 0, 0, 1 };
	convMat << convContents;

	//printCvMat(convMat);
	//float convInvContents[12];
	//convMatInv = cvMat(4, 3, CV_32F, convInvContents);
	//printCvMat(&convMatInv);
	//cvInvert(&convMat,&convMatInv);

}

/**
 * NEW !!!
 */
void Model::initialiseModel() {

	modelSizeX = input->getModelSizeX();
	modelSizeY = input->getModelSizeY();
	modelSizeZ = input->getModelSizeZ();

	modelCenterX = modelSizeX / 2 - 1;
	modelCenterY = modelSizeY / 2 - 1;
	modelCenterZ = modelSizeZ / 2 - 1;

	blocksize = modelSizeX * modelSizeY * modelSizeZ;

	// initialise model
	model = new DataType **[modelSizeX];
	for (int i = 0; i < modelSizeX; i++) {
		model[i] = new DataType *[modelSizeY];
		for (int j = 0; j < modelSizeY; j++) {
			model[i][j] = new DataType[modelSizeZ];
			for (int k = 0; k < modelSizeZ; k++) {
				model[i][j][k] = dataType;
			}
		}
	}

	LOG4CPLUS_INFO(myLogger, "Model initialised: (" << modelSizeX << "x" << modelSizeY << "x" <<
			modelSizeZ << ") with centre at (" << modelCenterX << "," <<
			modelCenterY << "," << modelCenterZ << ") pixels");

	// each voxel mesures in microns: microns per pixel
	voxelSizeX = input->getWorldDimX() / modelSizeX;
	voxelSizeY = input->getWorldDimY() / modelSizeY;
	voxelSizeZ = input->getWorldDimZ() / modelSizeZ;

	LOG4CPLUS_INFO(myLogger, "Model in world coordinates -> voxelSizeX: " << voxelSizeX << " microns/voxel; WorldDimX: " << input->getWorldDimX() << " microns");

	//pixels per unit of measurement in Y e.g. 1 pixel/microm
	// Relation between model coordinates and real world coordinates
	float convContents[] = { (float) 1 / voxelSizeX, 0, 0, modelCenterX, 0,
			(float) -1 / voxelSizeY, 0, modelCenterY, 0, 0, (float) -1
					/ voxelSizeZ, modelCenterZ, 0, 0, 0, 1 };
	convMat << convContents;


	//printCvMat(convMat);
	//float convInvContents[12];
	//convMatInv = cvMat(4, 3, CV_32F, convInvContents);
	//printCvMat(&convMatInv);
	//cvInvert(&convMat,&convMatInv);

}

Model::~Model() {
	LOG4CPLUS_DEBUG(myLogger,
			"Removing the model from memory: " << modelSizeX << "x" << modelSizeY <<
			"x" << modelSizeZ);

	if (model != NULL) {

		for (int i = 0; i < modelSizeX; i++) {
			for (int j = 0; j < modelSizeY; j++) {
				delete[] model[i][j];
			}
			delete[] model[i];
		}
		delete[] model;
	}
}

/*
 * Builds volume for test purposes
 */
void Model::buildTestVolume(int x, int y, int z) {
	if (model == NULL) {
		initialiseModel();
	}

	for (int i = 0; i < modelSizeX; i++) {
		for (int j = 0; j < modelSizeY; j++) {
			for (int k = 0; k < modelSizeZ; k++) {

				//cube
				//bool b = (i >= 50 && i <= modelSizeX - 50) && (j >= 50 && j
				//		<= modelSizeY - 50)
				//		&& (k >= 80 && k <= modelSizeZ - 80);

				//sphere
				bool b = ((float) square(i - (x / 2 + 1)) + (float) square(j
						- (y / 2 + 1)) + (float) square(k - (z / 2 + 1)))
						<= (float) square((x + y + z) / 3 / 2);
				if (b) {
					model[i][j][k] = CRYSTAL;
				}
			}
		}
	}

}

void Model::buildTestAxis() {
	if (model == NULL) {
		initialiseModel();
	}

	for (int i = 0; i < modelSizeX; i++) {
		model[i][0][0] = LOOP;
	}
	for (int j = 0; j < modelSizeY; j++) {
		model[0][j][0] = BUFFER;
	}

	for (int k = 0; k < modelSizeZ; k++) {

		model[0][0][k] = CRYSTAL;

	}


}


/*
 * Get voxel value indexed by real coordinates in micros.
 *
 * Real referential is right hand:
 *
 *    ^
 *    | Y
 *    |
 *     ----------> X
 *   /
 *  / Z
 *
 * Model referential is also right hand
 *
 *      / Z
 *     /
 *      -----------------> X
 *    |
 *    | Y
 *    v
 *
 * (u,v,w) coordinates in the voxel map (0..size)
 *
 */
DataType Model::getVoxelValueFromRealCoordinates(float x, float y, float z) {

	float worldContents[] = { x, y, z, 1 };
	Matrix worldMat(4, 1);
	worldMat << worldContents;


	Matrix modelMat(4,1);
	modelMat = 0;

	// [u,v,w] ?
	// [u,v,w,1] = convMat * [x,y,z,1]

	//printCvMat(convMat);
	//printCvMat(&worldMat);
	//printCvMat(&modelMat);

//	cvMatMul (convMat, &worldMat,&modelMat);
	modelMat = convMat * worldMat;

	//cvMatMul(A, B, D) cvGEMM(A, B, 1, NULL, 0, D, 0)
	//cvGEMM(&convMat, &worldMat, 1, NULL, 0, &modelMat,0);

	//printCvMat(&modelMat);

	int u = cvRound(modelMat.element(0, 0));
	int v = cvRound(modelMat.element(1, 0));
	int w = cvRound(modelMat.element(2, 0));

	//LOG4CPLUS_DEBUG(myLogger,"(x,y,z)=(" << x <<","<<y<<","<<z<<") -> (u,v,w)=("<< u <<","<<v<<","<<w<<")");

	return model[u][v][w];

}

CvPoint3D Model::getVoxelCoordinatesFromRealCoordinates(float x, float y,
		float z) {

	float worldContents[] = { x, y, z, 1 };
	Matrix worldMat(4, 1);
	worldMat << worldContents;


	Matrix modelMat(4, 1);
	modelMat = 0;

	// [u,v,w] ?
	// [u,v,w,1] = convMat * [x,y,z,1]

	//printCvMat(convMat);
	//printCvMat(&worldMat);
	//printCvMat(&modelMat);

//	cvMatMul (convMat, &worldMat,&modelMat);
	modelMat = convMat * worldMat;
	//cvMatMul(A, B, D) cvGEMM(A, B, 1, NULL, 0, D, 0)
	//cvGEMM(&convMat, &worldMat, 1, NULL, 0, &modelMat,0);

	//printCvMat(&modelMat);

	int u = cvRound(modelMat.element(0, 0));
	int v = cvRound(modelMat.element(1, 0));
	int w = cvRound(modelMat.element(2, 0));

	//LOG4CPLUS_DEBUG(myLogger,"(x,y,z)=(" << x <<","<<y<<","<<z<<") -> (u,v,w)=("<< u <<","<<v<<","<<w<<")");

	CvPoint3D p = cvPoint3D(u, v, w);
	return p;
}

CvPoint3D32f Model::getRealCoordinatesFromVoxelMap(int u, int v, int w) {


	//Matrix worldMat(4, 1);
	//worldMat = 0;

	float modelContents[] = { u, v, w, 1 };
	ColumnVector modelMat(4);
	modelMat << modelContents;

	// [x,y,z,1] ?
	// [x,y,z,1] = convMat^{-1} * [u,v,w,1]

	//cvSolve(&A, &b, &x);    // solve (Ax=b) for x
//	cvSolve(convMat, &modelMat, &worldMat);

	ColumnVector worldMat = convMat.i() * modelMat;

	//printCvMat(&worldMat);

	float x = worldMat.element(0);
	float y = worldMat.element(1);
	float z = worldMat.element(2);

	//LOG4CPLUS_DEBUG(myLogger, "(u,v,w)=("<< u <<","<<v<<","<<w<<") -> (x,y,z)=(" << x <<","<<y<<","<<z<<")");

	CvPoint3D32f p = cvPoint3D32f(x, y, z);
	return p;
}

/*
 * Mix in the current model (the volume) the loop and the crystal
 *
 */
void Model::mergeModels(const Model * loopModel, const Model * crystalModel) {

	for (int i = 0; i < modelSizeX; i++) {
		for (int j = 0; j < modelSizeY; j++) {
			for (int k = 0; k < modelSizeZ; k++) {
				if (loopModel->getModel() != NULL
						&& loopModel->getModelCoordinate(i, j, k) == LOOP)
					model[i][j][k] = LOOP;
				if (crystalModel->getModel() != NULL
						&& crystalModel->getModelCoordinate(i, j, k) == CRYSTAL)
					model[i][j][k] = CRYSTAL;

			}
		}
	}

}

/*
 * Mix in the current model the model passed by parameters
 *
 */
void Model::mergeModel(const Model * modelIn, DataType type) {

	for (int i = 0; i < modelSizeX; i++) {
		for (int j = 0; j < modelSizeY; j++) {
			for (int k = 0; k < modelSizeZ; k++) {
				if (modelIn->getModelCoordinate(i, j, k) == type)
					model[i][j][k] = type;

			}
		}
	}

}

/**
 *
 The inrimage format has been developped at INRIA.
 It is a quite simple format containing a header of 256 characters followed by the image raw data.
 The header contains the following informations:
 - the image dimensions
 - number of values per pixel (or voxel)
 - the type of coding (integer, float)
 - the size of the coding in bits
 - the type of machine that coded the information: sun, dec.

 #INRIMAGE-4#{
 XDIM=128   // x dimension
 YDIM=128   // y dimension
 ZDIM=128   // z dimension
 VDIM=1      // number of scalar per voxel (1 = scalar image, 3 = 3D image of vectors)
 VX=0.66   // voxel size in x
 VY=0.66   // voxel size in y
 VZ=1       // voxel size in z
 TYPE=unsigned fixed   // float, signed fixed, or unsigned fixed
 PIXSIZE=16 bits   // 8, 16, 32, or 64
 SCALE=2**0       // not used in my program
 CPU=decm            // decm, alpha, pc, sun, sgi
 // little endianness : decm, alpha, pc; big endianness :sun, sgi

 // fill with carriage return or with any other information

 ##} // until the total size of the header is 256 characters (including final newline character)
 // raw data, size=XDIM*YDIM*ZDIM*VDIM*PIXSIZE/8
 */
void Model::saveINR(char *path) {
	ofstream outputFile(path, ios::out | ios::binary);
	if (outputFile.fail()) {
		LOG4CPLUS_FATAL(myLogger, "Save: File can't be saved: " << path);
		exit(-1);
	}
	//LOG4CPLUS_INFO(myLogger, "Saving model to: " << path);

	outputFile << "#INRIMAGE-4#{" << endl;
	outputFile << "XDIM=" << modelSizeX << endl;
	outputFile << "YDIM=" << modelSizeY << endl;
	outputFile << "ZDIM=" << modelSizeZ << endl;
	outputFile << "VDIM=1" << endl;
	//todo input this as params
	outputFile << "VX=" << voxelSizeX << endl;
	outputFile << "VY=" << voxelSizeY << endl;
	outputFile << "VZ=" << voxelSizeZ << endl;
	outputFile << "TYPE=unsigned fixed" << endl;
	outputFile << "PIXSIZE=" << sizeof(char) * 8 << " bits" << endl;
	outputFile << "SCALE=2**0" << endl;
	outputFile << "CPU=pc" << endl;

	long pos = outputFile.tellp();

	while (pos <= 250) {
		outputFile << endl;
		pos = outputFile.tellp();
	}

	outputFile << endl << "##}" << endl;

	for (int i = 0; i < modelSizeX; i++) {
		for (int j = 0; j < modelSizeY; j++) {
			for (int k = 0; k < modelSizeZ; k++) {
				outputFile << (char) model[i][j][k];
				//printf("model: %08x \n", model[i][j][k]);
			}
		}
	}
	outputFile.close();

}

/**
 * Load INR image
 */
void Model::loadINR(char *path) {

	ifstream fileIn(path);
	if ((fileIn.rdstate() & ifstream::failbit) != 0) {
		LOG4CPLUS_FATAL(myLogger, "Load INR: File doesn't exist: " << path);
		return;
	}

	const int BUFFER_SIZE = 1024; // Buffer size;
	char line[BUFFER_SIZE];
	//memset(line, 0, BUFFER_SIZE);

	fileIn.getline(line, BUFFER_SIZE); //eats the: #INRIMAGE-4#{
	//memset(line, 0, BUFFER_SIZE);
	// If still in the header
	while (fileIn.getline(line, BUFFER_SIZE) && strstr(line, "##}") == NULL) {
		char *key;
		char *val;
		//cout << "Line: " << line << endl;
		//sscanf(line, "%[^=] = %[^=]", key, val);
		key = strtok(line, "=");
		val = strtok(NULL, "=");
		//cout << "Key: " << key << endl;
		//		//cout << "Val: " << val << endl;

		if (key != NULL && val != NULL && strcmp(key, "XDIM") == 0) {
			modelSizeX = atoi(val);
		} else if (key != NULL && val != NULL && strcmp(key, "YDIM") == 0) {
			modelSizeY = atoi(val);
		} else if (key != NULL && val != NULL && strcmp(key, "ZDIM") == 0) {
			modelSizeZ = atoi(val);
		} else if (key != NULL && val != NULL && strcmp(key, "VX") == 0) {
			voxelSizeX = atoi(val);
		} else if (key != NULL && val != NULL && strcmp(key, "VY") == 0) {
			voxelSizeY = atoi(val);
		} else if (key != NULL && val != NULL && strcmp(key, "VZ") == 0) {
			voxelSizeZ = atoi(val);
		}
		memset(line, 0, BUFFER_SIZE);

	}

	initialiseModelFromFile();

	for (int i = 0; i < modelSizeX; i++) {
		for (int j = 0; j < modelSizeY; j++) {
			for (int k = 0; k < modelSizeZ; k++) {
				char c = fileIn.get();
				model[i][j][k] = (DataType) c;
				//printf("%08x", model[i][j][k]);
				//printf("file: %08x \t model: %08x \n", c, model[i][j][k]);
			}
		}
	}

	fileIn.close();

}

/**
 *
 * Dump the model to the console
 * Just for debug
 *
 */
void Model::dumpModel() {

	for (int i = 0; i < modelSizeX; i++) {
		printf("\n\n");
		for (int j = 0; j < modelSizeY; j++) {
			printf("\n");
			for (int k = 0; k < modelSizeZ; k++) {
				printf("%02x", model[i][j][k]);
			}
		}
	}
}

/**
 *
 * Builds a map indexed by the image angle of IPLimages
 *
 *  Input: map<float, IplImage> &imageMap
 *
 */
void Model::buildImageMap(map<float, IplImage*> &imageMap) {

	// reads the list and shows the images
	vector<string> imagesList = inputImageParameters->getImagesPathToProccess();
	vector<int> anglesList = inputImageParameters->getAnglesToProcess();

	// Read images from files and load them in memory.
	// Use hash tables indexed by the angle:

	for (unsigned int i = 0; i < imagesList.size(); i = i + 1) {
		string path = imagesList[i];
		IplImage *img = cvLoadImage(path.c_str());
		if (!img) {
			LOG4CPLUS_FATAL(myLogger, "Could not load image file: " << path);
			return;
		}
		// gray image
		IplImage *grayImg = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
		cvCvtColor(img, grayImg, CV_RGB2GRAY);
		//Because the images in the beamline are inverted in x direction
		cvFlip(grayImg, grayImg, 1);

		//        cvNamedWindow("test", 1);
		//        cvShowImage("test", img);
		//        cvWaitKey(0);

		int phiDegrees = anglesList[i];
		float phi = (PI / 180.0) * phiDegrees;
		imageMap.insert(make_pair(phi, grayImg));

		cvReleaseImage(&img);
	}
}

void Model::deleteImageMap(map<float, IplImage*> &imageMap) {

	map<float, IplImage*>::iterator it;

	for (it = imageMap.begin(); it != imageMap.end(); it++) {
		IplImage *img = it->second;
		cvReleaseImage(&img);
	}

}
/**
 * Build the 3D model with a back projection:
 * iterates all the model voxels and back project them to every image
 *
 */
void Model::buildBackProjection() {

	map<float, IplImage*> imageMap;
	buildImageMap(imageMap);

	MultipleViewGeom *multipleViewGeom = NULL;

	// iterate the model
	// for every voxel in the voxelmap backproject it against the image
	map<float, IplImage*>::iterator it;

	LOG4CPLUS_DEBUG(myLogger,"Memory Usage: " << memUsage());

	for (int x = 0; x < modelSizeX; x++) {
		progressBar(x, modelSizeX - 1);

		LOG4CPLUS_DEBUG(myLogger,"Memory Usage: " << memUsage());
		for (int y = 0; y < modelSizeY; y++) {

			for (int z = 0; z < modelSizeZ; z++) {

				CvPoint3D32f realCoordinates = getRealCoordinatesFromVoxelMap(
						x, y, z);

				for (it = imageMap.begin(); it != imageMap.end(); it++) {

					IplImage *img = it->second;
					float phi = it->first;

					if (multipleViewGeom == NULL) {
						// optical center assumed to be in the center of the image
						int u0 = cvRound(img->width / 2 - 1);
						int v0 = cvRound(img->height / 2 - 1);
						float alfaU = input->getPixelsPerMicromX();
						float alfaV = input->getPixelsPerMicromY();
						multipleViewGeom = new MultipleViewGeom(u0, v0, alfaU,
								-alfaV);
					}

					CvPoint imageCoordinates =
							multipleViewGeom->getProjectionOf(phi,
									realCoordinates);

					//					LOG4CPLUS_DEBUG(myLogger,"Angle: " << radToDegree(phi) << " imageCoordinates=(" << imageCoordinates.x
					//							<< "," << imageCoordinates.y << ") of (" << img.width <<"," << img.height << ")\trealCoordinates=("
					//							<< realCoordinates.x <<","<<realCoordinates.y << "," << realCoordinates.z << ")\tmodel coordinates=("
					//							<< x << "," << y << "," << z <<")");

					// Image pixel access
					// unsigned char value = ((img->imageData + img->widthStep * y))[x];

					if (imageCoordinates.x < 0 || imageCoordinates.x
							>= img->width || imageCoordinates.y < 0
							|| imageCoordinates.y >= img->height) {

						setModelCoordinate(x, y, z, NOTHING);
						//cout << "Removing VOXEL coordinates=(" << x << "," << y << "," << z <<")" << endl;
						break;

					} else {
						unsigned char
								pixelValue =
										((img->imageData + img->widthStep
												* imageCoordinates.y))[imageCoordinates.x];
						// if pixel value is inside outside the silhouette, e.g. black, it exists!
						// so, break the cycle
						if (pixelValue < 255) { //black
							// cout << "Angle: " << radToDegree(phi) << " White Pixel=(" << imageCoordinates.x << "," << imageCoordinates.y << ")" << endl;
							setModelCoordinate(x, y, z, NOTHING);
							//cout << "Removing VOXEL coordinates=(" << x << "," << y << "," << z <<")" << endl;
							// jumps to next voxel and don't iterate more images;
							break;

						}
						// else jumps to the next voxel

					}
				}
			}
		}
	}
	LOG4CPLUS_DEBUG(myLogger,"Memory Usage: " << memUsage());
	delete multipleViewGeom;
	deleteImageMap(imageMap);
}

/**
 *
 * Progess bar for building the model
 */
void Model::progressBar(int current, int total) {
	static char buffer[256] = { 0 };
	static char percent[7] = "0.0%%";
	static int barLength = 40;

	static float normalisedTotal = barLength / ((float) total + 1);

	int barPos = ((int) floor(normalisedTotal * current)) % barLength;

	buffer[0] = '[';

	buffer[barPos + 1] = '=';

	int j = barPos % 4;
	if (j == 0)
		buffer[barPos + 2] = '\\';
	else if (j == 1)
		buffer[barPos + 2] = '|';
	else if (j == 2)
		buffer[barPos + 2] = '/';
	else
		buffer[barPos + 2] = '-';

	for (int k = barPos + 3; k < barLength + 1; k++)
		buffer[k] = ' ';

	buffer[barLength + 1] = ']';

	sprintf(percent, "%3.2f%%", (float) (barPos / (float) barLength) * 100.0);

	printf("%s%s\r", buffer, percent);
	fflush(stdout);

	if (current == total) {
		barPos = barLength;

		sprintf(percent, "%3.2f%%", (float) (barPos / (float) barLength)
				* 100.0);
		printf("%s%s\r", buffer, percent);
		fflush(stdout);
		cout << endl;
	}

}
