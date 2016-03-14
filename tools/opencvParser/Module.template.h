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

#ifndef _MARKUS_CV_$moduleName
#define _MARKUS_CV_$moduleName

#include "Module.h"


/**
* @brief A simple example module that mimics a slit camera (or linear camera). The camera input is a range of pixels in the middle of the image.
*/
class $moduleName : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) :
			Module::Parameters(x_confReader)
		{
			Init();
		}
	};

	$moduleName(ParameterStructure& xr_params);
	~$moduleName();
	MKCLASS("$moduleName")
	MKCATEG("Input")
	MKDESCR("A simple example module that mimics a slit camera (or linear camera). The camera input is a range of pixels in the middle of the image.")
	virtual void ProcessFrame() override;
	virtual void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	$members
};

#endif

