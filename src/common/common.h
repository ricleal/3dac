#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <fstream>
#include <ostream>
#include <sstream>
#include <cmath>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/layout.h>
#include <log4cplus/ndc.h>

#include <algorithm>

#define PI 3.14159265

typedef struct CvPoint3D {
	int x;
	int y;
	int z;
	//

} CvPoint3D;

inline CvPoint3D cvPoint3D(int x, int y, int z) {
	CvPoint3D p;

	p.x = x;
	p.y = y;
	p.z = z;

	return p;
}

typedef struct Triangle {
	CvPoint3D32f vertices[3];
} Triangle;

inline Triangle triangle(CvPoint3D32f v1, CvPoint3D32f v2, CvPoint3D32f v3) {
	Triangle t;
	t.vertices[0] = v1;
	t.vertices[1] = v2;
	t.vertices[2] = v3;

	return t;
}

inline static void operator +=(CvPoint3D32f &p1, const CvPoint3D32f &p2) {
	p1.x += p2.x;
	p1.y += p2.y;
	p1.z += p2.z;
}

inline static CvPoint3D32f operator /(const CvPoint3D32f &p, int d) {
	return cvPoint3D32f(p.x / d, p.y / d, p.z / d);
}

inline static CvPoint3D32f operator +(CvPoint3D32f a, CvPoint3D32f b) {
	return cvPoint3D32f(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline static CvPoint3D32f operator -(CvPoint3D32f a, CvPoint3D32f b) {
	return cvPoint3D32f(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline static CvPoint3D32f operator *(CvPoint3D32f v, double f) {
	return cvPoint3D32f(f * v.x, f * v.y, f * v.z);
}

inline CvPoint3D32f crossProduct(CvPoint3D32f v1, CvPoint3D32f v2) {

	CvPoint3D32f ret = cvPoint3D32f(v1.y * v2.z - v1.z * v2.y,
			v1.z*v2.x - v1.x*v2.z,
			v1.x*v2.y - v1.y * v2.x);
	return ret;

}

inline std::ostream & operator <<(std::ostream & os, const CvPoint3D & p) {
	return os << "{" << p.x << " " << p.y << " " << p.z << "}";
}
;

inline std::ostream & operator <<(std::ostream & os, const CvPoint3D32f & p) {
	return os << "{" << p.x << " " << p.y << " " << p.z << "}";
}
;

inline std::ostream & operator <<(std::ostream & os, const Triangle & t) {
	return os << "[" << t.vertices[0] << "; " << t.vertices[1] << "; "
			<< t.vertices[2] << "]";
}
;

inline int charpToInt(char *s) {
	std::string str(s);
	std::istringstream strin(str);
	int i;
	strin >> i;
	return i;
}

inline float charpToFloat(char *s) {
	std::string str(s);
	std::istringstream strin(str);
	float i;
	strin >> i;
	return i;
}

// string trim
inline std::string trim(std::string s, const std::string drop = " ") {
	std::string r = s.erase(s.find_last_not_of(drop) + 1);
	return r.erase(0, r.find_first_not_of(drop));
}

inline int stringToInt(std::string str) {
	std::istringstream strin(str);
	int i;
	strin >> i;
	return i;
}

inline float stringToFloat(std::string str) {
	std::istringstream strin(str);
	float i;
	strin >> i;
	return i;
}

inline char *getTime() {
	time_t t = time(NULL);
	tm *tt = gmtime(&t);
	char s[8];
	sprintf(s, "%.2d:%.2d:%.2d - ", tt->tm_hour, tt->tm_min, tt->tm_sec);
	return strdup(s);
}

//
template<class TYPE> inline TYPE radToDegree(const TYPE & Rad) {
	return (TYPE) ((180.0 / PI) * Rad);

}

//
template<class TYPE> inline TYPE degreeToRad(const TYPE & Degree) {
	return (TYPE) ((PI / 180.0) * Degree);
}

template<class T> T exponentiation(const T & x, const int times) {
	T t = 1;
	for (int i = 0; i < times; i++) {
		t = t * x;
	}

	return t;
}

template<class T> T square(const T & x) {
	return x * x;
}

// length of a vector
template<class T> T magnitude(const T & x, const T & y, const T & z) {
	return sqrt(x * x + y * y + z * z);
}

// length of a vector
inline float magnitude(CvPoint3D p1, CvPoint3D p2) {
	return sqrt(square(p1.x - p2.x) + square(p1.y - p2.y) + square(p1.z - p2.z));
}

inline bool nullVector(CvPoint3D p) {
	if (p.x == 0 && p.y == 0 && p.z == 0)
		return true;
	else
		return false;

}

template<class T> T max(T t1, T t2) {
	return (t1 > t2 ? t1 : t2);
}

template<class T> T min(T t1, T t2) {
	return (t1 < t2 ? t1 : t2);
}
/*
 inline void printCvMat(CvMat* a)
 {
 int i,j;

 //printf("\nMatrix = :");
 for(i=0;i<a->rows;i++)
 {
 switch( CV_MAT_DEPTH(a->type) )
 {
 case CV_32F:
 case CV_64F:
 for(j=0;j<a->cols;j++)
 printf("%9.3f ", (float) cvGetReal2D( a, i, j ));
 break;
 case CV_8U:
 case CV_16U:
 for(j=0;j<a->cols;j++)
 printf("%6d",(int)cvGetReal2D( a, i, j ));
 break;
 default:
 break;
 }
 printf("\n");
 }
 //    printf("\n");
 }
 */
inline std::string printCvMat(CvMat* a) {
	int i, j;
	char buf[256];
	std::string out;
	for (i = 0; i < a->rows; i++) {
		switch (CV_MAT_DEPTH(a->type)) {
		case CV_32F:
		case CV_64F:
			for (j = 0; j < a->cols; j++) {
				sprintf(buf, "%9.3f ", (float) cvGetReal2D(a, i, j));
				out.append(buf);
				//memset (buf,0,256);
			}
			break;
		case CV_8U:
		case CV_16U:
			for (j = 0; j < a->cols; j++) {
				sprintf(buf, "%6d", (int) cvGetReal2D(a, i, j));
				out.append(buf);
				//memset (buf,0,256);
			}
			break;
		default:
			break;
		}
		out.append("\n");
	}
	return out;
}

// convert char* to upper case
inline char* strToUpper(char *inStr) {

	std::string str(inStr);
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);

	strcpy(inStr, str.c_str());
	return inStr;

}

inline char * memUsage()
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   double vm_usage     = 0.0;
   double resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;

   char output[1024];
   sprintf(output, "Usage :: Virtual memory: %10.2f :: Resident set size: %10.2f", vm_usage , resident_set);

   return strdup(output);
}



#endif /*COMMON_H_ */
