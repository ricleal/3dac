#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

#include "InputImageParameters.h"
#include "InputParameters.h"
#include "../ini_parser/iniparser.h"
#include "../common/common.h"

InputImageParameters::InputImageParameters()
{

}

InputImageParameters::InputImageParameters(const char *sect)
{
    section = sect;
}

InputImageParameters::~InputImageParameters()
{
}

int
 InputImageParameters::parseConfigFile(dictionary * d)
{

	char completeKey[1024];

    if (d != NULL)
    {

    	sprintf(completeKey,"%s:%s", section,"process");
    	process = iniparser_getstring(d, completeKey, NULL);

    	sprintf(completeKey,"%s:%s", section,"images_path_folder");
        imagesPathFolder = iniparser_getstring(d, completeKey, NULL);

        sprintf(completeKey,"%s:%s", section,"images_pattern_file_name");
        imagesPatternFileName = iniparser_getstring(d, completeKey, NULL);

        sprintf(completeKey,"%s:%s", section,"background_file_name");
        backgroundFileName = iniparser_getstring(d, completeKey, NULL);

        sprintf(completeKey,"%s:%s", section,"image_list_to_proccess");
        imagesListToProccess = iniparser_getstring(d, completeKey, NULL);
        
        sprintf(completeKey,"%s:%s", section,"model_file_path");
        modelFilePath = iniparser_getstring(d, completeKey, NULL);
        
        sprintf(completeKey,"%s:%s", section,"image_list_step");
        char *imageListStepStr = iniparser_getstring(d, completeKey, NULL);
        imageListStep = stringToInt(imageListStepStr);

    }

    return 0;
}

/*
 * Builds a vector of the form:
 * /images_path/image_name_000.tif
 * ...
 * /images_path/image_name_090.tif
 */
void InputImageParameters::imagesToProcess()
{

    //Logger log = Logger(ERROR, &cout);

    string imagesPathFolderStr = imagesPathFolder;

    // if the imagesPathFolder doesn't end with /
    if (imagesPathFolderStr.at(imagesPathFolderStr.size() - 1) != '/')
        imagesPathFolderStr += "/";

    if (backgroundFileName != NULL)
    {
        backgroundCompletePath = imagesPathFolderStr;
        backgroundCompletePath += backgroundFileName;

    }

    // mumber of ??? in the imagesPatternFileName
    int NDigits = wordOccurrenceCount(imagesPatternFileName, "?");

    // gives a list with the images numbers to process
    vector < int >v_imageNumbers = processImageNumbers();

    for (unsigned int i = 0; i < v_imageNumbers.size(); i++)
    {

        // replace image_??? for image_001.
        string digits = fillInteger(v_imageNumbers[i], NDigits);
        string imagesPatternFileNameStr = imagesPatternFileName;
        int pos = imagesPatternFileNameStr.find("?");
        imagesPatternFileNameStr.replace(pos, NDigits, digits);
        // complete path
        string totalPath = imagesPathFolderStr;
        totalPath += imagesPatternFileNameStr;

        v_imagesPathToProccess.push_back(totalPath);
        //log << DEBUG << "Path Built: " << v_imagesPathToProccess[i]<<  "\n";
    }
}

/*
 * receives this: 0-20, 40, 50-70, if step 10, returns:
 * 0,10,40,50,60,70
 */
vector < int >InputImageParameters::processImageNumbers()
{

    vector < char *>v_chars;

    // split by ,
    char *pch;
    pch = strtok(imagesListToProccess, ", ");   //,
    while (pch != NULL)
    {
        v_chars.push_back(pch);
        pch = strtok(NULL, ", ");
    }

    // split by - and built list
    vector < int >v_ints;
    for (unsigned int i = 0; i < v_chars.size(); i++)
    {
        pch = strtok(v_chars[i], "-");  //-
        if (pch != NULL)
        {
            int start = stringToInt(pch);
            pch = strtok(NULL, "-");
            if (pch != NULL)
            {
                int end = stringToInt(pch);

                for (int j = start; j <= end; j += imageListStep)
                {
                    v_ints.push_back(j);
                }
            }
            else
            {
                v_ints.push_back(stringToInt(v_chars[i]));
            }
        }

    }

    v_anglesToProcess = v_ints;

    return v_ints;
}

/**
 * Count the number of words in one string 
 */
int InputImageParameters::wordOccurrenceCount(std::string const &str, std::string const &word) {
    int count(0);
    std::string::size_type word_pos(0);
    while (word_pos != std::string::npos)
    {
        word_pos = str.find(word, word_pos);
        if (word_pos != std::string::npos)
        {
            ++count;

            // start next search after this word
            word_pos += word.length();
        }
    }

    return count;
}

string InputImageParameters::fillInteger(int number, int precision)
{
    stringstream os;
    os << setfill('0');
    os << setw(precision);
    os << number;
    return os.str();
}
