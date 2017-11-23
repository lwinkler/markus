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

#ifndef COMPARE_OBJECTS_H
#define COMPARE_OBJECTS_H

#include "Module.h"
#include "Object.h"

/**
 * @brief Compare the two input videos and compute the dissimilarity of them for all the sequence.
 * When processing, the module throws an exception as soon as the dissimilarity of all already processed frames exceeds the given threshold.
 */
class CompareObjects : public Module
{
public:
	class Parameters : public Module::Parameters
	{

	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterFloat("threshold", 0.1, 0, 1, &threshold, "Dissimilarity threshold for all the sequence"));
		};

		/* Dissimilarity threshold [0;100] */
		float threshold;
	};

	/* Constructor */
	explicit CompareObjects(ParameterStructure& xr_params);

	/* Destructor */
	virtual ~CompareObjects();

	/* Reset current state of this module */
	void Reset() override;

	/* Compare the two images and return the amount of pixels that are different */
	int64_t ComputeDissimilarity(const cv::Mat& A, const cv::Mat& B);

	MKCLASS("CompareObjects");
	MKDESCR("Compare the two input objects streams and compute the dissimilarity of them for all the sequence");
	MKCATEG("Test")

	void ProcessFrame() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	std::vector<Object> m_objects1;
	std::vector<Object> m_objects2;

	// state variables
	// long m_allErrors;
	// long m_frameCount;
};

#endif
