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
#include <fstream>
#include "StreamImage.h"
#include "util.h"
#include "Manager.h"

namespace mk {
using namespace std;
using namespace cv;

log4cxx::LoggerPtr ThumbnailWriter::m_logger(log4cxx::Logger::getLogger("ThumbnailWriter"));

ThumbnailWriter::ThumbnailWriter(ParameterStructure& xr_params):
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_input(Size(m_param.width, m_param.height), m_param.type),
	m_input2(Size(m_param.width, m_param.height), m_param.type)
{
	AddInputStream(0, new StreamImage( "image", m_input, *this,   "Video input"));
	AddInputStream(1, new StreamObject("objects", m_objectsIn, *this,     "Incoming objects", "{\"width\":{\"min\":1},\"height\":{\"min\":1}}"_json));
	AddInputStream(2, new StreamImage( "image2", m_input2, *this,   "Binary mask"));
}

ThumbnailWriter::~ThumbnailWriter()
{
}


bool replaceExpr(string& rx_name, const map<string,FeaturePtr>& x_features)
{
	auto beg = std::find(rx_name.begin(), rx_name.end(), '$');
	if(beg >= rx_name.end() - 1 || *(beg + 1) != '{')
		return false;
	auto end = std::find(beg, rx_name.end(), '}');
	if(end >= rx_name.end())
		return false;
	string pattern(beg + 2, end);

	auto it = x_features.find(pattern);
	if(it == x_features.end())
	{
		rx_name.replace(beg, end + 1, "unknown");
	}
	else
	{
		mkjson json;
		(*it->second).Serialize(json);
		rx_name.replace(beg, end + 1, oneLine(json));
	}

	return true;
}


void ThumbnailWriter::Reset()
{
	Module::Reset();
}

void ThumbnailWriter::ProcessFrame()
{
	int cpt = 0;
	for(auto & elem : m_objectsIn)
	{
		// folder name
		string folderName = m_param.folder;
		while(replaceExpr(folderName, elem.GetFeatures()))
			;


		// Save features to .json
		std::stringstream ss2;
		ss2 << m_currentTimeStamp << "_" << elem.GetName()<< elem.GetId() << "_" << cpt << ".json";
		MkDirectory dir(folderName, RefContext().RefOutputDir(), RefContext().RefOutputDir().DirExists(folderName));

		string fileName = dir.ReserveFile(ss2.str());
		ofstream of(fileName);
		if(!of.is_open())
		{
			throw MkException("Impossible to create file " + ss2.str(), LOC);
		}
		LOG_DEBUG(m_logger, "Write object to " << ss2.str());
		mkjson json(elem);
		of << multiLine(json);
		of.close();


		// For each object save a thumbnail
		elem.Intersect(m_input);
		const Rect rect(elem.GetRect());
		/*if(rect.x < 0 || rect.y < 0 || rect.x + rect.width >= GetWidth() || rect.y + rect.height >= GetHeight())
		{
			LOG_WARN(m_logger, "Object is larger than the base image");
			rect = it1->Rect();
			continue;
		}*/
		std::stringstream ss1;
		ss1 << m_currentTimeStamp << "_" << elem.GetName()<< elem.GetId() << "_" << cpt << "." << m_param.extension;
		imwrite(dir.ReserveFile(ss1.str()), (m_input)(rect));

		// For each object save a thumbnail
		if(m_inputStreams.at("image2")->IsConnected())
		{
			std::stringstream ss3;
			ss3 << m_currentTimeStamp << "_" << elem.GetName()<< elem.GetId() << "_" << cpt << "_mask." << m_param.extension;
			imwrite(dir.ReserveFile(ss3.str()), (m_input2)(rect));
		}

		cpt++;
	}
}


} // namespace mk
