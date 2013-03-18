#ifndef INPUTIMAGEPARAMETERS_H_
#define INPUTIMAGEPARAMETERS_H_

#include "../ini_parser/iniparser.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std;

class InputImageParameters {
  public:
    InputImageParameters();
    InputImageParameters(const char *);

     virtual ~ InputImageParameters();

    int parseConfigFile(dictionary *);
    void imagesToProcess();

    //Getter and Setter for modelFilePath
    char *getModelFilePath() const {
        return modelFilePath;
    };
    void setModelFilePath(char *modelFilePath_p) {
        modelFilePath = modelFilePath_p;
    };

    string getBackgroundCompletePath() {
        return backgroundCompletePath;
    };
    vector < string > getImagesPathToProccess()const {
        return v_imagesPathToProccess;
    };
    vector < int >getAnglesToProcess() const {
        return v_anglesToProcess;
    };
    //Getter and Setter for process
    char *getProcess() const { return process; };
    void setProcess(char *process_p) { process = process_p; };


  private:

    const char *section;

    char *process;
    char *imagesPathFolder;
    char *imagesPatternFileName;
    char *backgroundFileName;
    char *imagesListToProccess;
    int imageListStep;

    char *modelFilePath;
    string backgroundCompletePath;

    vector < string > v_imagesPathToProccess;
    vector < int >v_anglesToProcess;

    int wordOccurrenceCount(std::string const &str, std::string const &word);
    string fillInteger(int , int );
    vector < int >processImageNumbers();

};

#endif /*INPUTIMAGEPARAMETERS_H_ */
