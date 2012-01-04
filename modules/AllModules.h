#ifndef ALL_MODULES_H
#define ALL_MODULES_H

#include "SlitCam/SlitCam.h"
#include "ObjectTracker/ObjectTracker.h"
#include "CascadeDetector/CascadeDetector.h"

#include "UsbCam/UsbCam.h"
#include "VideoFileReader/VideoFileReader.h"

Module * createNewModule(std::string& rx_moduleClass, const std::string& rx_moduleName, const ConfigReader& rx_configReader)
{
	Module * tmp = NULL;
	if(rx_moduleClass.compare("SlitCamera") == 0)
	{
		tmp = new SlitCam(rx_moduleName, rx_configReader);
	}
	else if(rx_moduleClass.compare("ObjectTracker") == 0)
	{
		tmp = new ObjectTracker(rx_moduleName, rx_configReader);
	}
	else if(rx_moduleClass.compare("CascadeDetector") == 0)
	{
		tmp = new CascadeDetector(rx_moduleName, rx_configReader);
	}
	else if(rx_moduleClass.compare("UsbCam") == 0)
	{
		tmp = new UsbCam(rx_moduleName, rx_configReader);
	}
	else if(rx_moduleClass.compare("VideoFileReader") == 0)
	{
		tmp = new VideoFileReader(rx_moduleName, rx_configReader);
	}
	else throw("Module type unknown : " + rx_moduleClass);

	return tmp;
}

#endif