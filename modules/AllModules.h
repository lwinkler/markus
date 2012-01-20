#ifndef ALL_MODULES_H
#define ALL_MODULES_H

#include "SlitCam/SlitCam.h"
#include "BackgroundExtractor/BackgroundExtractor.h"
#include "CascadeDetector/CascadeDetector.h"
#include "SimpleTracker/SimpleTracker.h"
#include "BlobSegmenter/BlobSegmenter.h"

#include "UsbCam/UsbCam.h"
#include "VideoFileReader/VideoFileReader.h"

#include "VideoOutput/VideoOutput.h"

Module * createNewModule(const ConfigReader& rx_configReader)
{
	Module * tmp = NULL;

	const string moduleClass = rx_configReader.SubConfig("parameters").SubConfig("param", "class").GetValue();
	
	if(moduleClass.compare("SlitCamera") == 0)
	{
		tmp = new SlitCam(rx_configReader);
	}
	else if(moduleClass.compare("BackgroundExtractor") == 0)
	{
		tmp = new BackgroundExtractor(rx_configReader);
	}
	else if(moduleClass.compare("BlobSegmenter") == 0)
	{
		tmp = new BlobSegmenter(rx_configReader);
	}
	else if(moduleClass.compare("SimpleTracker") == 0)
	{
		tmp = new SimpleTracker(rx_configReader);
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
	else if(moduleClass.compare("VideoOutput") == 0)
	{
		tmp = new VideoOutput(rx_configReader);
	}
	else throw("Module type unknown : " + moduleClass);

	return tmp;
}

#endif