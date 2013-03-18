#include "Rotation.h"
#include "../common/common.h"

#include <cmath>
#include <opencv/cxcore.h>

Rotation::Rotation()
{
}

Rotation::Rotation(float angle)
{
	phi = angle;
	cosPhi = cos(angle); 
	sinPhi = sin(angle);
}

Rotation::~Rotation()
{
}

CvPoint3D Rotation::rotateX(CvPoint3D point,CvPoint3D center) {
	
	
	//translate to the (0,0,0)
	//CvPoint3D pTranslated = CvPoint3D(point.x - center.x, point.y - center.y, point.z - center.z);
	CvPoint3D pTranslated = cvPoint3D(point.x, point.y - center.y, point.z - center.z);
			
	// rotate in the (0,0)
	//	1  	0  	    0		 * (Xi - Xc)
	//  0 	cos(a) 	-sin(a)
	//  0 	sin(a) 	cos(a)
	//float x = pTranslated.x;
	float y = pTranslated.y * cosPhi - pTranslated.z * sinPhi;
	float z = pTranslated.y * sinPhi + pTranslated.z * cosPhi;
		
	//translate to the center of rotation
	//CvPoint3D res = cvPoint3D(cvRound(pTranslated.x + center.x),cvRound(pTranslated.y + center.y),cvRound(pTranslated.z + center.z))
	CvPoint3D res = cvPoint3D(pTranslated.x,cvRound(y + center.y),cvRound(z + center.z));
	
	return res;
}


CvPoint3D32f Rotation::rotateX(CvPoint3D32f point,CvPoint3D32f center) {
	
	
	//translate to the (0,0,0)
	//CvPoint3D pTranslated = CvPoint3D(point.x - center.x, point.y - center.y, point.z - center.z);
	CvPoint3D32f pTranslated = cvPoint3D32f(point.x, point.y - center.y, point.z - center.z);
		
	// rotate in the (0,0)
	//	1  	0  	    0		 * (Xi - Xc)
	//  0 	cos(a) 	-sin(a)
	//  0 	sin(a) 	cos(a)
	//float x = pTranslated.x;
	float y = pTranslated.y * cosPhi - pTranslated.z * sinPhi;
	float z = pTranslated.y * sinPhi + pTranslated.z * cosPhi;
		
	//translate to the center of rotation
	//CvPoint3D res = cvPoint3D(cvRound(pTranslated.x + center.x),cvRound(pTranslated.y + center.y),cvRound(pTranslated.z + center.z))
	CvPoint3D32f res = cvPoint3D32f(pTranslated.x,cvRound(y + center.y),cvRound(z + center.z));
	
	return res;
}

