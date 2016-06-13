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

#include "Context.h"
#include "util.h"
#include "version.h"

#include <boost/format.hpp>

using namespace std;

log4cxx::LoggerPtr Context::m_logger(log4cxx::Logger::getLogger("Context"));
bool Context::m_unique = false;

Context::Context(ParameterStructure& xr_params) :
	Configurable(xr_params),
	m_param(dynamic_cast<const Parameters&>(xr_params))
{
	LOG_DEBUG(m_logger, "Create context object");
	if(m_unique)
		throw MkException("More than one Context object was created. This should never happen.", LOC);
	m_unique = true;
	if(m_param.configFile.empty())
		throw MkException("Config file name is empty", LOC);

	string ts = timeStamp(getpid());
	CreateOutputDir(m_param.outputDir, ts);
	if(m_param.jobId.empty())
	{
		LOG_INFO(m_logger, "A test jobId is created from time stamp. This should only be used for tests");
		m_jobId   = "test_" + ts;
	}
	else m_jobId = m_param.jobId;
	LOG_INFO(m_logger, "Created context with cameraId=\"" << GetCameraId() << "\", jobId=\""
		<< GetJobId() << "\", applicationName=\"" << GetApplicationName() << "\", configFile=\"" << m_param.configFile << "\"");
	m_param.PrintParameters();
}

Context::~Context()
{
	LOG_DEBUG(m_logger, "Destroy context object");
	// check if dir is empty and was automatically generated (no -o option)
	mp_outputDir->CheckOutputDir();
	bool empty = m_param.outputDir.empty() && mp_outputDir->IsEmpty();

	if(!empty)
		LOG_INFO(m_logger, "Results written to directory "<<mp_outputDir->GetPath());

	if(getenv("LOG_DIR") == nullptr && !empty)
	{
		try
		{
			// note: for tests the log file is located in tests
			mp_outputDir->Cp(m_param.outputDir == "tests/out" ? "tests/markus.log" : "markus.log", "markus.copy.log");
		}
		catch(exception& e)
		{
			LOG_WARN(m_logger, "Error at the copy of markus.log: " << e.what());
		}
	}

	// Copy the directory for archiving if needed
	try
	{
		mp_outputDir->ArchiveAndClean(m_param.autoClean, m_param.archiveDir, m_param.configFile);
	}
	catch(MkException &e)
	{
		LOG_ERROR(m_logger, "Exception thrown while archiving: " << e.what());
	}
	m_unique = false;
}

/**
* @brief Return a directory that will contain all outputs files and logs. The dir is created at the first call of this method.
*
* @param x_outputDir  Name and path to the dir, if empty, the name is generated automatically
*
* @return Name of the output dir
*/
void Context::CreateOutputDir(const string& x_outputDir, const string& x_timeStamp)
{
	string outputDir;
	try
	{
		if(x_outputDir.empty())
		{
			MkDirectory::mkDir("out");
			outputDir = "out/out_" + x_timeStamp;
			if(boost::filesystem::exists(outputDir))
				throw MkException("Output directory already existing: " + outputDir, LOC);
			// MkDirectory::mkDir(outputDir);

			// note: do not log as logger may not be initialized
			mp_outputDir.reset(new MkDirectory(outputDir, ".", false));
			boost::filesystem::remove("out_latest");
			boost::filesystem::create_symlink(outputDir, "out_latest");
		}
		else
		{
			// If the name is specified do not check if the directory exists
			outputDir = x_outputDir;
			// MkDirectory::mkDir(outputDir);

			// note: do not log as logger may not be initialized
			// Copy config file to output directory
			mp_outputDir.reset(new MkDirectory(outputDir, ".", false));
			mp_outputDir->Cp(m_param.configFile, "");
		}
	}
	catch(exception& e)
	{
		throw MkException("Exception in Context::CreateOutputDir: " + string(e.what()), LOC);
	}
}


/**
* @brief Return a string containing the version of the executable
*
* @param x_full Return the full info string with info on host
*
* @return Version
*/
string Context::Version(bool x_full)
{
	stringstream ss;
	if(x_full)
		ss<<"Markus version "<<VERSION_STRING
		  <<", compiled with Opencv "<<CV_VERSION
		  << ", vp-detection modules version "<<VERSION_STRING2
		  << ", built on "<<VERSION_BUILD_HOST;
	else
		ss<<VERSION_STRING<<","<<VERSION_STRING2;

	return ss.str();
}

