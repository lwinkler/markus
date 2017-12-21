
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

#include "MarkusApplication.h"
#include "Manager.h"
#include <opencv2/opencv.hpp>
namespace mk {

log4cxx::LoggerPtr MarkusApplication::m_logger(log4cxx::Logger::getLogger("MarkusApplication"));

MarkusApplication::MarkusApplication(int &argc, char ** argv) : QApplication(argc, argv) {}


//----------------------------------------------------------------------------------------------------
// Reimplement the notify function to manage exceptions
//----------------------------------------------------------------------------------------------------
bool MarkusApplication::notify(QObject *receiver_, QEvent *event_)
{
	try
	{
		return QApplication::notify(receiver_, event_);
	}
	catch(cv::Exception& e)
	{
		LOG_ERROR(m_logger, "Exception raised (cv::Exception): "<<e.what());
	}
	catch(std::exception& e)
	{
		LOG_ERROR(m_logger, "Exception raised (std::exception): "<<e.what());
		// exit(0);
	}
	catch(std::string& str)
	{
		LOG_ERROR(m_logger, "Exception raised (string): " << str);
	}
	catch(const char* str)
	{
		LOG_ERROR(m_logger, "Exception raised (const char*): " << str);
	}
	catch(...)
	{
		LOG_ERROR(m_logger, "Unknown exception");
	}

	return false;
}
} // namespace mk
