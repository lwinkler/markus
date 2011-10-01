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