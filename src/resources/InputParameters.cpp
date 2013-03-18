#include <sstream>

#include "InputParameters.h"
#include "../ini_parser/iniparser.h"



using namespace std;

/*
 * Constructor
 */
InputParameters::InputParameters(char *file):myLogger(log4cplus::Logger::getInstance("parameters"))
{

    LOG4CPLUS_INFO(myLogger, "Reading Parameters File: " << file);

    d = iniparser_load(file);

//    visibleSet = new InputImageParameters("Visible");
//    laserSet = new InputImageParameters("Laser");
    volumeSet = new InputImageParameters("Volume");
    crystalSet = new InputImageParameters("Crystal");
    loopSet = new InputImageParameters("Loop");
}

InputParameters::~InputParameters()
{
	iniparser_freedict(d);
//    delete visibleSet;
//    delete laserSet;
    delete volumeSet;
    delete crystalSet;
    delete loopSet;
}

/*
 * Main function to parse the input file
 */
int InputParameters::parseConfigFile()
{

    if (d != NULL)
    {
    	steps = iniparser_getstring(d, "General:steps", NULL);

        inputReflectionsFilePath = iniparser_getstring(d, "General:input_reflections_file_path", NULL);
        outputReflectionsFilePath = iniparser_getstring(d, "General:output_reflections_file_path", NULL);

        outputInrGlobalmodelFilePath = iniparser_getstring(d, "General:output_inr_global_model_file_path", NULL);
        inputInrGlobalmodelFilePath = iniparser_getstring(d, "General:input_inr_global_model_file_path", NULL);

        offCrystalModelFilePath = iniparser_getstring(d, "General:off_crystal_model_file_path", NULL);
        offLoopModelFilePath = iniparser_getstring(d, "General:off_loop_model_file_path", NULL);
        offVolumeModelFilePath = iniparser_getstring(d, "General:off_volume_model_file_path", NULL);

        inrCrystalModelFilePath = iniparser_getstring(d, "General:inr_crystal_model_file_path", NULL);
        inrLoopModelFilePath = iniparser_getstring(d, "General:inr_loop_model_file_path", NULL);
        inrVolumeModelFilePath = iniparser_getstring(d, "General:inr_volume_model_file_path", NULL);

        modelToProcess = iniparser_getstring(d, "General:model_to_process", NULL);

        outputCsvFilePath = iniparser_getstring(d, "General:output_csv_file_path", NULL);

        beamSizeX = iniparser_getint(d, "General:beam_size_x", -1);
        beamSizeY = iniparser_getint(d, "General:beam_size_y", -1);
        nDiffractedBeams = iniparser_getint(d, "General:n_diffracted_beams",-1);
        
        LOG4CPLUS_INFO(myLogger, "Beam size=" << beamSizeX << "x" << beamSizeY << "; N.beams=" << nDiffractedBeams);
        
        absorptionCoefficientLiquid = iniparser_getdouble(d, "General:absorption_coefficient_liquid",-1);
        absorptionCoefficientLoop = iniparser_getdouble(d, "General:absorption_coefficient_loop",-1);
        absorptionCoefficientCrystal =  iniparser_getdouble(d, "General:absorption_coefficient_crystal",-1);
        absorptionCoefficientAir = iniparser_getdouble(d, "General:absorption_coefficient_air",-1);
        
        LOG4CPLUS_INFO(myLogger, "Absorption Coefficients: Liquid=" << absorptionCoefficientLiquid << " Loop=" << absorptionCoefficientLoop << " Crystal=" << absorptionCoefficientCrystal << " Air=" << absorptionCoefficientAir );

        pixelsPerMicromX = iniparser_getdouble(d, "General:pixels_per_microm_x",-1);
        pixelsPerMicromY = iniparser_getdouble(d, "General:pixels_per_microm_y",-1);
        pixelsPerMicromZ = iniparser_getdouble(d, "General:pixels_per_microm_z",-1);

        modelSizeX = iniparser_getint(d, "General:model_size_x",-1);
        modelSizeY = iniparser_getint(d, "General:model_size_y",-1);
        modelSizeZ = iniparser_getint(d, "General:model_size_z",-1);

        worldDimX = iniparser_getint(d, "General:world_dim_x",-1);
        worldDimY = iniparser_getint(d, "General:world_dim_y",-1);
        worldDimZ = iniparser_getint(d, "General:world_dim_z",-1);

        pixelSize = iniparser_getdouble(d, "General:pixel_size",-1);
        distance = iniparser_getdouble(d, "General:distance",-1);
        oscRange = iniparser_getdouble(d, "General:osc_range",-1);
        startAngle = iniparser_getdouble(d, "General:start_angle",-1);
        beamCenterX = iniparser_getdouble(d, "General:beam_center_x",-1);
        beamCenterY = iniparser_getdouble(d, "General:beam_center_y",-1);
        
        totalBatches = iniparser_getint(d, "General:total_batches",-1);
        
        negativeIntensities= iniparser_getint(d, "General:negative_intensities",-1);

        if (negativeIntensities == 0)
        	LOG4CPLUS_INFO(myLogger, "Correcting negative Intensities to 0");
                

//        visibleSet->parseConfigFile(d);
//        visibleSet->imagesToProcess();
//
//        if (iniparser_find_entry(d, (char *) "Laser"))
//        {
//
//            laserSet->parseConfigFile(d);
//            laserSet->imagesToProcess();
//
//        }
        if (iniparser_find_entry(d, (char *) "Volume"))
        {
            volumeSet->parseConfigFile(d);
            volumeSet->imagesToProcess();

        }
        if (iniparser_find_entry(d, (char *) "Loop"))
        {
            loopSet->parseConfigFile(d);
            loopSet->imagesToProcess();

        }
        if (iniparser_find_entry(d, (char *) "Crystal"))
        {

            crystalSet->parseConfigFile(d);
            crystalSet->imagesToProcess();

        }

    }

    return 0;
}
