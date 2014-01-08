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

using namespace std;
using namespace cv;


ThumbnailWriter::ThumbnailWriter(const ConfigReader& x_configReader): 
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Write an object stream as separate images in a folder.";

	m_input    = new Mat(Size(m_param.width, m_param.height), m_param.type);

	m_inputStreams.push_back(new StreamImage(0, "input", m_input, *this,   "Video input"));
	m_inputStreams.push_back(new StreamObject(1, "objects", m_objectsIn, cvScalar(255, 255, 255), *this,     "Incoming objects"));
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
	// For each object save a thumbnail
	int cpt = 0;
	for(vector<Object>::const_iterator it1 = m_objectsIn.begin() ; it1 != m_objectsIn.end() ; it1++)
	{
		const Rect &rect(it1->Rect());
		std::stringstream ss;
		ss << m_folderName << m_currentTimeStamp << "_" << it1->GetName()<< it1->GetId() << "_" << cpt << "." << m_param.extension;
		imwrite(ss.str(), (*m_input)(rect));
		cpt++;
	}
}


