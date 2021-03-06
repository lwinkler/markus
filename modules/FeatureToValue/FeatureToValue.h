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


namespace mk {
class Object;
/**
* @brief Output video stream with additional object streams
*/


class FeatureToValue : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterString("feature", "height",  &feature, "The feature to convert to a value stream"));
		};
		std::string feature;
	};

	explicit FeatureToValue(ParameterStructure& xr_params);
	~FeatureToValue() override;
	MKCLASS("FeatureToValue")
	MKCATEG("Conversion")
	MKDESCR("Convert an object feature to a value stream (if several objects are present, use the largest)")

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void Reset() override;
	void ProcessFrame() override;

	// input
	std::vector<Object> m_objectInput;

	// output
	double m_value;

	// temp
	StreamNum<double>* mp_streamValues;
};


} // namespace mk
#endif
