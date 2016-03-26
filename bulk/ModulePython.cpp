/*----------------------------------------------------------------------------------
 *
 *    MARKUS : a manager for video analysis modules
 *
 *    author : Laurent Winkler and Florian Rossier <florian.rossier@gmail.com>
 *
 *
 *    This file is part of Markus.
 *
 *    Markus is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *    Markus is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
 -------------------------------------------------------------------------------------*/

#include "ModulePython.h"

using namespace cv;
using namespace std;
using namespace boost::python;

ModulePython::OncePython ModulePython::oncePython;
log4cxx::LoggerPtr ModulePython::m_logger(log4cxx::Logger::getLogger("ModulePython"));

/// Converter: https://misspent.wordpress.com/2009/09/27/how-to-write-boost-python-converters/
struct MatToPython
{
	static PyObject* convert(cv::Mat const& x_mat)
	{
		return boost::python::incref(boost::python::object("Thisis not a mat").ptr());
	}
};

ModulePython::OncePython::OncePython()
{
	// Register converters TODO: In oncepython
	boost::python::to_python_converter<cv::Mat,MatToPython>();
}

ModulePython::ModulePython(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params))
{
	try
	{
		LOG_DEBUG(m_logger, "Initialize module from Python file " << m_param.script);
		oncePython.Init();

		// Change working directory (for python)
		// string syspath;
		char pwd[256];
		// PyRun_SimpleString("import sys");
		char* res = getcwd(pwd, sizeof(pwd));
		// assert(res != nullptr);
		// syspath = "sys.path.append('" + string(pwd) + "/" + m_param.scriptPath + "')";
		// PyRun_SimpleString(syspath.c_str());

		m_pyMain    = boost::python::import("__main__"); // TODO keep ?
		m_pyGlobals = m_pyMain.attr("__dict__");

		stringstream ss;
		ss << "import sys\n" 
		   << "sys.path.append('" << pwd << "/" << m_param.scriptPath << "')";

		exec(ss.str().c_str(), m_pyGlobals, m_pyGlobals);
		m_pyModule  = boost::python::exec_file(m_param.script.c_str(), m_pyGlobals, m_pyGlobals);
		m_pyGlobals["greet"]();
	}
	catch(...)
	{
		PyErr_Print();
		throw MkException("Error in initialization of ModulePython with script " + m_param.script, LOC);
	}

};


ModulePython::~ModulePython()
{
}

void ModulePython::Reset()
{
	Module::Reset();
}

/**
 */
void ModulePython::ProcessFrame()
{
}

