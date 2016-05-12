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

#include <fstream>
#include <unistd.h>


using namespace std;

log4cxx::LoggerPtr Context::m_logger(log4cxx::Logger::getLogger("Context"));

Context::Context(ParameterStructure& xr_params) :
	Configurable(xr_params),
	m_param(dynamic_cast<const Parameters&>(xr_params))
{
	m_outputDir = CreateOutputDir(m_param.outputDir);
	if(m_param.jobId.empty())
	{
		LOG_INFO(m_logger, "A test jobId is created from time stamp. This should only be used for tests");
		m_jobId   = "test_" + timeStamp(getpid());
	}
	else m_jobId = m_param.jobId;
	LOG_INFO(m_logger, "Created context with cameraId=\"" << GetCameraId() << "\", jobId=\""
			 << GetJobId() << "\", applicationName=\"" << GetApplicationName() << "\", configFile=\"" << m_param.configFile << "\"");
	m_param.PrintParameters();
}

Context::~Context()
{
	// check if dir is empty and was automatically generated (no -o option)
	bool empty = m_param.outputDir.empty() && IsOutputDirEmpty();

	if(!empty)
		LOG_INFO(m_logger, "Results written to directory "<<m_outputDir);

	if(getenv("LOG_DIR") == nullptr && !empty)
	{
		try
		{
			SYSTEM("cp markus.log " + m_outputDir + "/markus.copy.log");
		}
		catch(...)
		{
			LOG_WARN(m_logger, "Error at the copy of markus.log");
		}
	}

	// Copy the directory for archiving if needed
	try
	{
		if(m_param.autoClean)
		{
			if(m_param.archiveDir != "")
			{
				LOG_INFO(m_logger, "Working directory moved to " + m_param.archiveDir);
				SYSTEM("mkdir -p " + m_param.archiveDir);
				SYSTEM("mv " + m_outputDir + " " + m_param.archiveDir + "/");
			}
			else
			{
				LOG_INFO(m_logger, "Working directory deleted");
				SYSTEM("rm -rf " + m_outputDir);
			}
		}
		else
		{
			if(m_param.archiveDir != "")
			{
				LOG_INFO(m_logger, "Working directory moved to " + m_param.archiveDir);
				SYSTEM("mkdir -p " + m_param.archiveDir);
				SYSTEM("mv " + m_outputDir + " " + m_param.archiveDir + "/");
			}
			else
			{
				// Remove directory if empty else copy XML config
				if(empty)
				{
					LOG_INFO(m_logger, "Removing empty directory " << m_outputDir);
					SYSTEM("rm -r " + m_outputDir);
				}
				else
				{
					// note: -n means to avoid an error message if we copy a file to itself (happens with markus -o ...)
					SYSTEM("cp -n " + m_param.configFile + " " + m_outputDir);
				}
			}
		}
	}
	catch(MkException &e)
	{
		LOG_ERROR(m_logger, "Exception thrown while archiving: " << e.what());
	}
}

/**
* @brief Return a directory that will contain all outputs files and logs. The dir is created at the first call of this method.
*
* @param x_outputDir  Name and path to the dir, if empty, the name is generated automatically
*
* @return Name of the output dir
*/
string Context::CreateOutputDir(const string& x_outputDir)
{
	string outputDir;
	try
	{
		if(x_outputDir == "")
		{
			SYSTEM("mkdir -p out/");
			outputDir = "out/out_" + timeStamp();
			int16_t trial = 0; // Must NOT be a char to avoid concatenation problems!
			string tmp = outputDir;

			// Try to create the output dir, if it fails, try changing the name
			while(trial < 250)
			{
				try
				{
					SYSTEM("mkdir \"" + outputDir + "\"");
					trial = 250;
				}
				catch(...)
				{
					stringstream ss;
					trial++;
					ss<<tmp<<"_"<<trial;
					outputDir = ss.str();
					if(trial == 250)
						throw MkException("Cannot create output directory", LOC);
				}
			}

			// note: do not log as logger may not be initialized
			// cout<<"Creating directory "<<outputDir<<" and symbolic link out_latest"<<endl;
			// note: use ln with args sfn to override existing link
			SYSTEM("ln -sfn \"" + outputDir + "\" out_latest");
		}
		else
		{
			// If the name is specified do not check if the direcory exists
			outputDir = x_outputDir;
			SYSTEM("mkdir -p \"" + outputDir + "\"");

			// note: do not log as logger may not be initialized
			// Copy config file to output directory
			// cout<<"Creating directory "<<outputDir<<endl;
			SYSTEM("cp " + m_param.configFile + " " + outputDir);
		}
	}
	catch(exception& e)
	{
		cerr << "Exception in Context::CreateOutputDir: " << e.what() << endl;
	}
	return outputDir;
}

/**
* @brief Return true if the output directory contains no file
*
* @return True if dir is empty
*/
bool Context::IsOutputDirEmpty() const
{
	vector<string> res;
	// TODO Avoid using commands as much as possible
	execute("ls -A " + m_outputDir + " | wc -l", res);

	return res.at(0) == "0";
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
