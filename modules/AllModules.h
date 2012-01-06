#ifndef ALL_MODULES_H
#define ALL_MODULES_H

#include "SlitCam/SlitCam.h"
#include "ObjectTracker/ObjectTracker.h"
#include "CascadeDetector/CascadeDetector.h"

#include "UsbCam/UsbCam.h"
#include "VideoFileReader/VideoFileReader.h"

Module * createNewModule(const ConfigReader& rx_configReader)
{
	// TODO : class parameter useless
	Module * tmp = NULL;
	vector<ParameterValue> paramList = rx_configReader.SubConfig("parameters").ReadParameters("param");
	const string moduleClass = ConfigReader::GetParameterValue("class", paramList).m_value;

	// const string moduleClass = rx_configReader.GetAttribute("class"); // TODO this is better
	
	if(moduleClass.compare("SlitCamera") == 0)
	{
		tmp = new SlitCam(rx_configReader);
	}
	else if(moduleClass.compare("ObjectTracker") == 0)
	{
		tmp = new ObjectTracker(rx_configReader);
	}
	else if(moduleClass.compare("CascadeDetector") == 0)
	{
		tmp = new CascadeDetector(rx_configReader);
	}
	else if(moduleClass.compare("UsbCam") == 0)
	{
		tmp = new UsbCam(rx_configReader);
	}
	else if(moduleClass.compare("VideoFileReader") == 0)
	{
		tmp = new VideoFileReader(rx_configReader);
	}
	else throw("Module type unknown : " + moduleClass);

	return tmp;
}

#endif