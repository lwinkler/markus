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
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>
#include <log4cxx/logger.h>
#include "MkException.h"
#include "ConfigReader.h"
#include "ParameterStructure.h"
#include "ParameterString.h"
#include "ParameterNum.h"

/**
* @brief All informations that must be known by modules concerning application run-time. Including access to file system.
*/
class Context : public Configurable
{
public:
	class Parameters : public ParameterStructure
	{
	public:
		Parameters(const ConfigReader& x_confReader, const std::string& x_configFile, const std::string& x_applicationName, const std::string& x_outputDir) : ParameterStructure(x_confReader)
		{
			m_list.push_back(new ParameterBool("auto_clean", 0, 0, 1,     &autoClean,       "Automatically clean the temporary directory when the application closes"));
			m_list.push_back(new ParameterString("archive_dir", "",       &archiveDir,      "If specified the data is copied inside this directory for archive"));
			m_list.push_back(new ParameterString("config_file", "",       &configFile,      "Name of the XML containing the configuration"));
			m_list.push_back(new ParameterString("application_name", "",  &applicationName, "Name of the application. May also be set in the XML as attribute of <application>"));
			m_list.push_back(new ParameterString("output_dir", "",        &outputDir,       "Directory used to write results files of manager and modules. If empty a directory is created from the date"));
			m_list.push_back(new ParameterBool("centralized", 0, 0, 1,    &centralized,     "All modules are called from the manager. Option -c"));
			m_list.push_back(new ParameterBool("robust",      0, 0, 1,    &robust,          "Continue if an exception occured during processing"));
			m_list.push_back(new ParameterBool("real_time", 0, 0, 1,      &realTime,        "All modules process in real-time. Disable to increase processing speed. Option -f"));
			m_list.push_back(new ParameterString("job_id"      ,  ""    , &jobId         ,  "Job id for storage in database. Leave empty to generate a random value"));
			m_list.push_back(new ParameterInt("camera_id",  0, 0, INT_MAX, &cameraId     ,  "CameraId id for storage in database. Leave empty for tests only."));
			m_list.push_back(new ParameterString("cache_directory", ""  , &cacheDirectory,  "The directory in which the cache can be found, empty if no cache"));
			ParameterStructure::Init();

			// Override values: must be set for each run
			configFile      = x_configFile;
			applicationName = x_applicationName;
			outputDir       = x_outputDir;
		}
		bool autoClean;
		std::string archiveDir;
		std::string configFile;
		std::string applicationName;
		std::string outputDir;
		bool centralized;
		bool realTime;
		bool robust;
		std::string jobId;
		int cameraId;
		std::string cacheDirectory;
	};

	~Context();
	Context(ParameterStructure& xr_params);
	static std::string Version(bool x_full);
	inline const std::string& GetOutputDir() const {if(m_outputDir.empty())throw MkException("Output dir has not been created", LOC); return m_outputDir;}
	inline const std::string& GetApplicationName() const {return m_param.applicationName;}
	inline const std::string& GetJobId() const {return m_jobId;}
	bool IsOutputDirEmpty() const;
	inline bool IsCentralized() const {return m_param.centralized;}
	inline bool IsRealTime() const {return m_param.realTime;}
	const Parameters& GetParameters() const {return m_param;}

protected:
	std::string CreateOutputDir(const std::string& x_outputDir = "");
	std::string m_outputDir;
	std::string m_jobId;

private:
	DISABLE_COPY(Context)
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
};
#endif
