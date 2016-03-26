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

#ifndef MODULE_PYTHON_H
#define MODULE_PYTHON_H

#include <boost/python.hpp>
#include "Module.h"



/**
* @brief This class is a parent class for all module of keypoint extraction
* http://docs.opencv.org/modules/features2d/doc/common_interfaces_of_feature_detectors.html
*/
class ModulePython : public Module
{
	class OncePython final
	{
	public:
		OncePython();
		void Init()
		{
			// if(!m_initialized) // TODO: Check if ok!
				Py_Initialize();
			m_initialized = true;
		}
		~OncePython()
		{
			if(m_initialized)
				Py_Finalize();
		}
	protected:
		bool m_initialized = false;
	};

public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterString("script_path",  "modules2/FilterPython/vignettes", 	&scriptPath, "Path to the folder containing python scripts")); // TODO change value
			m_list.push_back(new ParameterString("script", 	     "markus.py",                               &script,     "Name of the Python script (without .py)"));

			Init();
		};
		std::string scriptPath;
		std::string script;
	};

	ModulePython(ParameterStructure& xr_params);
	virtual ~ModulePython();

	virtual void ProcessFrame() override;
	virtual void Reset() override;

private:
	Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
	static OncePython oncePython;

protected:
	// input

	// output

	// state variables
	boost::python::object m_pyMain;
	boost::python::object m_pyGlobals;
	boost::python::object m_pyModule;

#ifdef MARKUS_DEBUG_STREAMS
#endif
};


#endif
