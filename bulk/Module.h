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
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  <See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#ifndef MODULE_H
#define MODULE_H

#include "cv.h"
#include "ConfigReader.h"
#include "Parameter.h"
#include <QReadWriteLock>

class Stream;

class ModuleParameterStructure : public ParameterStructure
{
public:
	ModuleParameterStructure(const ConfigReader& x_confReader) : ParameterStructure(x_confReader, "module")
	{
		m_list.push_back(new ParameterT<std::string>(0, "class", 		"", 	PARAM_STR, 		&objClass,	"Class of the module (define the module's function)"));
		m_list.push_back(new ParameterT<int>(0, "inputWidth", 		640, 	PARAM_INT, 	0, 	4000,	&width,		"Width of the input"));
		m_list.push_back(new ParameterT<int>(0, "inputHeight", 	480, 	PARAM_INT, 	0, 	3000,		&height,	"Height of the input"));
		m_list.push_back(new ParameterImageType(0, "inputType", 	CV_8UC1, 				&type,		"Format of the input image"));
	//	m_list.push_back(new ParameterT<int>(0, "channels", 	3, 	PARAM_INT, 	1, 	3,		&channels));
		m_list.push_back(new ParameterT<double>(0, "fps", 	10, 	PARAM_DOUBLE, 	0, 	100000,		&fps,		"Frames per seconds (processing speed)")); //TODO : Check that min max works
	};

public:
	int width; // TODO : rename inputWidth
	int height;
	int type;
	// int channels;
	double fps;
	std::string objClass;
};

class Module : Configurable
{
public:
	Module(const ConfigReader& x_confReader);
	virtual ~Module();
	
	void ReadAndConvertInput(/*const cv::Mat * x_img*/);
	virtual void ProcessFrame(const double x_timeSinceLastProcessing) = 0;
	const std::string& GetName(){return m_name;};
	const std::string& GetDescription(){return m_description;};
	int GetId() const {return m_id;};
	//const cv::Mat * GetOutput(){return m_output;}
	//void AddStream(const std::string& x_name, StreamType x_type, Mat* m_image);
	const std::vector<Stream*>& GetStreamList() const {return m_outputStreams;};
	
	virtual int GetInputWidth() const = 0;// {return GetRefParameter().width;}
	virtual int GetInputHeight() const = 0;// {return GetRefParameter().height;}
	inline int GetInputType() const {return GetRefParameter().type;};
	//virtual inline int GetNbChannels() const {return GetRefParameter().channels;}
	inline int GetFps() const {return GetRefParameter().fps;};
	
	virtual inline bool IsInput() {return false;};
	void Export(std::ostream& rx_os, int x_indentation);
	Stream * GetInputStreamById(int x_id) const;
	Stream * GetOutputStreamById(int x_id) const;
	QReadWriteLock m_lock;
	
	
protected:
	std::vector<Stream *> m_inputStreams;
	std::vector<Stream *> m_outputStreams;
	std::vector<Stream *> m_debugStreams;	
	//cv::Mat * m_input;
	//cv::Mat * m_output;
	//cv::Mat * m_img_tmp1; // To convert the input
	//cv::Mat * m_img_tmp2;
	std::string m_name;
	std::string m_description; 
	int m_id;
	virtual const ModuleParameterStructure & GetRefParameter() const = 0;
	
};

#endif