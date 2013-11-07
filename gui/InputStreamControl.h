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

#ifndef INPUT_STREAM_CONTROL_H
#define INPUT_STREAM_CONTROL_H

#include "Control.h"

class VideoFileReader;

/// Control class for an input
class ControllerStream : public Controller
{
public:
	ControllerStream(VideoFileReader& rx_module);
	~ControllerStream();
	void SetPosition(int x_position);
	inline virtual const std::string& GetName() const {return m_name;}
	void SetControlledValue();
	void GetCurrent();
	void GetDefault();

protected:
	QParameterSlider * m_parameterSlider;
	VideoFileReader  & m_module;
	static const std::string m_name;
};


/// The control board for the module
class InputStreamControl : public ControlBoard
{
public:
	InputStreamControl(const std::string& x_name, const std::string& x_description);
	~InputStreamControl();
	void SetModule(VideoFileReader& rx_module);
private:
	VideoFileReader  * m_module;
};

#endif