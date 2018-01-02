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

#ifndef CONFIGURABLE_H
#define CONFIGURABLE_H

#include <boost/noncopyable.hpp>
#include "config.h"

namespace mk {
class ParameterStructure;

/**
* @brief Parent class for all configurable objects (containing parameters)
*/
class Configurable : boost::noncopyable
{
public:
	explicit Configurable(ParameterStructure& x_param);
	virtual ~Configurable() {}
	virtual void WriteConfig(mkconf& xr_config, bool x_nonDefaultOnly = false) const;
	virtual const ParameterStructure& GetParameters() const {return m_param;}

private:
	ParameterStructure& m_param;
};

} // namespace mk
#endif
