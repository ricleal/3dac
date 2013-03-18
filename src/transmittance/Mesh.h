/*
 * MeshProcessing.h
 *
 *  Created on: 27 Dec 2009
 *      Author: leal
 */

#ifndef MESHPROCESSING_H_
#define MESHPROCESSING_H_

#include "../common/common.h"


#define SMALL_NUM  0.00000001 // anything that avoids division overflow
// dot product (3D) which allows vector operations in arguments
#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)

//define EPSILON 0.000001
//
//define CROSS(dest,v1,v2) dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

//define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
//
//define SUB(dest,v1,v2) dest[0]=v1[0]-v2[0]; dest[1]=v1[1]-v2[1]; dest[2]=v1[2]-v2[2]

class Mesh {
public:
	Mesh();
	Mesh(char *offFilePath_);
	virtual ~Mesh();

//	CvPoint3D64f* intersectTriangle(double orig[3], double dir[3],
//			double vert0[3], double vert1[3], double vert2[3]);
	CvPoint3D32f* intersectTriangle(CvPoint3D32f orig, CvPoint3D32f dir,
			Triangle t);
	void testTriangleIntersection();

private:
	log4cplus::Logger myLogger;
	void loadOffFile(char *offFilePath_);

	std::vector<Triangle> triangleList;

};

#endif /* MESHPROCESSING_H_ */
