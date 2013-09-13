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

#include "SegmenterBlob.h"
#include "StreamDebug.h"
#include "StreamObject.h"
#include "StreamImage.h"

#include "cvblobs/blob.h"
#include "cvblobs/BlobResult.h"

// for debug
#include "util.h"

using namespace cv;

const char * SegmenterBlob::m_type = "SegmenterBlob";


SegmenterBlob::SegmenterBlob(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Segments a binary image and outputs a stream of objects (from segmented blobs) with their features (position, width and height)";
	m_input = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	m_blobsImg = new Mat(cvSize(m_param.width, m_param.height), CV_8UC3);

	m_inputStreams.push_back(new StreamImage(0, "input", m_input, *this,	"Input binary stream"));

	m_outputStreams.push_back(new StreamObject(0, "segmented", m_param.width, m_param.height, m_regions, cvScalar(255, 255, 255), *this,	"Segmented objects"));
	// StreamObject* tmp = dynamic_cast<StreamObject*>(m_outputStreams[0]);
	
	m_debugStreams.push_back(new StreamDebug(0, "blobs", m_blobsImg, *this,	"Blobs"));
}

SegmenterBlob::~SegmenterBlob(void )
{
	delete(m_input);
	delete(m_blobsImg);
}

void SegmenterBlob::Reset()
{
	Module::Reset();
}

void SegmenterBlob::ProcessFrame()
{
	ExtractBlobs(m_input);
	//dynamic_cast<StreamObject*>(m_outputStreams[0])->PrintObjects();
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* ExtractBlob */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void SegmenterBlob::ExtractBlobs(Mat* x_img)
{
	// object that will contain blobs of inputImage
	CBlobResult blobs;
	m_blobsImg->setTo(0);
	m_regions.clear();
	
	// Extract the blobs using a threshold of 100 in the image
	IplImage img1 = *x_img;
	// blobs = CBlobResult(&img, NULL, 100/*m_foreground_thres* 255*/, true);  // old lib cvBlobs
	blobs = CBlobResult(&img1, NULL, 0);/*m_foreground_thres* 255*/ 
	//saveMat(x_img, "x_img.bmp");
	// create a file with some of the extracted features
	blobs.PrintBlobs( (char*) "blobs.txt" );
	
	int i;
	CBlob *currentBlob;
	
	// exclude the ones smaller than value
	//blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_OUTSIDE, m_param.minSize, m_param.maxSize);

	for (i = 0; i < blobs.GetNumBlobs(); i++ )
	{
		IplImage img = *m_blobsImg; // TODO : debug only
		currentBlob = blobs.GetBlob(i);
		
		cv::Rect rect = currentBlob->GetBoundingBox();
			
		if(rect.width >= m_param.minWidth && rect.height >= m_param.minHeight)
		{
			currentBlob->FillBlob( &img, colorFromId(i));
			
			Object obj(m_param.objectLabel);
			obj.m_posX 	= rect.x + rect.width / 2;
			obj.m_posY 	= rect.y + rect.height / 2;
			obj.m_width	= rect.width;
			obj.m_height 	= rect.height;
			
			// Add here all features that are to be added in the templates/region
			// obj.AddFeature(currentBlob->Area(), 0.1); // Note: it seems that the tracker algo does not work good with these 2 features
	 		// obj.AddFeature(currentBlob->Perimeter(), 0.1);
	
	// 		//reg.AddFeature("mean", GetSTLResult(currentBlob, CBlobGetMean()));
	// 		reg.AddFeature("compactness", GetSTLResult(currentBlob, CBlobGetCompactness()));
	// 		reg.AddFeature("length", GetSTLResult(currentBlob, CBlobGetLength()));
	// 
	// 		reg.AddFeature("convex hull perimeter", GetSTLResult(currentBlob, CBlobGetHullPerimeter()));
	
			obj.AddFeature("x", rect.x);
			obj.AddFeature("y", rect.y);
			obj.AddFeature("width", rect.width);
			obj.AddFeature("height", rect.height);
			m_regions.push_back(obj);
		}
		//else throw("ERROR");
	}
}


double SegmenterBlob::GetSTLResult( CBlob* blob, funcio_calculBlob *evaluador ) const
{
	return (*evaluador)(*blob);
}


