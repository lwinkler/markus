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
#include "StreamObject.h"
#include "AnnotationAssFileReader.h"
#include "AnnotationSrtFileReader.h"
#include "Event.h"



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
			m_list.push_back(new ParameterString("pattern",	"state_1",      &pattern, "Pattern to search in text. If this is found the state is equal to the text"));
			m_list.push_back(new ParameterDouble("distance", 0.1, 0, 1,		&distance, "distance between real object and ROI in ass file in pourcentage of image diagonal"));
			Init();
		};

	public:
		std::string file;
		std::string pattern;
		double distance;
	};

	GroundTruthReader(ParameterStructure& x_confReader);
	~GroundTruthReader();
	MKCLASS("GroundTruthReader")
	MKDESCR("Read a ground truth file")

	inline virtual const Parameters& GetParameters() const {return m_param;}
	void ProcessFrame();
	void Reset();

private:
	Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
	bool m_oldState;
	bool m_assFile;
	std::vector<int> trackedObj;
	int distanceRefObject;

protected:

	// input (and output)
	cv::Mat m_input;
	std::vector <Object> m_objects;

	// ouput
	bool m_state;
	Event m_event;

#ifdef MARKUS_DEBUG_STREAMS
	// debug
	cv::Mat m_debug;
#endif

	// temp
	AnnotationFileReader* mp_annotationReader;
};

#endif
