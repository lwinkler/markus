/*----------------------------------------------------------------------------------
*
*    MARKUS : a manager for video analysis modules
* 
*    author : Loïc Monney <loic.monney@hefr.ch>
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
#include "CompareVideo.h"

#include "StreamImage.h"
#include "StreamDebug.h"
#include "MkException.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

log4cxx::LoggerPtr CompareVideo::m_logger(log4cxx::Logger::getLogger("CompareVideo"));

CompareVideo::CompareVideo(const ConfigReader &x_configReader) :
	Module(x_configReader),
	m_param(x_configReader),
	m_video1(Size(m_param.width, m_param.height), m_param.type),
	m_video2(Size(m_param.width, m_param.height), m_param.type) {

		AddInputStream(0, new StreamImage("video1", m_video1, *this, "Video 1"));
		AddInputStream(1, new StreamImage("video2", m_video2, *this, "Video 2"));
	}

CompareVideo::~CompareVideo() {
}

void CompareVideo::Reset() {
	Module::Reset();
	m_frameCount = 0;
	m_allErrors = 0;
}

long CompareVideo::ComputeDissimilarity(const Mat &A, const Mat &B) {

	/* Check size of images */
	assert(A.rows > 0 && A.cols > 0 && A.rows == B.rows && A.cols == B.cols);

	/* Numbers of pixels that are different */
	double errors = 0;

	/* Compare pixel-by-pixel if they are the same */
	for (int i = 0; i < A.cols; i++) {
		for (int j = 0; j < A.rows; j++) {

			/* RGS channels */
			const Vec3b INTENSITY_A = A.at<Vec3b>(j, i);
			const Vec3b INTENSITY_B = B.at<Vec3b>(j, i);

			/* Compare the channels of both pixels */
			int badChannels = 0;
			for (int k = 0; k < A.channels(); k++) {
				badChannels += INTENSITY_A.val[k] == INTENSITY_B.val[k] ? 0 : 1;
			}

			/* If one or more channels are different, these two pixels are not the same */
			if (badChannels > 0) {
				errors++;
			}
		}
	}

	return errors;
}

void CompareVideo::ProcessFrame() {

	/* Compute similarity/dissimilarity */
	long dissimilarity = ComputeDissimilarity(m_video1, m_video2);
	long pixels = m_video1.total();
	long similarity = pixels - dissimilarity;
	m_allErrors += dissimilarity;
	m_frameCount++;

	/* Log */
	cout << "frame[" << m_frameCount << "]: ";
	cout << "\tsimilarity = " << similarity << "/" << pixels;
	cout << "\tdissimilarity = " << dissimilarity << "/" << pixels;
	cout << "\t(error: " << (dissimilarity / (double) pixels * 100) << "%)" << endl;

	/* Exception when exceeds threshold */
	double e = m_allErrors / (double)(pixels * m_frameCount) * 100;
	if (e > m_param.threshold) {
		stringstream t;
		t << e << " > " << m_param.threshold << "!";
		throw MkException(t.str(), LOC);
	}
}


