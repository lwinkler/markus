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

#ifndef SEGMENTER_BLOB_H
#define SEGMENTER_BLOB_H

#include "Module.h"
#include "StreamObject.h"

#include "cvblobs/BlobResult.h"
//#include "cvblobs/BlobExtraction.h"
//#include "cvblobs/Blob.h"
#include "cvblobs/BlobLibraryConfiguration.h"
#include "cvblobs/BlobResult.h"

class ConfigReader;

class SegmenterBlobParameterStructure : public ModuleParameterStructure
{
public:
	SegmenterBlobParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterString("objectLabel", "object", 		   &objectLabel,"Label to be applied to the objects detected by the cascade filter (e.g. face)"));
		m_list.push_back(new ParameterInt("minWidth",  0, 	 PARAM_INT, 0, MAX_WIDTH,  &minWidth,	"Minimal width of an object to segment."));
		m_list.push_back(new ParameterInt("minHeight", 0, 	 PARAM_INT, 0, MAX_HEIGHT, &minHeight,	"Minimal height of an object to segment."));
		
		ParameterStructure::Init();
	};
	
	std::string objectLabel;
	int minWidth;
	int minHeight;
};

class SegmenterBlob : public Module
{
public:
	SegmenterBlob(const ConfigReader& x_configReader);
	~SegmenterBlob();
	
	virtual void ProcessFrame();
	void Reset();
	
protected:
	void ExtractBlobs(cv::Mat* x_img);
	
	// for streams
	cv::Mat * m_input;
#ifdef MARKUS_DEBUG_STREAMS
	cv::Mat* m_blobsImg;
#endif

	static const char * m_type;
	//std::vector <TrackedRegion> m_regions;		
	std::vector<Object> m_regions;
	double GetSTLResult( CBlob* blob, funcio_calculBlob *evaluador ) const;

private:
	SegmenterBlobParameterStructure m_param;
	inline virtual ModuleParameterStructure& RefParameter() { return m_param;};
};


#endif
