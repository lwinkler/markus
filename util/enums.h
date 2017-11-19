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

#ifndef MK_UTIL_ENUMS_H
#define MK_UTIL_ENUMS_H

#include <opencv2/core/core.hpp>

namespace mk {


	/// This file lists different enums to be parsed and create ParameterEnumT
	enum CachedState
	{
		NO_CACHE    = 0,
		WRITE_CACHE = 1,
		READ_CACHE  = 2,
		DISABLED    = 3
	};

	
	enum ImageType {
		mk_CV_8UC1 = CV_8UC1,
		mk_CV_8UC2 = CV_8UC2,
		mk_CV_8UC3 = CV_8UC3,
		//map1["_8UC(n)"] = CV_8UC(n);
		mk_CV_8SC1 = CV_8SC1,
		mk_CV_8SC2 = CV_8SC2,
		mk_CV_8SC3 = CV_8SC3,
		mk_CV_8SC4 = CV_8SC4,
		//map1["_8SC(n)"] = CV_8SC(n);
		mk_CV_16UC1 = CV_16UC1,
		mk_CV_16UC2 = CV_16UC2,
		mk_CV_16UC3 = CV_16UC3,
		mk_CV_16UC4 = CV_16UC4,
		//map1["_16UC(n)"] = CV_16UC(n);
		mk_CV_16SC1 = CV_16SC1,
		mk_CV_16SC2 = CV_16SC2,
		mk_CV_16SC3 = CV_16SC3,
		mk_CV_16SC4 = CV_16SC4,
		//map1["_16SC(n)"] = ;
		mk_CV_32SC1 = CV_32SC1,
		mk_CV_32SC2 = CV_32SC2,
		mk_CV_32SC3 = CV_32SC3,
		mk_CV_32SC4 = CV_32SC4,
		//map1["_32SC1"] = CV_32SC1;
		mk_CV_32FC1 = CV_32FC1,
		mk_CV_32FC2 = CV_32FC2,
		mk_CV_32FC3 = CV_32FC3,
		mk_CV_32FC4 = CV_32FC4,
		//map1["_32FC1"] = CV_32FC1;
		mk_CV_64FC1 = CV_64FC1,
		mk_CV_64FC2 = CV_64FC2,
		mk_CV_64FC3 = CV_64FC3,
		mk_CV_64FC4 = CV_64FC4
		//map1["_64FC1"] = CV_64FC1;
	};
}

#endif
