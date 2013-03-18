/*
 * MeshProcessing.cpp
 *
 *  Created on: 27 Dec 2009
 *      Author: leal
 */

#include <iostream>
#include <fstream>
#include <opencv/cxcore.h>

#include "../common/common.h"
#include "Mesh.h"

using namespace std;

Mesh::Mesh() :
	myLogger(log4cplus::Logger::getInstance("mesh")) {
	// TODO Auto-generated constructor stub

}

Mesh::Mesh(char *offFilePath_) :
	myLogger(log4cplus::Logger::getInstance("mesh")) {
	loadOffFile(offFilePath_);

}

Mesh::~Mesh() {
	// TODO Auto-generated destructor stub
}

/*
 * Read Off File
 Line 1
 OFF
 Line 2
 vertex_count face_count edge_count
 One line for each vertex:
 x y z
 for vertex 0, 1, ..., vertex_count-1
 One line for each polygonal face:
 n v1 v2 ... vn,
 the number of vertices, and the vertex indices for each face.

 */
void Mesh::loadOffFile(char *offFilePath_) {
	int vertexCount;
	int faceCount;
	int edgeCount;

	ifstream fileIn(offFilePath_, ifstream::out);
	if ((fileIn.rdstate() & ifstream::failbit ) != 0 ) {
		LOG4CPLUS_FATAL(myLogger, "Load OFF file: File doesn't exist: " << offFilePath_);
		return;
	}

	LOG4CPLUS_INFO(myLogger, "Loading mesh Off file: " << offFilePath_);

	const int BUFFER_SIZE = 1024; // Buffer size;
	char line[BUFFER_SIZE];
	//memset(line, 0, BUFFER_SIZE);

	if (fileIn.getline(line, BUFFER_SIZE) && strcmp(strToUpper(line), "OFF")
			!= 0) {
		LOG4CPLUS_FATAL(myLogger, "Load OFF File: File doesn't start with OFF");
		return;
	}

	while (fileIn.getline(line, BUFFER_SIZE) && (line[0] == '#' || strcmp(line,
			"") == 0)) {
		// eats empty or comment lines

	}
	sscanf(line, "%d%d%d", &vertexCount, &faceCount, &edgeCount);

	//map indexed by the vertices number
	std::map<int, CvPoint3D32f> verticesMap;
	int vertex = 0;
	while (vertex < vertexCount) {
		fileIn.getline(line, BUFFER_SIZE);
		if (line[0] != '#' && trim(string(line)).empty() == false) {
			float x, y, z;
			sscanf(line, "%f%f%f", &x, &y, &z);
			CvPoint3D32f p = cvPoint3D32f(x, y, z);
			verticesMap[vertex] = p;
			vertex++;
			//cout << p << endl;
		}
	}

	// triangles
	int face = 0;
	while (fileIn.getline(line, BUFFER_SIZE) && face < faceCount) {
		if (line[0] != '#' && trim(string(line)).empty() == false) {
			int n, v1, v2, v3;
			sscanf(line, "%d%d%d%d", &n, &v1, &v2, &v3);
			if (n != 3) {
				LOG4CPLUS_FATAL(myLogger, "Load OFF File: Expecting a triangle! Got: " << n);
				return;
			}
			Triangle t = triangle(verticesMap[v1], verticesMap[v2],
					verticesMap[v3]);
			face++;
			triangleList.push_back(t);
			//cout << v1 << " " << v2 << " " << v3 << " -> ";
			//cout << t << endl;
		}

	}

}
//
//CvPoint3D64f* MeshProcessing::intersectTriangle(double orig[3], double dir[3],
//		double vert0[3], double vert1[3], double vert2[3]) {
//
//	double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
//	double det, inv_det;
//	double t, u, v;
//	/* find vectors for two edges sharing vert0 */
//	SUB(edge1, vert1, vert0);
//	SUB(edge2, vert2, vert0);
//	/* begin calculating determinant - also used to calculate U parameter */
//	CROSS(pvec, dir, edge2);
//	/* if determinant is near zero, ray lies in plane of triangle */
//	det = DOT(edge1, pvec);
//	/* the non-culling branch */
//	if (det > -EPSILON && det < EPSILON)
//		return NULL;
//	inv_det = 1.0 / det;
//	/* calculate distance from vert0 to ray origin */
//	SUB(tvec, orig, vert0);
//	/* calculate U parameter and test bounds */
//
//	u = DOT(tvec, pvec) * inv_det;
//	if (u < 0.0 || u > 1.0)
//		return NULL;
//	/* prepare to test V parameter */
//	CROSS(qvec, tvec, edge1);
//	/* calculate V parameter and test bounds */
//	v = DOT(dir, qvec) * inv_det;
//	if (v < 0.0 || u + v > 1.0)
//		return NULL;
//	/* calculate t, ray intersects triangle */
//	t = DOT(edge2, qvec) * inv_det;
//	if (t < 0.0 || t > 1.0)
//		return NULL;
//
//	CvPoint3D64f *ret = new CvPoint3D64f();
//	*ret = cvPoint3D64f(t, u, v);
//	return ret;
//
//}


//CvPoint3D32f* MeshProcessing::intersectTriangle(CvPoint3D32f orig_,
//		CvPoint3D32f dir_, Triangle tr) {
//
//	float orig[3] = { orig_.x, orig_.y, orig_.z };
//	float dir[3] = { dir_.x, dir_.y, dir_.z };
//	float vert0[3] = { tr.vertices[0].x, tr.vertices[0].y, tr.vertices[0].z };
//	float vert1[3] = { tr.vertices[1].x, tr.vertices[1].y, tr.vertices[1].z };
//	float vert2[3] = { tr.vertices[2].x, tr.vertices[2].y, tr.vertices[2].z };
//
//	float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
//	float det, inv_det;
//	float t, u, v;
//	/* find vectors for two edges sharing vert0 */
//	SUB(edge1, vert1, vert0);
//	SUB(edge2, vert2, vert0);
//	/* begin calculating determinant - also used to calculate U parameter */
//	CROSS(pvec, dir, edge2);
//	/* if determinant is near zero, ray lies in plane of triangle */
//	det = DOT(edge1, pvec);
//	/* the non-culling branch */
//	if (det > -EPSILON && det < EPSILON)
//		return NULL;
//	inv_det = 1.0 / det;
//	/* calculate distance from vert0 to ray origin */
//	SUB(tvec, orig, vert0);
//	/* calculate U parameter and test bounds */
//
//	u = DOT(tvec, pvec) * inv_det;
//	if (u < 0.0 || u > 1.0)
//		return NULL;
//	/* prepare to test V parameter */
//	CROSS(qvec, tvec, edge1);
//	/* calculate V parameter and test bounds */
//	v = DOT(dir, qvec) * inv_det;
//	if (v < 0.0 || u + v > 1.0)
//		return NULL;
//	/* calculate t, ray intersects triangle */
//	t = DOT(edge2, qvec) * inv_det;
//	if (t < 0.0 || t > 1.0)
//		return NULL;
//
//	CvPoint3D32f *ret = new CvPoint3D32f();
//	ret->x = orig_.x + t * dir_.x;
//	ret->y = orig_.y + t * dir_.y;
//	ret->z = orig_.z + t * dir_.z;
//	return ret;
//
//}


/*
 // Copyright 2001, softSurfer (www.softsurfer.com)
 // This code may be freely used and modified for any purpose
 // providing that this copyright notice is included with it.
 // SoftSurfer makes no warranty for this code, and cannot be held
 // liable for any real or imagined damage resulting from its use.
 // Users of this code must verify correctness for their application.

 // intersect_RayTriangle(): intersect a ray with a 3D triangle
 //    Input:  a ray R, and a triangle T
 //    Output: *I = intersection point (when it exists)
 //    Return: -1 = triangle is degenerate (a segment or point)
 //             0 = disjoint (no intersect)
 //             1 = intersect in unique point I1
 //             2 = are in the same plane
 */

/***
 *
 * orig: model centre
 * dir: normalised direction vector
 * tr: mesh triangle
 *
 */
CvPoint3D32f* Mesh::intersectTriangle(CvPoint3D32f orig,
		CvPoint3D32f dir, Triangle tr) {

	CvPoint3D32f u, v, n; // triangle vectors
	CvPoint3D32f w0, w; // ray vectors
	float r, a, b; // params to calc ray-plane intersect

	// get triangle edge vectors and plane normal
	u = tr.vertices[1] - tr.vertices[0];
	v = tr.vertices[2] - tr.vertices[0];
	n = crossProduct(u, v); // cross product
	if (n.x == 0 && n.y == 0 && n.z == 0) // triangle is degenerate
		return NULL; // do not deal with this case

	//dir = R.P1 - R.P0; // ray direction vector
	w0 = orig - tr.vertices[0];
	a = -dot(n,w0);
	b = dot(n,dir);
	if (fabs(b) < SMALL_NUM) { // ray is parallel to triangle plane
		if (a == 0) // ray lies in triangle plane
			return NULL;
		else
			return NULL; // ray disjoint from plane
	}

	// get intersect point of ray with triangle plane
	r = a / b;
	if (r < 0.0) // ray goes away from triangle
		return NULL; // => no intersect
	// for a segment, also test if (r > 1.0) => no intersect

	//I = R.P0 + r * dir; // intersect point of ray and plane
	CvPoint3D32f *ret = new CvPoint3D32f();
	ret->x = orig.x + r * dir.x;
	ret->y = orig.y + r * dir.y;
	ret->z = orig.z + r * dir.z;

	// is I inside T?
	float uu, uv, vv, wu, wv, D;
	uu = dot(u,u);
	uv = dot(u,v);
	vv = dot(v,v);
	w = *ret - tr.vertices[0];
	wu = dot(w,u);
	wv = dot(w,v);
	D = uv * uv - uu * vv;

	// get and test parametric coords
	float s, t;
	s = (uv * wv - vv * wu) / D;
	if (s < 0.0 || s > 1.0) // I is outside T
		return NULL;
	t = (uv * wu - uu * wv) / D;
	if (t < 0.0 || (s + t) > 1.0) // I is outside T
		return NULL;

	return ret; // I is in T


}

/**
 *
 * For test purposes
 */
void Mesh::testTriangleIntersection() {

	std::map<CvPoint3D32f*, Triangle> intersectionsMap;

	CvPoint3D32f centre = cvPoint3D32f(250, 250, 250);
	CvPoint3D32f direction = cvPoint3D32f(-1, -1, -1);

	for (unsigned int i = 0; i < triangleList.size(); i = i + 1) {
		Triangle t = triangleList[i];

		CvPoint3D32f *p =  intersectTriangle(centre, direction, t);

		//std::cout << "Testing Intersection: t=" << t << std::endl;
		if (p != NULL) {
			intersectionsMap[p]=t;
			std::cout << "Intersection: t=" << t << ", p=" << *p << std::endl;
		}
	}

}

