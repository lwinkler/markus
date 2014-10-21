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

#ifndef COMPAREVIDEO_H
#define COMPAREVIDEO_H

#include "Module.h"
#include "ParameterNum.h"

/**
* @brief todo
*/
class CompareVideo : public Module {

public:

    class Parameters : public Module::Parameters {

    public:
        Parameters(const ConfigReader &x_confReader) : Module::Parameters(x_confReader) {

            m_list.push_back(new ParameterFloat("threshold", 99.9, 0, 100, &threshold, "threshold"));

            RefParameterByName("type").SetDefault("CV_8UC3");
            RefParameterByName("type").SetRange("[CV_8UC1,CV_8UC3]");

            Init();
        };

        float threshold;
    };

    CompareVideo(const ConfigReader &x_configReader);

    ~CompareVideo();

    void Reset();

    double GetSimilarity(const cv::Mat& A, const cv::Mat& B);

    MKCLASS("CompareVideo")
    MKDESCR("Perform ...") // todo

    inline virtual const Parameters &GetParameters() const {
        return m_param;
    }

    virtual void ProcessFrame();

private:
    inline virtual Parameters &RefParameters() {
        return m_param;
    }

    Parameters m_param;
    static log4cxx::LoggerPtr m_logger;

protected:
    // input
    cv::Mat m_video1;
    cv::Mat m_video2;

    // output
    // (nothing)

    // state variables
    // (nothing)
    double m_sum;
};

#endif
