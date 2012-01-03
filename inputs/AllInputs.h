#ifndef ALL_INPUTS_H
#define ALL_INPUTS_H

#include "UsbCam/UsbCam.h"
#include "VideoFileReader/VideoFileReader.h"


Input * createNewInput(const std::string& rx_inputClass, const std::string& rx_inputName, const ConfigReader& rx_configReader)
{
	Input * tmp = NULL;
	if(rx_inputClass.compare("UsbCam") == 0)
	{
		tmp = new UsbCam(rx_inputName, rx_configReader);
	}
	else if(rx_inputClass.compare("VideoFileReader") == 0)
	{
		tmp = new VideoFileReader(rx_inputName, rx_configReader);
	}
	else throw("Input type unknown : " + rx_inputClass);
	return tmp;
}
#endif