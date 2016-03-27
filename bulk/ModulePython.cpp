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
#include "FeatureVector.h"
#include "FeatureStd.h"

using namespace cv;
using namespace std;
using namespace boost::python;

ModulePython::OncePython ModulePython::oncePython;
log4cxx::LoggerPtr ModulePython::m_logger(log4cxx::Logger::getLogger("ModulePython"));

/// Convert cv::Mat to Python: https://misspent.wordpress.com/2009/09/27/how-to-write-boost-python-converters/
PyObject* ModulePython::MatToPython::convert(cv::Mat const& x_mat)
{
	assert(x_mat.type() == CV_8UC1);
	PyObject *mylist = PyList_New(x_mat.cols * x_mat.rows);
	assert(mylist);
	int k = 0;
	for (int i = 0; i < x_mat.rows; i++)
	{
		for (int j = 0; j < x_mat.cols; j++)
		{
			PyList_SET_ITEM(mylist, k++, PyInt_FromLong(x_mat.at<uchar>(i, j)));// PyInt_FromLong(array[x_mat.step[0] * i + j]));
		}
	}
	Py_BuildValue("[i]", mylist);
	// return boost::python::incref(boost::python::object("Thisis not a mat").ptr());
	return boost::python::incref(mylist);
}

/// Convert FeatureList to Python
PyObject* ModulePython::FeatureListToPython::convert(ModulePython::FeatureList const& x_list)
{
	PyObject *mylist = PyList_New(x_list.GetNumberOfFeatures());
	int cpt = 0;
	for (const auto & featureName : x_list.featureNames)
	{
		// Retrieve the feature with the given name
		auto feat = x_list.features.find(featureName);
		if(feat == x_list.features.end())
			throw MkException("Feature " + featureName + " not found in input object", LOC);

		//LOG_DEBUG(m_logger, "Add feature "<<featureName<<" to Python arguments: "<<*feat->second);
		const FeatureFloat* pff = dynamic_cast<const FeatureFloat*>(&*feat->second); //TODO: Define cast for FeaturePtr ?
		if(pff != nullptr)
		{
			PyList_SET_ITEM(mylist, cpt, PyFloat_FromDouble(pff->value));
			cpt++;
		}
		else
		{
			const FeatureVectorFloat* pff = dynamic_cast<const FeatureVectorFloat*>(&*feat->second);
			if(pff == nullptr)
				throw MkException("Feature " + featureName + " must inherit from FeatureFloat or FeatureVectorFloat", LOC);
			for(auto val : pff->values)
			{
				PyList_SET_ITEM(mylist, cpt, PyFloat_FromDouble(val));
				cpt++;
			}
		}
	}
	Py_BuildValue("[d]", mylist);
	return boost::python::incref(mylist);
}

int ModulePython::FeatureList::GetNumberOfFeatures() const
{
	int cnt = 0;
	for (const auto & featureName : featureNames)
	{
		// Retrieve the feature with the given name
		auto feat = features.find(featureName);
		if(feat == features.end())
			throw MkException("Feature " + featureName + " not found in input object", LOC);
		const FeatureFloat* pff = dynamic_cast<const FeatureFloat*>(&*feat->second);
		if(pff != nullptr)
		{
			cnt++;
		}
		else
		{
			const FeatureVectorFloat* pff = dynamic_cast<const FeatureVectorFloat*>(&*feat->second);
			if(pff != nullptr) cnt+= pff->values.size();
		}
	}
	return cnt;
}

ModulePython::OncePython::OncePython()
{
	// Register converters
	boost::python::to_python_converter<cv::Mat,MatToPython>();
	boost::python::to_python_converter<FeatureList,FeatureListToPython>();
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
		char pwd[256];
		char* res = getcwd(pwd, sizeof(pwd));

		m_pyMain    = boost::python::import("__main__");
		m_pyGlobals = m_pyMain.attr("__dict__");

		stringstream ss;
		ss << "import sys\n" 
		   << "sys.path.append('" << pwd << "/" << m_param.scriptPath << "')";

		exec(ss.str().c_str(), m_pyGlobals, m_pyGlobals);
		m_pyModule  = boost::python::exec_file(m_param.script.c_str(), m_pyGlobals, m_pyGlobals);
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

