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
#include "StreamRect.h"
#include "StreamImage.h"

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

	m_outputStreams.push_back(new StreamRect(0, "tracker", m_param.width, m_param.height, m_trackerOutput, cvScalar(255,100,255), *this,	"Segmented objects"));
	
	m_debugStreams.push_back(new StreamDebug(0, "blobs", m_blobsImg, *this,	"Blobs"));
}

BlobSegmenter::~BlobSegmenter(void )
{
	delete(m_input);
	delete(m_blobsImg);
}

void BlobSegmenter::ProcessFrame(const double x_timeSinceLastProcessing)
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
	//cvSet(m_blobsImg, cvScalar(0,0,0));
	m_regions.clear();
	
	// Extract the blobs using a threshold of 100 in the image
	IplImage img = *x_img;
	blobs = CBlobResult(&img, NULL, 100/*m_foreground_thres* 255*/, true); 
	
	// create a file with some of the extracted features
	blobs.PrintBlobs( (char*) "blobs.txt" );
	
	int i;
	CBlob *currentBlob;
	// exclude the ones smaller than value
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_OUTSIDE, 25, x_img->size().area() / 4);

	for (i = 0; i < blobs.GetNumBlobs(); i++ )
	{
			currentBlob = blobs.GetBlob(i);
			
			double posx = currentBlob->SumX();
			double posy = currentBlob->SumY();
		if(currentBlob->Parent() && !currentBlob->Exterior() && posx > 0 /*&& posx < m_blobsImg->cols && posy > 0 && posy < m_blobsImg->rows*/) // TODO : fix this for real
		{
			// currentBlob->FillBlob( m_blobsImg, m_colorArray[i % m_colorArraySize]);
			
			TrackedRegion reg(i);
			reg.m_posX = currentBlob->SumX();
			reg.m_posY = currentBlob->SumY();
			
			// Add here all features that are to be added in the templates/region
			static int do_once = 1;
			if(do_once)
			{
				// Write names once
				Feature::m_names.push_back("area");
				Feature::m_names.push_back("perimeter");
				Feature::m_names.push_back("position x");
				Feature::m_names.push_back("position y");
				do_once = 0;
			}
			
			reg.AddFeature(currentBlob->Area());
	 		reg.AddFeature(currentBlob->Perimeter());
	
	// 		//reg.AddFeature("mean", GetSTLResult(currentBlob, CBlobGetMean()));
	// 		reg.AddFeature("compactness", GetSTLResult(currentBlob, CBlobGetCompactness()));
	// 		reg.AddFeature("length", GetSTLResult(currentBlob, CBlobGetLength()));
	// 
	// 		reg.AddFeature("convex hull perimeter", GetSTLResult(currentBlob, CBlobGetHullPerimeter()));
	
			reg.AddFeature(currentBlob->SumX());
			reg.AddFeature(currentBlob->SumY());
			
			//cout<<"Blob extracted position "<<currentBlob->SumX()<<","<<currentBlob->SumY()<<endl;;
			m_regions.push_back(reg);
		}
		//else throw("ERROR");
	}
}


double BlobSegmenter::GetSTLResult( CBlob* blob, funcio_calculBlob *evaluador ) const
{
	return (*evaluador)(*blob);
}


