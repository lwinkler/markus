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

#ifndef INPUT_H
#define INPUT_H

#include "Configurable.h"
#include "Parameter.h"
#include "Module.h"

/// Class representing an module used for input (camera, video file, network stream, ...)
class Input : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			RefParameterByName("autoProcess").SetRange("[1:1]");
			RefParameterByName("autoProcess").SetDefaultAndValue(1); // Input must be in real time otherwise they are never called
			RefParameterByName("autoProcess").SetValueToDefault();
		}
	};

	explicit Input(ParameterStructure& x_param);
	virtual ~Input();

	virtual void Capture() = 0;
	inline void ProcessFrame() override {Capture();}
	inline bool IsInput() const override {return true;}

protected:
	bool m_endOfStream;
};

#endif
