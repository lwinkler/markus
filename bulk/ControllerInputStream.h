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

namespace mk {
class VideoFileReader;
class QParameterSlider;

/**
* @brief Control class for an input of type VideoFileReader
*/
class ControllerInputStream : public Controller
{
public:
	explicit ControllerInputStream(mk::VideoFileReader& rx_module);
	~ControllerInputStream() override;
	MKCLASS("ControllerInputStream")

	QWidget* CreateWidget() override;

	typedef void (ControllerInputStream::*action)(mkjson&);
	DECLARE_CALL_ACTION(action);
	DECLARE_LIST_ACTION(action);

	// Controllers
	void SetCursor(mkjson& rx_value);
	void GetCursor(mkjson& rx_value);

protected:
	std::map<std::string, const action> m_actions;
	// cppcheck-suppress unsafeClassCanLeak // we must return this pointer to Qt
	QParameterSlider * m_parameterSlider;
	mk::VideoFileReader  & m_module;
};
} // namespace mk
#endif
