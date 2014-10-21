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
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

log4cxx::LoggerPtr CompareVideo::m_logger(log4cxx::Logger::getLogger(

"CompareVideo"));

CompareVideo::CompareVideo(const ConfigReader &x_configReader) :
        Module(x_configReader),
        m_param(x_configReader),
        m_video1(Size(m_param.width, m_param.height), m_param.type),
        m_video2(Size(m_param.width, m_param.height), m_param.type) {

    AddInputStream(0, new StreamImage("video1", m_video1, *this, "Video 1"));
    AddInputStream(1, new StreamImage("video2", m_video2, *this, "Video 2"));

    m_sum = 0;
}

CompareVideo::~CompareVideo() {
}

void CompareVideo::Reset() {
    m_sum = 0;
}

/**
* Compare two images by getting the L2 error (square-root of sum of squared error).
*/
double CompareVideo::GetSimilarity(const Mat &A, const Mat &B) {

    /* check images size */
    assert(A.rows > 0 && A.cols > 0 && A.rows == B.rows && A.cols == B.cols);

#define sCOMPARE_A
#ifdef COMPARE_A
        /* Calculate the L2 relative error between images. */
        double errorL2 = norm(A, B, CV_L2);
        /* Convert to a reasonable scale, since L2 error is summed across all pixels of the image. */
        // return errorL2 / (double) (A.rows * A.cols);
        return errorL2;
#else
        double sum = 0;
        for (int i = 0; i < A.cols; i++) {
            for (int j = 0; j < A.rows; j++) {
                const Vec3b INTENSITY_A = A.at<Vec3b>(j, i);
                const Vec3b INTENSITY_B = B.at<Vec3b>(j, i);
                int jkl = 0;
                for(int k = 0; k < A.channels(); k++) {
                    jkl += INTENSITY_A.val[k] == INTENSITY_B.val[k] ? 0 : 1;
                }
                if(jkl != 0)
                    sum++;
            }
        }
        return sum;
#endif
}

void CompareVideo::ProcessFrame() {
    static long frameCount = 0;
    double similarity = GetSimilarity(m_video1, m_video2);

    cout << "frame[" << frameCount++ << "]: similarity = " << similarity << "/" << m_video1.total() << " (" << (similarity / m_video1.total() * 100) << "%)" << endl;
}

