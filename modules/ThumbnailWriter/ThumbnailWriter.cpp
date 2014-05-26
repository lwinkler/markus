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

#include "ThumbnailWriter.h"
#include "StreamImage.h"
#include "util.h"
#include "Manager.h"
#include <opencv2/highgui/highgui.hpp>
#include <fstream>

using namespace std;
using namespace cv;

log4cxx::LoggerPtr ThumbnailWriter::m_logger(log4cxx::Logger::getLogger("ThumbnailWriter"));

ThumbnailWriter::ThumbnailWriter(const ConfigReader& x_configReader): 
	Module(x_configReader),
	m_param(x_configReader),
	m_input(Size(m_param.width, m_param.height), m_param.type)
{
	AddInputStream(0, new StreamImage( "input", m_input, *this,   "Video input"));
	AddInputStream(1, new StreamObject("objects", m_objectsIn, *this,     "Incoming objects"));
}

ThumbnailWriter::~ThumbnailWriter()
{
}

void ThumbnailWriter::Reset()
{
	Module::Reset();
	m_folderName  = Manager::OutputDir() + "/" + m_param.folder + "/"; 
	SYSTEM("mkdir -p " + m_folderName);
}

void ThumbnailWriter::ProcessFrame()
{
	int cpt = 0;
	for(vector<Object>::const_iterator it1 = m_objectsIn.begin() ; it1 != m_objectsIn.end() ; it1++)
	{
		// Save features to .json
		std::stringstream ss2;
		ss2 << m_folderName << m_currentTimeStamp << "_" << it1->GetName()<< it1->GetId() << "_" << cpt << ".json";
		ofstream of(ss2.str().c_str());
		if(!of.is_open())
		{
			SYSTEM("mkdir -p " + m_folderName)
			of.open(ss2.str().c_str());
			if(!of.is_open())
				throw MkException("Impossible to create file " + ss2.str(), LOC);
		}
		LOG_DEBUG(m_logger, "Write object to " << ss2.str());
		it1->Serialize(of, m_folderName);
		of.close();

		// For each object save a thumbnail
		const Rect &rect(it1->Rect());
		std::stringstream ss1;
		ss1 << m_folderName << m_currentTimeStamp << "_" << it1->GetName()<< it1->GetId() << "_" << cpt << "." << m_param.extension;
		imwrite(ss1.str(), (m_input)(rect));

		cpt++;
	}
}


