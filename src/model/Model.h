#ifndef MODEL_H_
#define MODEL_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <zthread/Runnable.h>
#include <zthread/Mutex.h>

#include "../resources/InputImageParameters.h"
#include "../common/common.h"
#include "../transmittance/Rotation.h"
#include <newmat11/newmatap.h>

using namespace ZThread;

// type of data inside the model
typedef enum DataType {
	NOTHING = 0,
	CRAP,
	LOOP = 200,
	BUFFER = 150,
	CRYSTAL = 100,
	LOOP_INTERSECTION,
	BUFFER_INTERSECTION,
	CRYSTAL_INTERSECTION,
	//temp
	POINT,
	INCIDENT,
	DIFFRACTED
} DataType;

class Model {
public:
	Model();
	Model(char *);
	Model(InputImageParameters *, DataType);

	virtual ~ Model();

	void initialiseModel(int x, int y, int z);
	void initialiseModel();
	void buildBackProjection();

	void buildTestVolume(int x, int y, int z);
	void buildTestAxis();


	int getModelSizeX() const {
		return modelSizeX;
	}
	int getModelSizeY() const {
		return modelSizeY;
	}
	int getModelSizeZ() const {
		return modelSizeZ;
	}
	int getModelCenterX() const {
		return modelCenterX;
	}
	int getModelCenterY() const {
		return modelCenterY;
	}
	int getModelCenterZ() const {
		return modelCenterZ;
	}
	DataType ***getModel() const {
		return model;
	}
	void setModelCoordinate(int x, int y, int z, DataType value) {
		model[x][y][z] = value;
	}
	DataType getModelCoordinate(int x, int y, int z) const {
		return model[x][y][z];
	}
	void mergeModels(const Model * loopModel, const Model * crystalModel);
	void mergeModel(const Model * modelIn, DataType type);
	DataType getVoxelValueFromRealCoordinates(float x, float y, float z);

	CvPoint3D32f getRealCoordinatesFromVoxelMap(int u, int v, int w);
	CvPoint3D getVoxelCoordinatesFromRealCoordinates(float x, float y, float z);

	void progressBar(int current, int total);

	void save(char *path);
	void saveINR(char *path);
	void load(char *path);
	void loadINR(char *path);
	void dumpModel();

private:
	log4cplus::Logger myLogger; // = log4cplus::Logger::getInstance("model");
	DataType ***model; // volume
	DataType dataType;

	int modelSizeX, modelSizeY, modelSizeZ;
	int modelCenterX, modelCenterY, modelCenterZ;
	int blocksize;

	// voxel size in microns
	int voxelSizeX;
	int voxelSizeY;
	int voxelSizeZ;

	// matrix wich converts model coordinates (u,v,w)
	// to the world coordinates (x,y,z)
	// [u,v,w]^T = [conv][x,y,z,1]^T
	Matrix convMat;

	InputImageParameters *inputImageParameters;

	void buildImageMap(map<float, IplImage*> &imageMap);
	void deleteImageMap(map<float, IplImage*> &imageMap);
	void initialiseModelFromFile();


};

#endif /*MODEL_H_ */
