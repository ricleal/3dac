#ifndef ROTATION_H_
#define ROTATION_H_

#include "../common/common.h"
#include <opencv/cxcore.h>

class Rotation
{
public:
	Rotation();
	Rotation(float angle);
	virtual ~Rotation();

	CvPoint3D rotateX(CvPoint3D point,CvPoint3D center);
	CvPoint3D32f rotateX(CvPoint3D32f point,CvPoint3D32f center);
	

private:
	float phi;
	float sinPhi;
	float cosPhi;
};

#endif /*ROTATION_H_*/
