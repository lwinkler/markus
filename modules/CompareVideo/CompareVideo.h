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
 * @brief Compare the two input videos and compute the dissimilarity of them for all the sequence.
 * When processing, the module throws an exception as soon as the dissimilarity of all already processed frames exceeds the given threshold.
 */
class CompareVideo : public Module {

	public:

		class Parameters : public Module::Parameters {

			public:
				Parameters(const ConfigReader &x_confReader) : Module::Parameters(x_confReader) {

					m_list.push_back(new ParameterFloat("threshold", 0.1, 0, 100, &threshold, "Dissimilarity threshold for all the sequence"));

					RefParameterByName("type").SetDefault("CV_8UC3");
					RefParameterByName("type").SetRange("[CV_8UC1,CV_8UC3]");

					Init();
				};

				/* Dissimilarity threshold [0;100] */
				float threshold;
		};

		/* Constructor */
		CompareVideo(const ConfigReader &x_configReader);

		/* Destructor */
		~CompareVideo();

		/* Reset current state of this module */
		void Reset();

		/* Compare the two images and return the amount of pixels that are different */
		long ComputeDissimilarity(const cv::Mat& A, const cv::Mat& B);

		MKCLASS("CompareVideo");
		MKDESCR("Compare the two input videos and compute the dissimilarity of them for all the sequence");

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

		// state variables
		long m_allErrors;
		long m_frameCount;
};

#endif
