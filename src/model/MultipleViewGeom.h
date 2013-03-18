/*
 * MultipleViewGeom2.h
 *
 *  Created on: Jun 9, 2009
 *      Author: leal
 */

#ifndef MultipleViewGeom2_H_
#define MultipleViewGeom2_H_

#include <opencv/cv.h>

#include "../common/common.h"
#include <newmat11/newmatap.h>

using namespace std;


/**
 * Matrixes [K] [P] [R|T]
 */

class MultipleViewGeom {

public:
	MultipleViewGeom();
	MultipleViewGeom(int u0, int v0, float alfaU, float alfaV);
	virtual ~MultipleViewGeom();

private:
	log4cplus::Logger myLogger;

	// K P R|T
	Matrix pMat;
	Matrix kMat;
	//CvMat rtMat;

	// x = projMat X
	//CvMat projMat;

	// calculated projection matrices for certain angles
	map<float,Matrix*> projMatList;


	Matrix* calculateRotationMatrix(float);
	void calculateCameraMatrix(int, int, float, float);
	Matrix* calculateProjectionMatrix(Matrix);


public:

	CvPoint getProjectionOf(float , CvPoint3D32f);



};

#endif /* MultipleViewGeom2_H_ */
