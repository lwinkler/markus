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

#ifndef BLOBSEGMENTER_H
#define BLOBSEGMENTER_H

#include "Module.h"
#include "StreamObject.h"

#include "cvblobs/BlobResult.h"
//#include "cvblobs/BlobExtraction.h"
//#include "cvblobs/Blob.h"
#include "cvblobs/BlobLibraryConfiguration.h"
#include "cvblobs/BlobResult.h"

#include "Tracker.h" // TODO : should not be here

class ConfigReader;

class BlobSegmenterParameterStructure : public ModuleParameterStructure
{
public:
	BlobSegmenterParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterT<std::string>(0, "objectLabel", "object", PARAM_STR, &objectLabel,	"Label to be applied to the objects detected by the cascade filter (e.g. face)"));
		m_list.push_back(new ParameterT<int>(0, "minWidth",  0, 	 PARAM_INT, 0, MAX_WIDTH, &minWidth,	"Minimal width of an object to segment."));
		m_list.push_back(new ParameterT<int>(0, "minHeight", 0, 	 PARAM_INT, 0, MAX_HEIGHT, &minHeight,	"Minimal height of an object to segment."));
		
		ParameterStructure::Init();
	};
	
	std::string objectLabel;
	int minWidth;
	int minHeight;
};

class BlobSegmenter : public Module
{
public:
	BlobSegmenter(const ConfigReader& x_configReader);
	~BlobSegmenter();
	
	virtual void ProcessFrame();
	inline virtual int GetInputWidth() const {return m_param.width;};
	inline virtual int GetInputHeight() const {return m_param.height;};
	
protected:
	void ExtractBlobs(cv::Mat* x_img);
	
	// for streams
	cv::Mat * m_input;
	cv::Mat* m_blobsImg;

	static const char * m_type;
	//std::vector <TrackedRegion> m_regions;		
	std::vector<Object> m_regions;
	double GetSTLResult( CBlob* blob, funcio_calculBlob *evaluador ) const;

private:
	BlobSegmenterParameterStructure m_param;
	inline virtual const ModuleParameterStructure& GetRefParameter() const { return m_param;};
};


#endif