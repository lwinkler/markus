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

#include "$moduleName.h"
#include "StreamNum.h"
#include "StreamImage.h"
$includes

namespace mk {
using namespace std;

log4cxx::LoggerPtr $moduleName::m_logger(log4cxx::Logger::getLogger("$moduleName"));

$moduleName::$moduleName(ParameterStructure& xr_params):
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params))
	$init
{
	$streams
}

void $moduleName::Reset()
{
	Module::Reset();
}

void $moduleName::ProcessFrame()
{
	$function
}

} // namespace mk

