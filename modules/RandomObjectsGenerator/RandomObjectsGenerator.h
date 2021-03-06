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

#ifndef RANDOM_OBJECTS_GENERATOR_H
#define RANDOM_OBJECTS_GENERATOR_H

#include "Input.h"
#include "Object.h"

namespace mk {
/**
* @brief Generate an object with random features at each step
*/
class RandomObjectsGenerator : public Input
{
public:
	class Parameters : public Input::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Input::Parameters(x_name)
		{
			AddParameter(new ParameterInt("nbObjects"  , 10  , 0 , 1000    , &nbObjects  , "Number of objects to generate per step"));
			AddParameter(new ParameterInt("nbFeatures" , 4   , 0 , 1000    , &nbFeatures , "Number of features per event"));
			AddParameter(new ParameterInt("randomSeed" , 0   , 0 , INT_MAX , &randomSeed , "Seed for random generator: 0 means seed is generated from timer"));
			AddParameter(new ParameterDouble("speed"    , .005, 0 , DBL_MAX , &speed ,      "Speed for the variation of object features"));
			RefParameterByName("fps").SetDefaultAndValue(5);
		}

	public:
		int nbObjects;
		int nbFeatures;
		int randomSeed;
		double speed;
	};

	explicit RandomObjectsGenerator(ParameterStructure& xr_params);
	~RandomObjectsGenerator() override;
	MKCLASS("RandomObjectsGenerator")
	MKCATEG("Input")
	MKDESCR("Generate an object with varying features at each step")

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void Capture() override;
	void Reset() override;

	// output
	std::vector<Object> m_objects;

	// state
	unsigned int m_seed;
	int m_cpt;
};

} // namespace mk
#endif
