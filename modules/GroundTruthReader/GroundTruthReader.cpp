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

#include "GroundTruthReader.h"
#include "StreamImage.h"
#include "StreamState.h"

using namespace std;
using namespace cv;

GroundTruthReader::GroundTruthReader(const ConfigReader& x_configReader): 
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Read a ground truth file";

	m_input       = new Mat(Size(m_param.width, m_param.height), m_param.type);
	m_inputStreams.push_back(new StreamImage(0, "input",  m_input, *this, "Video input"));

	m_outputStreams.push_back(new StreamImage(0, "input", m_input,  *this, 	"Copy of the input stream"));
	m_outputStreams.push_back(new StreamState(1, "state", m_state,  *this, 	"State read from the annotation file"));
}

GroundTruthReader::~GroundTruthReader()
{
	delete(m_input);
	m_srtFile.close();
}

void GroundTruthReader::Reset()
{
	Module::Reset();
	m_state    = false;
	m_stateSub = false;
	m_srtStart = "";
	m_srtEnd   = "";

	LOG4CXX_DEBUG(m_logger, "Open ground truth file: "<<m_param.file);
	// if(m_param.file.size() == 0)
		// No file
		// return;

	m_srtFile.open(m_param.file.c_str(), std::ifstream::in);
	
	if(! m_srtFile.is_open())
	{
		throw MkException("Error : GroundTruthReader cannot open file : " + m_param.file, LOC);
	}
}

void GroundTruthReader::ProcessFrame()
{
	if(! m_srtFile.good())
		return;
	
	// istringstream ss;
	m_state = 0;
	string current = msToTimeStamp(m_currentTimeStamp);

	if(current > m_srtEnd)
	{
		string line, tmp;
		int num = -1;
		try
		{
			// Read next subtitle
			// 2
			// 00:00:30,700 --> 00:00:31,900
			// state_0

			while(line.size() == 0)
			{
				m_srtFile >> line;
				if(! m_srtFile.good())
					throw MkException("End of file in GroundTruthReader", LOC);
			}
			num = atoi(line.c_str());
			if(num != m_num + 1)
				LOG4CXX_WARN(m_logger, "Missing number in subtitle file "<<(m_num + 1));
			m_num = num;
			//getline(m_srtFile, line);
			//istringstream ss(line);
			//ss >> m_srtStart >> tmp >> m_srtEnd;
			m_srtFile >> m_srtStart;
			m_srtFile >> tmp;
			assert(tmp == "-->");
			m_srtFile >> m_srtEnd;

			getline(m_srtFile, line); // end of line: must be empty
			assert(line.size() == 0);
			getline(m_srtFile, line);

			while(line.size() != 0)
			{
				tmp += line + " ";
				getline(m_srtFile, line);
				if(! m_srtFile.good())
					throw MkException("End of file in GroundTruthReader", LOC);
			}
			m_stateSub = tmp.find(m_param.pattern) != string::npos;
		}
		catch(MkException& e)
		{
			LOG4CXX_WARN(m_logger, "Exception while reading .srt file in GroundTruthReader " << e.what()); 
			throw;
		}
	}


	// Interpret the current state
	if(current >= m_srtStart && current <= m_srtEnd)
	{
		m_state = m_stateSub;
	}
	else
	{
		m_state = 0;
	}
}


