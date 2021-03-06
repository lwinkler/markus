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

#include <log4cxx/logger.h>
#include "MkException.h"
#include "Configurable.h"
#include "ParameterStructure.h"
#include "ParameterT.h"
#include "MkDirectory.h"

namespace mk {
/**
* @brief All informations that must be known by modules concerning application run-time. Including access to file system.
*/
class Context : public Configurable
{
public:
	class Parameters : public ParameterStructure
	{
	public:
		explicit Parameters(const std::string& x_name) : ParameterStructure(x_name)
		{
			AddParameter(new ParameterBool("autoClean", false,       &autoClean,       "Automatically clean the temporary directory when the application closes"));
			AddParameter(new ParameterString("archiveDir", "",       &archiveDir,      "If specified the data is copied inside this directory for archive"));
			AddParameter(new ParameterString("configFile", "",       &configFile,      "Name of the JSON containing the configuration"));
			AddParameter(new ParameterString("applicationName", "",  &applicationName, "Name of the application. May also be set in the JSON as attribute of <application>"));
			AddParameter(new ParameterString("outputDir", "",        &outputDir,       "Directory used to write results files of manager and modules. If empty a directory is created from the date"));
			AddParameter(new ParameterBool("centralized", false,      &centralized,     "All modules are called from the manager. Option -c"));
			AddParameter(new ParameterBool("robust",      false,      &robust,          "Continue if an exception occured during processing"));
			AddParameter(new ParameterBool("realTime", false,        &realTime,        "All modules process in real-time. Disable to increase processing speed. Option -f"));
			AddParameter(new ParameterString("jobId"      ,  ""    , &jobId         ,  "Job id for storage in database. Leave empty to generate a random value"));
			AddParameter(new ParameterString("cameraId",  ""       , &cameraId      ,  "CameraId id for storage in database. Leave empty for tests only."));
			AddParameter(new ParameterString("cacheIn",        ""  , &cacheIn       ,  "The cache directory of a previous, empty if no cache, relative to output directory"));
			AddParameter(new ParameterString("cacheOut",       ""  , &cacheOut      ,  "The directory in which the cache should be written, empty if no cache, relative to current directory"));
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
		std::string cameraId;
		std::string cacheIn;
		std::string cacheOut;
	};

	~Context() override;
	explicit Context(ParameterStructure& xr_params);

	// note: All public methods must be thread-safe
	static std::string Version(bool x_full);
	inline const std::string& GetApplicationName() const {return m_param.applicationName;}
	inline const std::string& GetJobId() const {return m_jobId;}
	inline const std::string& GetCameraId() const {return m_param.cameraId;}
	inline MkDirectory& RefOutputDir()
	{
		if(mp_outputDir.get() == nullptr)
			throw MkException("No output dir exists", LOC);
		return *mp_outputDir;
	}
	inline MkDirectory& RefCacheIn()
	{
		if(mp_cacheIn.get() == nullptr)
			throw MkException("No input cache dir exists, use option -I", LOC);
		return *mp_cacheIn;
	}
	inline MkDirectory& RefCacheOut()
	{
		if(mp_cacheOut.get() == nullptr)
			throw MkException("No output cache dir exists, use option -O", LOC);
		return *mp_cacheOut;
	}
	inline bool IsCentralized() const {return m_param.centralized;}
	inline bool IsRealTime() const {return m_param.realTime;}
	const Parameters& GetParameters() const override {return m_param;}

protected:
	void CreateOutputDir(const std::string& x_outputDir, const std::string& x_timeStamp);
	std::string m_jobId;
	std::unique_ptr<MkDirectory> mp_outputDir;
	std::unique_ptr<MkDirectory> mp_cacheIn;
	std::unique_ptr<MkDirectory> mp_cacheOut;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
	static bool m_unique;
};
} // namespace mk
#endif
