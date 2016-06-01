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

// Workaround: should be unnecessary in time: http://stackoverflow.com/questions/35007134/c-boost-undefined-reference-to-boostfilesystemdetailcopy-file
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

#include <log4cxx/logger.h>
#include <boost/thread/shared_mutex.hpp>
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
		Parameters(const ConfigReader& x_confReader) : ParameterStructure(x_confReader)
		{
			AddParameter(new ParameterBool("auto_clean", 0, 0, 1,     &autoClean,       "Automatically clean the temporary directory when the application closes"));
			AddParameter(new ParameterString("archive_dir", "",       &archiveDir,      "If specified the data is copied inside this directory for archive"));
			AddParameter(new ParameterString("config_file", "",       &configFile,      "Name of the XML containing the configuration"));
			AddParameter(new ParameterString("application_name", "",  &applicationName, "Name of the application. May also be set in the XML as attribute of <application>"));
			AddParameter(new ParameterString("output_dir", "",        &outputDir,       "Directory used to write results files of manager and modules. If empty a directory is created from the date"));
			AddParameter(new ParameterBool("centralized", 0, 0, 1,    &centralized,     "All modules are called from the manager. Option -c"));
			AddParameter(new ParameterBool("robust",      0, 0, 1,    &robust,          "Continue if an exception occured during processing"));
			AddParameter(new ParameterBool("real_time", 0, 0, 1,      &realTime,        "All modules process in real-time. Disable to increase processing speed. Option -f"));
			AddParameter(new ParameterString("job_id"      ,  ""    , &jobId         ,  "Job id for storage in database. Leave empty to generate a random value"));
			AddParameter(new ParameterString("camera_id",  ""       , &cameraId      ,  "CameraId id for storage in database. Leave empty for tests only."));
			AddParameter(new ParameterString("cache_directory", ""  , &cacheDirectory,  "The directory in which the cache can be found, empty if no cache"));
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
		std::string cacheDirectory;
	};

	virtual ~Context();
	Context(ParameterStructure& xr_params);

	// All file system methods
	void MkDir(const std::string& x_directory);
	std::string ReserveFile(const std::string& x_file);
	void UnreserveFile(const std::string& x_file);
	void Cp(const std::string& x_fileName1, const std::string& x_fileName2);
	void Rm(const std::string& x_fileName);
	void RmDir(const std::string& x_directory);

	// note: All public methods must be thread-safe
	static std::string Version(bool x_full);
	inline const std::string& GetOutputDir() const {if(m_outputDir.empty())throw MkException("Output dir has not been created", LOC); return m_outputDir;}
	inline const std::string& GetApplicationName() const {return m_param.applicationName;}
	inline const std::string& GetJobId() const {return m_jobId;}
	inline const std::string& GetCameraId() const {return m_param.cameraId;}
	bool IsOutputDirEmpty();
	inline bool IsCentralized() const {return m_param.centralized;}
	inline bool IsRealTime() const {return m_param.realTime;}
	const Parameters& GetParameters() const {return m_param;}

protected:
	inline static void mkDir(const std::string& x_directory)
	{
		boost::filesystem::create_directories(x_directory);
	}
	inline static void rm(const std::string& x_file)
	{
		if(!boost::filesystem::remove(x_file))
			LOG_WARN(m_logger, "Cannot remove unexistant file " << x_file);
	}
	inline static void cp(const std::string& x_filePath1, const std::string& x_filePath2)
	{
		if(! boost::filesystem::exists(x_filePath1))
		{
			throw MkException("Cannot copy unexistant file " + x_filePath1, LOC);
		}
		if(boost::filesystem::exists(x_filePath2))
		{
			LOG_WARN(m_logger, "File " << x_filePath2 << " overwritten");
		}
		boost::filesystem::copy_file(x_filePath1, x_filePath2, boost::filesystem::copy_option::overwrite_if_exists);
	}
	inline static void rmDir(const std::string& x_directory)
	{
		if(!boost::filesystem::remove_all(x_directory))
			LOG_WARN(m_logger, "Cannot remove unexisting directory " << x_directory);
	}
	inline static void mv(const std::string& x_path1, const std::string& x_destDir)
	{
		boost::filesystem::rename(x_path1, x_destDir);
	}

	void CreateOutputDir(const std::string& x_outputDir, const std::string& x_timeStamp);
	std::string m_outputDir;
	std::string m_jobId;

	typedef boost::shared_mutex Lock;
	typedef boost::unique_lock<Lock> WriteLock;
	typedef boost::shared_lock<Lock> ReadLock;
	Lock m_lock;
	std::map<std::string, bool> m_reservedFiles;

private:
	DISABLE_COPY(Context)
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
	static bool m_unique;
};
#endif
