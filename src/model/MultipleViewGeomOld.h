/*
 * MultipleViewGeom.h
 *
 *  Created on: Jun 9, 2009
 *      Author: leal
 */

#ifndef MULTIPLEVIEWGEOM_H_
#define MULTIPLEVIEWGEOM_H_

#include <opencv/cv.h>

#include "../common/common.h"

using namespace std;


/**
 * Matrixes [K] [P] [R|T]
 */

class MultipleViewGeomOld {

public:
	MultipleViewGeomOld();
	MultipleViewGeomOld(int u0, int v0, float alfaU, float alfaV);
	virtual ~MultipleViewGeomOld();

private:
	log4cplus::Logger myLogger;

	// K P R|T
	CvMat *kMat;
	CvMat *pMat;
	//CvMat rtMat;

	// x = projMat X
	//CvMat projMat;

	// calculated projection matrices for certain angles
	map<float,CvMat*> projMatList;


	CvMat* calculateRotationMatrix(float);
	void calculateCameraMatrix(int, int, float, float);
	CvMat* calculateProjectionMatrix(CvMat* rtMat);


public:

	CvPoint getProjectionOf(float , CvPoint3D32f);



};

#endif /* MULTIPLEVIEWGEOM_H_ */
