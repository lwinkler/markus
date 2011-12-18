/*----------------------------------------------------------------------------------
*
*    MARKUS : a manager for video analysis modules
* 
*    author : Laurent Winkler <lwinkler888@gmail.com>
* 
* 
*    This file is part of Markus.
*
*    Markus is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Markus is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include "cv.h"
#include "ConfigReader.h"
#include "Parameter.h"
//#include "Stream.h"
#include "Input.h"
#include "Module.h"

//class Module;
//class Input;

class ImageProcessorParameterStructure : public ParameterStructure
{
public:
	ImageProcessorParameterStructure(ConfigReader& x_confReader, int x_nb) : ParameterStructure(x_confReader, "ImageProcessors", "ImageProcessor", x_nb)
	{
		m_list.push_back(new ParameterT<std::string>(0, "input",  "(definp)",	PARAM_STR,	&input));
		m_list.push_back(new ParameterT<std::string>(0, "module", "(defmod)",	PARAM_STR,	&module));
	};

public:
	std::string input;
	std::string module;
};

class ImageProcessor : Configurable
{
public:
	ImageProcessor(const std::string& x_name, int x_nb, ConfigReader& x_confReader, std::vector<Input*>& x_inputList);
	~ImageProcessor();
	
	void Process(double x_timeSinceLast);
	const int & GetNumber(){return m_nb;};
	
	Input & GetInput() {return *m_input;}
	Module & GetModule() {return *m_module;}
	
	inline int GetFps() const {return m_module->GetFps();}

	
protected:
	//const std::string m_name;
	const int m_nb;
	double m_timeSinceLastProcessing;
	double m_timeInterval;
	Module * m_module;
	Input * m_input;

	IplImage * m_img_tmp1;
	IplImage * m_img_tmp2;
	IplImage * m_img_input;

	ImageProcessorParameterStructure m_param;
	inline virtual const ParameterStructure& GetRefParameter() const {return m_param;};
};

#endif