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

#ifndef FEATURE_TO_VALUE_H
#define FEATURE_TO_VALUE_H

#include "Module.h"
#include "StreamNum.h"

/**
* @brief Output video stream with additional object streams
*/

class Object;

class FeatureToValue : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			AddParameter(new ParameterString("feature", "height",  &feature, "The feature to convert to a value stream"));

			Init();
		};
		std::string feature;
	};

	FeatureToValue(ParameterStructure& xr_params);
	virtual ~FeatureToValue();
	MKCLASS("FeatureToValue")
	MKCATEG("Conversion")
	MKDESCR("Convert an object feature to a value stream (if several objects are present, use the largest)")

	virtual void Reset() override;
	virtual void ProcessFrame() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	std::vector<Object> m_objectInput;

	// output
	double m_value;

	// temp
	StreamNum<double>* mp_streamValues;
};


#endif
