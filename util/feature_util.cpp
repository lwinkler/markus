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

#include "feature_util.h"
#include "MkException.h"

using namespace std;
using namespace cv;

/* -------------------------------------------------------------------------------- */

void randomize(KeyPoint& xr_val, unsigned int& xr_seed)
{
	randomize(xr_val.angle, xr_seed);
	randomize(xr_val.class_id, xr_seed);
	randomize(xr_val.octave, xr_seed);
	randomize(xr_val.pt.x, xr_seed);
	randomize(xr_val.pt.y, xr_seed);
	randomize(xr_val.response, xr_seed);
	randomize(xr_val.size, xr_seed);
}
/* -------------------------------------------------------------------------------- */

void randomize(Point2f& xr_val, unsigned int& xr_seed)
{
	randomize(xr_val.x, xr_seed);
	randomize(xr_val.y, xr_seed);
}

/* -------------------------------------------------------------------------------- */

void randomize(Point2d& xr_val, unsigned int& xr_seed)
{
	randomize(xr_val.x, xr_seed);
	randomize(xr_val.y, xr_seed);
}

/* -------------------------------------------------------------------------------- */

void randomize(Point3f& xr_val, unsigned int& xr_seed)
{
	randomize(xr_val.x, xr_seed);
	randomize(xr_val.y, xr_seed);
	randomize(xr_val.z, xr_seed);
}
/* -------------------------------------------------------------------------------- */
// note: Support for FeatureMat is only partially implemented

// ostream& serialize(ostream& x_out, const Mat& x_value)
// {
// assert(x_value.type() == CV_8UC1); // Only type supported so far
// x_out << "\"rows\":"   << x_value.rows;
// x_out << ",\"cols\":"  << x_value.cols;
// x_out << ",\"depth\":" << x_value.type();
// x_out << ",\"data\":";
//
// x_out<<'[';
// for(int i = 0 ; i < x_value.rows ; i++)
// {
// x_out << "[";
// int j = 0;
// for( ; j < x_value.cols - 1 ; j++)
// x_out << static_cast<int>(x_value.at<uchar>(i,j,0)) << ",";
// if(j == x_value.cols - 1)
// x_out << static_cast<int>(x_value.at<uchar>(i,j,0));
//
// if(i == x_value.rows - 1)
// x_out<<"]";
// else
// x_out<<"],";
// }
// x_out<<']';
// return x_out;
// }


// istream& deserialize(istream& x_in,  Mat& xr_val)
// {
// /*
// int cols = 0;
// int rows = 0;
// int type = 0;
// x_in >> "\"rows\":"   >> rows;
// x_in >> ",\"cols\":"  >> cols;
// x_in >> ",\"depth\":" >> type;
// x_in >> ",\"data\":";
// x_in >> get_char<'['>;
// for(int i = 0 ; i < xr_val.rows ; i++)
// {
// get_char<'['>;
// for(int j = 0 ; j < xr_val.cols ; j++)
// >> xr_val.at(i, j)
// >> get_char<','> >> xr_val.z >> get_char<']'>)
// return x_in;
// else throw MkException("Error in format", LOC);
// */
// assert(false);
// return x_in;
// }
//
// void randomize(Mat& xr_val, unsigned int& xr_seed)
// {
// xr_val = Mat::ones(12, 5, CV_8UC1);
// randu(xr_val, Mat::zeros(1, 1, CV_8UC1), 255 * Mat::ones(1, 1, CV_8UC1));
// cout<<xr_val<<endl;
// }
//
// double compareSquared(const Mat& x_1, const Mat& x_2)
// {
// Mat temp;
// absdiff(x_1, x_2, temp);
// Scalar rgb = mean(temp); // compute a mean for each channel (from 1 to 4)
// double sum = 0;
// for(int c = 0; c < x_1.channels(); c++)
// {
// sum += rgb[c];
// }
// return sum / x_1.channels();
// }
//

