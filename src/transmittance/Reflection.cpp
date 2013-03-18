#include "Reflection.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

ReflectionItem::ReflectionItem()
{
}

ReflectionItem::~ReflectionItem()
{

}

ReflectionItem::ReflectionItem(int h_p, int k_p, int l_p, int misym_p, int batch_p)
{
    h = h_p;
    k = k_p;
    l = l_p;
    misym = misym_p;
    batch = batch_p;

    xDet = 0;
    yDet = 0;
    iObs = 0;
    sigiObs = 0;
    iprObs = 0;
    sigiprObs = 0;
    
    resolution = 0;

    iCorr = 0;
    sigiCorr = 0;
    
    iprCorr = 0;
    sigiprCorr = 0;

    totalIncidentBeamMagnitude = 0;
    totalDiffractedBeamMagnitude = 0;
    totalBeamTransmittance = 0;

    crystalIncidentBeamMagnitude = 0;
    crystalDiffractedBeamMagnitude = 0;
    crystalBeamTransmittance = 0;

    liquidIncidentBeamMagnitude = 0;
    liquidDiffractedBeamMagnitude = 0;
    liquidBeamTransmittance = 0;

    loopIncidentBeamMagnitude = 0;
    loopDiffractedBeamMagnitude = 0;
    loopBeamTransmittance = 0;

    airIncidentBeamMagnitude = 0;
    airDiffractedBeamMagnitude = 0;
    airBeamTransmittance = 0;

    crystalVolume = 0;
    xDetReal = 0;
    yDetReal = 0;

}

/*
 * Operator ==
 * these are the 5 attributes that distinguish a reflection
 */
bool ReflectionItem::operator==(const ReflectionItem & r) const 
{
    if (h == r.h && k == r.k && l == r.l && misym == r.misym && batch == r.batch)
        return true;
    else
        return false;
}

/*
 * Operator =<
 * these are the 3 attributes that distinguish a reflection
 */
bool ReflectionItem::operator<=(const ReflectionItem & r) const 
{
    if (h == r.h && k == r.k && l == r.l)
        return true;
    else
        return false;
}

bool ReflectionItem::isBatch(int i) 
{
	if (batch == i)
		return true;
	else
		return false;

}

/* 
 * Reflexion
 */

Reflection::Reflection()
{

}

Reflection::~Reflection()
{
    std::vector < ReflectionItem * >::iterator iter;

    for (iter = reflectionList.begin(); iter != reflectionList.end(); iter++)
    {
        ReflectionItem *item = *iter;

        //free(item);
        delete(item);

    }
}

void Reflection::appendItem(ReflectionItem * r)
{
    //cout << "Size: " << reflectionList.size() << " Adding element: " << *r;
    reflectionList.push_back(r);
}

// DUMP the reflections to a csv file 
void Reflection::csvDump(char *path)
{
    if (path != NULL || !strcmp(path,""))
    {

        ofstream myfile;
        myfile.open(path);
        myfile << *this;
        myfile.close();
    }

}
