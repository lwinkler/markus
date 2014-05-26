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

#ifndef INPUT_GROUNDTRUTH_READER_H
#define INPUT_GROUNDTRUTH_READER_H

#include "Module.h"
#include "AnnotationFileReader.h"


/**
* @brief Read a ground truth file
*/
class GroundTruthReader : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : 
		Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterString("file", 	"in/input.srt", &file,    "Name of the video file to read, with path"));
			m_list.push_back(new ParameterString("pattern",	"state_1",      &pattern, "Pattern to search in text. If this is found the state is set to 1"));
			Init();
		};

	public:
		std::string file;
		std::string pattern;
	};

	GroundTruthReader(const ConfigReader& x_confReader);
	~GroundTruthReader();
	MKCLASS("GroundTruthReader")
	MKDESCR("Read a ground truth file")
	
	void ProcessFrame();
	void Reset();
private:
	inline virtual const Parameters& GetParameters() const {return m_param;}
	Parameters m_param;
	static log4cxx::LoggerPtr m_logger;

protected:

	// input (and output)
	cv::Mat m_input;

	// ouput
	bool m_state;

#ifdef MARKUS_DEBUG_STREAMS
	// debug
	cv::Mat m_debug;
#endif

	// temp
	AnnotationFileReader* mp_annotationReader;
};

#endif
