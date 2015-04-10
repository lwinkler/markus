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
* @brief All informations that must be known by modules concerning application run-time
*/
/// All informations that must be known by modules concerning application run-time
class Context
{
public:
	class Parameters : public ParameterStructure
	{
	public:
		Parameters(const ConfigReader& x_confReader) : ParameterStructure(x_confReader)
		{
			m_list.push_back(new ParameterBool("auto_clean", 0, 0, 1,     &autoClean, "Automatically clean the temporary directory when the application closes"));
			m_list.push_back(new ParameterString("archive_dir", "",       &archiveDir, "If specified the data is copied inside this directory for archive"));
			ParameterStructure::Init();
		}
		bool autoClean;
		std::string archiveDir;
	};

	~Context();
	Context(const ParameterStructure& xr_params, const std::string& x_configFile, const std::string& x_applicationName, const std::string& x_outputDir);
	Context &operator = (const Context &x_context);
	static std::string Version(bool x_full);
	inline const std::string& GetOutputDir() const {if(m_outputDir.empty())throw MkException("Output dir has not been created", LOC); return m_outputDir;}
	inline const std::string& GetApplicationName() const {return m_applicationName;}

protected:
	std::string CreateOutputDir(const std::string& x_outputDir = "");
	std::string m_applicationName;
	std::string m_outputDir;
	std::string m_configFile;

private:
	Context(const Context &);
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
};
#endif
