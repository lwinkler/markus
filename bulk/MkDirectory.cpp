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

#include "MkDirectory.h"
#include "util.h"

#include <boost/format.hpp>

using namespace std;

log4cxx::LoggerPtr MkDirectory::m_logger(log4cxx::Logger::getLogger("MkDirectory"));

MkDirectory::MkDirectory(const std::string& x_dirName, MkDirectory& xr_parent, bool x_exist) : m_path(xr_parent.GetPath() + "/" + x_dirName), mp_parent(&xr_parent)
{
	if(m_path == "." || m_path == ".." || m_path.empty())
	{
		throw FatalException("Invalid directory path '" + m_path + "', aborting", LOC);
	}
	if(x_dirName.find('/') != std::string::npos)
		throw MkException("Directory name must not containt '/': " + x_dirName, LOC);
	xr_parent.RegisterSubDir(x_dirName, this);
	if(!x_exist)
		mkDir(m_path);
}
MkDirectory::MkDirectory(const std::string& x_dirName, const std::string& x_path, bool x_exist) : m_path(x_path + "/" + x_dirName), mp_parent(nullptr)
{
	if(m_path == "." || m_path == ".." || m_path.empty())
	{
		throw FatalException("Invalid directory path '" + m_path + "', aborting", LOC);
	}
	// if(x_dirName.find('/') != std::string::npos)
		// throw MkException("Directory name must not containt '/': " + x_dirName, LOC);
	if(!x_exist)
		mkDir(m_path);
}

MkDirectory::~MkDirectory()
{
	// note: do not erase, link only
	for(const auto& elem : mp_subDirectories)
	{
		LOG_ERROR(m_logger, "Sub directory " << elem.first << " was not unregistred from parent " << m_path);
	}
	if(mp_parent != nullptr)
		mp_parent->UnregisterSubDir(this);
}

/**
* @brief Return pointer to a subdirectory
*
* @return Pointer or null if unexistant
*/
MkDirectory* MkDirectory::FindSubDir(const string& x_dirName)
{
	for(auto elem : mp_subDirectories) {
		if(elem.first == x_dirName)
			return elem.second;
	}
	return nullptr;
}

/**
* @brief Return true if the output directory contains no file
*
* @return True if dir is empty
*/
bool MkDirectory::IsEmpty()
{
	LOG_DEBUG(m_logger, "Lock MkDirectory, line " << __LINE__);
	ReadLock lock(m_lock);
	return m_reservedFiles.empty();
}

void MkDirectory::UnregisterSubDir(MkDirectory* xp_subdir)
{
	WriteLock(m_lock);
	for(auto it = mp_subDirectories.begin() ; it != mp_subDirectories.end() ; ++it)
	{
		if(it->second == xp_subdir)
		{
			if(it->second == xp_subdir)
			{
				mp_subDirectories.erase(it);
				return;
			}
		}
	}
}

/**
* @brief Check that the output dir is in a coherent state with the reserved objects
*/
void MkDirectory::CheckOutputDir()
{
	LOG_DEBUG(m_logger, "Lock MkDirectory, line " << __LINE__);

	// recursive
	for(auto elem : mp_subDirectories)
		elem.second->CheckOutputDir();

	ReadLock lock(m_lock);
	vector<string> res;
	execute("find " + m_path + " -maxdepth 1 -type f | sed -n 's|^" + m_path + "/||p'", res);

	if(m_reservedFiles.size() != res.size())
		LOG_WARN(m_logger, res.size() << " files found in " << m_path << " for " << m_reservedFiles.size() << " correctly reserved");

	for(auto& elem : m_reservedFiles)
	{
		if(find(res.begin(), res.end(), elem.first) == res.end())
			LOG_WARN(m_logger, "File " << elem.first << " was reserved but is not present in output directory " << m_path);
	}
	for(auto& elem : res)
	{
		if(m_reservedFiles.find(elem) == m_reservedFiles.end())
			LOG_WARN(m_logger, "File " << elem << " is present in output directory " << m_path << " but was not reserved");
	}
	LOG_DEBUG(m_logger, "Reserved files " << m_reservedFiles.size() << " line:" << __LINE__)
}


/**
* @brief Clean the output directory
*
*/
void MkDirectory::CleanDir()
{
	CheckOutputDir(); // must be before lock
	LOG_DEBUG(m_logger, "Lock MkDirectory, line " << __LINE__);
	map<string, bool> reservedFiles;
	{
		ReadLock lock(m_lock);
		reservedFiles = m_reservedFiles; // since we remove from the map
	}
	// recursive
	for(auto elem : mp_subDirectories)
		elem.second->CleanDir();
	for(const auto elem : reservedFiles)
	{
		Rm(elem.first);
	}
	m_reservedFiles.clear();
	LOG_DEBUG(m_logger, "Reserved files " << m_reservedFiles.size() << " line:" << __LINE__)
}

/**
* @brief Reserve a file inside the output directory
*
* @param x_filePath    File name with path
* @param x_uniqueIndex Optional. An index to generate a unique file name 
* @return the full path to the file resource
*/
std::string MkDirectory::ReserveFile(const std::string& x_filePath, int x_uniqueIndex)
{
	LOG_DEBUG(m_logger, "Lock MkDirectory, line " << __LINE__);
	WriteLock lock(m_lock);
	stringstream ss;
	if(x_uniqueIndex < 0 || x_filePath.find("%d") == string::npos)
		ss << x_filePath; 
	else
		ss << boost::format(x_filePath) % x_uniqueIndex; 
	LOG_INFO(m_logger, "Reserve output file " << ss.str());
	auto ret = m_reservedFiles.insert(std::pair<string, bool>(ss.str(), true));
	LOG_DEBUG(m_logger, "Reserved files " << m_reservedFiles.size() << " line:" << __LINE__)
	if(ret.second == false)
	{
		LOG_WARN(m_logger, "File is overridden in output directory: " << ss.str());
	}
	return m_path + "/" + ss.str();
}

/**
* @brief Uneserve a file inside the output directory
*
* @param x_filePath File name with path
*/
void MkDirectory::UnreserveFile(const std::string& x_filePath)
{
	LOG_DEBUG(m_logger, "Lock MkDirectory, line " << __LINE__);
	WriteLock lock(m_lock);
	auto it = m_reservedFiles.find(x_filePath);
	if(it == m_reservedFiles.end())
	{
		LOG_WARN(m_logger, "Removing unknown file from output directory");
	}
	else
	{
		m_reservedFiles.erase(it);
	}
	LOG_DEBUG(m_logger, "Reserved files " << m_reservedFiles.size() << " line:" << __LINE__)
}

/**
* @brief Remove a file inside output directory
*
* @param x_filePath1 File name with path
* @param x_filePath2 File name with path (inside output directory), if empty, keep the same file
*/
void MkDirectory::Cp(const std::string& x_fileName1, const std::string& x_fileName2)
{
	string dest = x_fileName2.empty() ? basename(x_fileName1) : x_fileName2;
	LOG_DEBUG(m_logger, "Copy file " << x_fileName1 << " into output dir " << m_path << " as " << dest);
	cp(x_fileName1, ReserveFile(dest));
}

/**
* @brief Remove a file inside output directory
*
* @param x_fileName File name
*/
void MkDirectory::Rm(const std::string& x_fileName)
{
	LOG_DEBUG(m_logger, "Remove file " << x_fileName << " from output dir");
	UnreserveFile(x_fileName);
	rm(m_path + "/" + x_fileName);
}

/**
* @brief Remove a directory inside output directory
*
* @param x_directory MkDirectory name
*/
/*
void MkDirectory::RmDir(const std::string& x_directory)
{
	rmDir(m_path + "/" + x_directory);
}
*/

/**
* @brief Remove a directory inside output directory
*
* @param x_directory MkDirectory name
*/
void MkDirectory::ArchiveAndClean(bool x_clean, const string& x_archiveDir, const string& x_configFile)
{
	WriteLock lock(m_lock);
	if(mp_parent != nullptr)
		throw MkException("Unexpected: ArchiveAndClean should only be called from main output directory", LOC);
	if(x_clean)
	{
		if(x_archiveDir != "")
		{
			LOG_INFO(m_logger, "Working directory moved to " + x_archiveDir);
			mkDir(x_archiveDir);
			mv(GetPath(), x_archiveDir);
		}
		else
		{
			LOG_INFO(m_logger, "Working directory deleted");
			CleanDir();
			rmDir(GetPath());
		}
	}
	else
	{
		if(x_archiveDir != "")
		{
			LOG_INFO(m_logger, "Working directory moved to " + x_archiveDir);
			mkDir(x_archiveDir);
			mv(GetPath(), x_archiveDir);
		}
		else
		{
			// Remove directory if empty else copy XML config
			if(IsEmpty())
			{
				LOG_INFO(m_logger, "Removing empty directory " << GetPath());
				rmDir(GetPath());
			}
			else
			{
				if(!FileExists(basename(x_configFile)))
					Cp(x_configFile, "");
			}
		}
	}
}
