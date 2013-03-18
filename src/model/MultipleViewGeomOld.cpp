/*
 * MultipleViewGeom.cpp
 *
 *  Created on: Jun 9, 2009
 *      Author: leal
 */

/* Use:
 * MultipleViewGeom m = MultipleViewGeom(u0,v0,alfaU,alfaV);
 * CvPoint3D p = getProjectionOf(angle,world3DPoint);
 */

#include "MultipleViewGeomOld.h"

/**
 *
 * Constructor for test purposes
 *
 */
MultipleViewGeomOld::MultipleViewGeomOld() :
	myLogger(log4cplus::Logger::getInstance("multipleViewGeom")) {

	// Ortographic projection
	float pContents[] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 };
	//pMat = cvMat(3, 4, CV_32FC1, pContents);

	pMat = cvCreateMat(3, 4, CV_32F);
	cvInitMatHeader(pMat, 3, 4, CV_32F, pContents);
	pMat = cvCloneMat(pMat);


	//float projMatContents[3][4];
	//projMat = cvMat(3, 4, CV_32FC1, projMatContents);

}
/*
 * Constructor
 *
 * (u0,v0) = image centre
 * alfaU = f*ku = pixels per unit of measurement in Y e.g. 1 pixel/microm
 * alfaV
 */
MultipleViewGeomOld::MultipleViewGeomOld(int u0, int v0, float alfaU, float alfaV) :
	myLogger(log4cplus::Logger::getInstance("multipleViewGeom")) {

	// Ortographic projection
	float pContents[] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 };
	pMat = cvCreateMat(3, 4, CV_32F);
	cvInitMatHeader(pMat, 3, 4, CV_32F, pContents);
	pMat = cvCloneMat(pMat);

	calculateCameraMatrix(u0, v0, alfaU, alfaV);

}

/**
 * Desctructor
 *
 * Clean hash table with matrices
 */
MultipleViewGeomOld::~MultipleViewGeomOld() {
	map<float, CvMat*>::iterator it;

	for (it = projMatList.begin(); it != projMatList.end(); it++) {
		cvReleaseMat(&(it->second));
	}

}

/**
 * Calculate K matrix
 *
 *  ------------> u
 * |
 * |      ^ Y
 * |      |
 * |      |
 * |       -----> X
 * |
 * |
 * V v
 *
 */
void MultipleViewGeomOld::calculateCameraMatrix(int u0, int v0, float alfaU,
		float alfaV) {

	//     au  0 u0
	// k =  0 av v0
	//      0  0  1

	float a[] = { alfaU, 0, u0, 0, alfaV, v0, 0, 0, 1 };

	kMat = cvCreateMat(3, 3, CV_32F);
	cvInitMatHeader(kMat, 3, 3, CV_32F, a);
	kMat = cvCloneMat(kMat);

	LOG4CPLUS_DEBUG(myLogger,"Camera K matrix" << endl << printCvMat(kMat));

}


/**
 * Calculate pose of the camera. Since no translation are made
 * Only the rotation is calculated.
 *
 * [R|T]
 */
CvMat* MultipleViewGeomOld::calculateRotationMatrix(float angle) {

	// | R T |
	// | 0 1 |
	// 1 	 0 	    0 	0
	// 0 cos() -sin()   0
	// 0 sin()  cos()   0
	// 0     0      0   1

	float sinTeta = sin(angle);
	float cosTeta = cos(angle);

	float a[] = { 1, 0, 0, 0, 0, cosTeta, -sinTeta, 0, 0, sinTeta, cosTeta, 0,
			0, 0, 0, 1 };

	//CvMat rtMat = cvMat(4, 4, CV_32FC1, a);
	//rtMat = *cvCloneMat(&rtMat);

	CvMat* rtMat = cvCreateMat(4, 4, CV_32F);
	cvInitMatHeader(rtMat, 4, 4, CV_32F, a);
	rtMat = cvCloneMat(rtMat);

	LOG4CPLUS_DEBUG(myLogger,"Rotation R|T matrix for angle: " << angle << endl << printCvMat(rtMat));

	return rtMat;

}



/**
 * Calculate: K P R|T
 */
CvMat* MultipleViewGeomOld::calculateProjectionMatrix(CvMat *rtMat) {


	// 3 rows, 4 columns
	CvMat* kpMat = cvCreateMat(3, 4, CV_32FC1);
	cvMatMul(kMat,pMat,kpMat);

	CvMat* projMat = cvCreateMat(3, 4, CV_32FC1);
	cvMatMul(kpMat,rtMat,projMat);

	projMat = cvCloneMat(projMat);

	LOG4CPLUS_DEBUG(myLogger,"Projection K P R|T matrix" << endl << printCvMat(projMat));

	return projMat;
}

/**
 * Projects a point in real world coordinates against the image
 * Output: image coordinate in pixels
 */
CvPoint MultipleViewGeomOld::getProjectionOf(float angle, CvPoint3D32f point) {

	//
	map<float, CvMat*>::iterator iter = projMatList.find(angle);

	CvMat *projMat = cvCreateMat(4, 3, CV_32FC1);

	if (iter == projMatList.end()) {
		// project matrix does not exist!!

		// Calculate rotation matrix
		CvMat* rtMat = calculateRotationMatrix(angle);

		// Calculate projection matrix
		projMat = calculateProjectionMatrix(rtMat);

		projMat = cvCloneMat(projMat);

		projMatList.insert(pair<float, CvMat*> (angle, projMat));

	} else {
		// otherwise it exists
		projMat = iter->second;
	}

	LOG4CPLUS_DEBUG(myLogger,"Projection matrix for angle: " << radToDegree(angle) << " and points: " << point << endl <<  printCvMat(projMat));

	//  [u v 1] = proj * [X Y Z 1]

	float uvContents[3];
	//CvMat* uvMat = cvMat(3, 1, CV_32F, uvContents);
	CvMat* uvMat = cvCreateMat(3, 1, CV_32F);
	cvInitMatHeader(uvMat, 3, 1, CV_32F, uvContents);


	float xyzContents[] = { point.x, point.y, point.z, 1 };
	//CvMat* xyzMat = cvMat(4, 1, CV_32F, xyzContents);
	CvMat* xyzMat = cvCreateMat(4, 1, CV_32F);
	cvInitMatHeader(xyzMat, 4, 1, CV_32F, xyzContents);

	cvMatMul (projMat, xyzMat,uvMat);

	LOG4CPLUS_DEBUG(myLogger, "Result [u v 1] = proj * [X Y Z 1]: " << endl << printCvMat(uvMat));

	return cvPoint(cvRound(cvmGet(uvMat, 0, 0)), cvRound(cvmGet(uvMat, 1, 0)));

}

