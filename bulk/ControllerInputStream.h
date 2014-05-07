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

#include "Controller.h"

class VideoFileReader;
class QParameterSlider;

/**
* @brief Control class for an input of type VideoFileReader
*/
class ControllerInputStream : public Controller
{
public:
	ControllerInputStream(VideoFileReader& rx_module);
	~ControllerInputStream();
	MKCLASS("ControllerInputStream")

	virtual QWidget* CreateWidget();

	typedef void (ControllerInputStream::*action)(std::string*);
	DECLARE_CALL_ACTION(action);
	DECLARE_LIST_ACTION(action);

	// Controllers
	void SetCursor(std::string* xp_value);
	void GetCursor(std::string* xp_value);

protected:
	std::map<std::string, const action> m_actions;
	QParameterSlider * parameterSlider;
	VideoFileReader  & module;
};
#endif
