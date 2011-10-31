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

#ifndef UTIL_H
#define UTIL_H

#include <cv.h>
#include <cstdio>
#include <cstdlib>

void convertByte2Float(const IplImage *byte_img,IplImage *float_img);
void convertFloat2Byte(const IplImage *float_img,IplImage *byte_img);

void adjust(const IplImage* im_in, IplImage* im_out, IplImage*& tmp1, IplImage*& tmp2);//, IplImage* tmp2);
void adjustSize(const IplImage* im_in, IplImage* im_out);
void adjustChannels(const IplImage* im_in, IplImage* im_out);

#endif