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

#ifndef MKDIRECTORY_H
#define MKDIRECTORY_H

#include <log4cxx/logger.h>
#include <map>
#include <boost/thread/shared_mutex.hpp>
// Workaround: should be unnecessary in time: http://stackoverflow.com/questions/35007134/c-boost-undefined-reference-to-boostfilesystemdetailcopy-file
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#include "MkException.h"

namespace mk {
/**
* @brief A directory class to control accesses to the system file
*/
class MkDirectory : boost::noncopyable
{
public:
	MkDirectory(const std::string& x_dirName, MkDirectory& xr_parent, bool x_exist);
	MkDirectory(const std::string& x_dirName, bool x_exist);
	virtual ~MkDirectory();
	void CleanDir();
	bool IsEmpty();
	void CheckOutputDir();

	// All file system methods
	std::string ReserveFile(const std::string& x_file, int x_uniqueIndex = -1);
	void UnreserveFile(const std::string& x_file);
	void Cp(const std::string& x_fileName1, const std::string& x_fileName2 = "");
	void Rm(const std::string& x_fileName);
	inline bool FileExists(const std::string& x_fileName) {ReadLock(m_lock); return m_reservedFiles.find(x_fileName) != m_reservedFiles.end();}
	inline bool DirExists(const std::string& x_dirName) {ReadLock lock(m_lock); return FindSubDir(x_dirName) != nullptr;}
	inline const std::string& GetPath() const {return m_path;}

	// For context
	void ArchiveAndClean(bool x_clean, const std::string& x_archiveDir, const std::string& x_configFile);

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

	MkDirectory* FindSubDir(const std::string& x_dirName);
	inline void RegisterSubDir(const std::string& x_dirName, MkDirectory* xp_subdir){WriteLock(m_lock); mp_subDirectories.insert(std::make_pair(x_dirName, xp_subdir));}
	void UnregisterSubDir(MkDirectory* xp_subdir);

	typedef boost::shared_mutex Lock;
	typedef boost::unique_lock<Lock> WriteLock;
	typedef boost::shared_lock<Lock> ReadLock;
	Lock m_lock;
	std::map<std::string, bool> m_reservedFiles;
	std::map<std::string, MkDirectory*> mp_subDirectories;

private:
	static log4cxx::LoggerPtr m_logger;
	const std::string m_path;
	MkDirectory* mp_parent = nullptr;
};
} // namespace mk
#endif
