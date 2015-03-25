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

#include "ConfigReader.h"
#include "Parameter.h"
#include "Module.h"

/// Class representing an module used for input (camera, video file, network stream, ...)
class Input : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			RefParameterByName("auto_process").SetRange("[1:1]");
			RefParameterByName("auto_process").SetDefault("1"); // Input must be in real time otherwise they are never called
			Init();
		}
	};

	Input(ParameterStructure& x_param);
	~Input();

	virtual void Capture() = 0;
	inline const std::string& GetName()const {return m_name;}
	inline bool   IsEndOfStream() const {return m_endOfStream;}
	inline void ProcessFrame() {Capture();}
	virtual inline bool IsInput() {return true;}

private:
	virtual const Parameters& GetParameters() const = 0;

protected:
	bool m_endOfStream;
};

#endif
