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

#ifdef COMMENTED
#ifndef MODULE_PYTHON_H
#define MODULE_PYTHON_H

#include <boost/python.hpp>
#include "Module.h"

namespace mk {
class FeaturePtr;

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
			if(!m_initialized)
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
protected:

	/// A struct representing a list of features and the names of features to use (for use in method calls)
	struct FeatureList
	{
		FeatureList(const std::map <std::string, FeaturePtr>& xr_features, const std::vector<std::string>& xr_featureNames)
		: features(xr_features), featureNames(xr_featureNames) {}
		int GetNumberOfFeatures() const;

		const std::map <std::string, FeaturePtr>& features;
		const std::vector<std::string>& featureNames;
	};

	// Conversion structures
	struct MatToPython
	{
		static PyObject* convert(cv::Mat const& x_mat);
	};
	struct FeatureListToPython
	{
		static PyObject* convert(FeatureList const& x_mat);
	};

public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterString("scriptPath",  "python_dir", &scriptPath, "Path to the folder containing python scripts"));
			AddParameter(new ParameterString("script", 	     "script.py",  &script,     "Name of the Python script (without .py)"));
		};
		std::string scriptPath;
		std::string script;
	};

	explicit ModulePython(ParameterStructure& xr_params);
	~ModulePython() override;

private:
	Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
	static OncePython oncePython;

protected:
	void ProcessFrame() override = 0;
	void Reset() override;

	// input

	// output

	// state variables
	boost::python::object m_pyMain;
	boost::python::object m_pyGlobals;
	boost::python::object m_pyModule;
};


} // namespace mk
#endif
#endif
