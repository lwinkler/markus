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

#include "BlobSegmenter.h"
#include "StreamDebug.h"
#include "StreamObject.h"
#include "StreamImage.h"

#include "cvblobs/blob.h"
#include "cvblobs/BlobResult.h"

// for debug
#include "util.h"

using namespace cv;

const char * BlobSegmenter::m_type = "BlobSegmenter";


BlobSegmenter::BlobSegmenter(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Segments the binary blobs.";
	m_input = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	m_blobsImg = new Mat(cvSize(m_param.width, m_param.height), CV_8UC3);

	m_inputStreams.push_back(new StreamImage(0, "input", m_input, *this,	"Input binary stream"));

	m_outputStreams.push_back(new StreamObject(0, "segmented", m_param.width, m_param.height, m_regions, cvScalar(255,100,255), *this,	"Segmented objects"));
	
	m_debugStreams.push_back(new StreamDebug(0, "blobs", m_blobsImg, *this,	"Blobs"));
}

BlobSegmenter::~BlobSegmenter(void )
{
	delete(m_input);
	delete(m_blobsImg);
}

void BlobSegmenter::ProcessFrame()
{
	ExtractBlobs(m_input);
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* ExtractBlob */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void BlobSegmenter::ExtractBlobs(Mat* x_img)
{
	// object that will contain blobs of inputImage
	CBlobResult blobs;
	m_blobsImg->setTo(cvScalar(0,0,0));
	m_regions.clear();
	
	// Extract the blobs using a threshold of 100 in the image
	IplImage img = *x_img;
	// blobs = CBlobResult(&img, NULL, 100/*m_foreground_thres* 255*/, true);  // old lib cvBlobs
	blobs = CBlobResult(&img, NULL, 0);/*m_foreground_thres* 255*/ 
	saveMat(x_img, "x_img.bmp");
	// create a file with some of the extracted features
	blobs.PrintBlobs( (char*) "blobs.txt" );
	
	int i;
	CBlob *currentBlob;
	// exclude the ones smaller than value
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_OUTSIDE, 25, x_img->size().area() / 4); // TODO Add param

	for (i = 0; i < blobs.GetNumBlobs(); i++ )
	{
			IplImage img = *m_blobsImg; // TODO : debug only
			currentBlob = blobs.GetBlob(i);
			
			//double posx = currentBlob->SumX();
			//double posy = currentBlob->SumY();
			
			//double posx = currentBlob->GetBoundingBox().x;
			//double posy = currentBlob->GetBoundingBox().y;
			cv::Rect rect = currentBlob->GetBoundingBox();
			
			
			
		//if(currentBlob->Parent() && !currentBlob->Exterior() && posx > 0 && posx < m_blobsImg->cols && posy > 0 && posy < m_blobsImg->rows) // TODO : fix this for real
		if(true)//posx >= 0 && posx < m_blobsImg->cols && posy >= 0 && posy < m_blobsImg->rows) // TODO : fix this for real
		{
			currentBlob->FillBlob( &img, Tracker::m_colorArray[i % Tracker::m_colorArraySize]);
			
			TrackedRegion reg(i);
			reg.m_posX = rect.x;
			reg.m_posY = rect.y;
			
			// Add here all features that are to be added in the templates/region
			if(Feature::m_names.size() == 0)
			{
				// Write names once
				Feature::m_names.push_back("area");
				Feature::m_names.push_back("perimeter");
				Feature::m_names.push_back("position x");
				Feature::m_names.push_back("position y");
			}
			
			reg.AddFeature(currentBlob->Area());
	 		reg.AddFeature(currentBlob->Perimeter());
	
	// 		//reg.AddFeature("mean", GetSTLResult(currentBlob, CBlobGetMean()));
	// 		reg.AddFeature("compactness", GetSTLResult(currentBlob, CBlobGetCompactness()));
	// 		reg.AddFeature("length", GetSTLResult(currentBlob, CBlobGetLength()));
	// 
	// 		reg.AddFeature("convex hull perimeter", GetSTLResult(currentBlob, CBlobGetHullPerimeter()));
	
			reg.AddFeature(rect.x);
			reg.AddFeature(rect.y); // TODO : Add width / 2 ??
			
			//cout<<"Blob extracted position "<<currentBlob->SumX()<<","<<currentBlob->SumY()<<endl;;
			//m_regions.push_back(reg); // TODO : Manage regions better
			/*cv::Rect rect;
			rect.x = posx;
			rect.y = posy;
			rect.width  = currentBlob->MaxX() - currentBlob->MinX();
			rect.height = currentBlob->MaxY() - currentBlob->MinY();*/
			Object obj;
			obj.SetRect(rect);
			m_regions.push_back(obj);
		}
		//else throw("ERROR");
	}
}


double BlobSegmenter::GetSTLResult( CBlob* blob, funcio_calculBlob *evaluador ) const
{
	return (*evaluador)(*blob);
}


