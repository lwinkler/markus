// Matlab style plot functions for OpenCV by Changbo (zoccob@gmail).

#include "cvplot.h"

using namespace cv;

namespace CvPlot
{

const Scalar CV_BLACK = CV_RGB(0,0,0);
const Scalar CV_WHITE = CV_RGB(255,255,255);
const Scalar CV_GREY = CV_RGB(150,150,150);


Series::Series(const vector<float>& rx_data) : data(rx_data)
{
	Clear();
}

Series::Series(const Series& s): label(s.label), auto_color(s.auto_color), color(s.color), data(s.data)
{}

void Series::Clear()
{
	color = CV_BLACK;
	auto_color = true;
	label = "";
}

void Series::SetColor(Scalar x_color, bool x_auto_color)
{
	color = x_color;
	auto_color = x_auto_color;
}

Figure::Figure(const string& name)
{
	figure_name = name;

	custom_range_y = false;
	custom_range_x = false;
	backgroud_color = CV_WHITE;
	axis_color = CV_BLACK;
	text_color = CV_BLACK;

	figure_size = cvSize(600, 200);
	border_size = 30;

	plots.reserve(10);
}

Figure::~Figure()
{
}

Series* Figure::Add(const Series &s)
{
	plots.push_back(s);
	return &(plots.back());
}

void Figure::Clear()
{
      plots.clear();
}

void Figure::Initialize()
{
	color_index = 0;

	// size of the figure
	if (figure_size.width <= border_size * 2 + 100)
		figure_size.width = border_size * 2 + 100;
	if (figure_size.height <= border_size * 2 + 200)
		figure_size.height = border_size * 2 + 200;

	y_max = FLT_MIN;
	y_min = FLT_MAX;

	x_max = 0;
	x_min = 0;

	// find maximum/minimum of axes
	for(auto &plot : plots)
	{
		for(auto v : plot.data)
		{
			if (v < y_min)
				y_min = v;
			if (v > y_max)
				y_max = v;
		}
		if (x_max < plot.data.size())
			x_max = plot.data.size();
	}

	// calculate zoom scale
	// set to 2 if y range is too small
	float y_range = y_max - y_min;
	float eps = 1e-9f;
	if (y_range <= eps)
	{
		y_range = 1;
		y_min = y_max / 2;
		y_max = y_max * 3 / 2;
	}

	x_scale = 1.0f;
	if (x_max - x_min > 1)
		x_scale = static_cast<float>(figure_size.width - border_size * 2) / (x_max - x_min);

	y_scale = static_cast<float>(figure_size.height - border_size * 2) / y_range;
}

Scalar Figure::GetAutoColor()
{
	// 	change color for each curve.
	static const std::vector<Scalar> colors = {
		CV_RGB(200,200,200),	// grey
		CV_RGB(60,60,255),	// light-blue
		CV_RGB(60,255,60),	// light-green
		CV_RGB(255,60,40),	// light-red
		CV_RGB(0,210,210),	// blue-green
		CV_RGB(180,210,0),	// red-green
		CV_RGB(210,0,180),	// red-blue
		CV_RGB(0,0,185),	// dark-blue
		CV_RGB(0,185,0),	// dark-green
		CV_RGB(185,0,0) 	// dark-red
	};

	if(color_index >= colors.size())
	{
		color_index = 0;
		return colors.at(0);
	}
	return colors.at(color_index++);
}

void Figure::DrawAxis(Mat& output)
{
	int bs = border_size;		
	int h = figure_size.height;
	int w = figure_size.width;

	// size of graph
	int gh = h - bs * 2;
	// int gw = w - bs * 2;

	// draw the horizontal and vertical axis
	// let x, y axies cross at zero if possible.
	float y_ref = y_min;
	if ((y_max > 0) && (y_min <= 0))
		y_ref = 0;

	int x_axis_pos = h - bs - cvRound((y_ref - y_min) * y_scale);

	line(output, Point(bs, x_axis_pos), Point(w - bs, x_axis_pos), axis_color);
	line(output, Point(bs, h - bs), Point(bs, h - bs - gh), axis_color);

	// Write the scale of the y axis
	int chw = 6, chh = 10;
	char text[16];

	// y max
	if ((y_max - y_ref) > 0.05 * (y_max - y_min))
	{
		snprintf(text, sizeof(text)-1, "%.1f", y_max);
		putText(output, text, Point(bs / 5, bs - chh / 2), CV_FONT_HERSHEY_PLAIN, 0.6, text_color, 1, CV_AA);
	}
	// y min
	if ((y_ref - y_min) > 0.05 * (y_max - y_min))
	{
		snprintf(text, sizeof(text)-1, "%.1f", y_min);
		putText(output, text, Point(bs / 5, h - bs + chh), CV_FONT_HERSHEY_PLAIN, 0.6, text_color, 1, CV_AA);
	}

	// x axis
	snprintf(text, sizeof(text)-1, "%.1f", y_ref);
	putText(output, text, Point(bs / 5, x_axis_pos + chh / 2), CV_FONT_HERSHEY_PLAIN, 0.6, text_color, 1, CV_AA);

	// Write the scale of the x axis
	snprintf(text, sizeof(text)-1, "%.0f", x_max );
	putText(output, text, Point(w - bs - strlen(text) * chw, x_axis_pos + chh), CV_FONT_HERSHEY_PLAIN, 0.6, text_color, 1, CV_AA);

	// x min
	snprintf(text, sizeof(text)-1, "%.0f", x_min );
	putText(output, text, Point(bs, x_axis_pos + chh), CV_FONT_HERSHEY_PLAIN, 0.6, text_color, 1, CV_AA);
}

void Figure::DrawPlots(Mat &output)
{
	int bs = border_size;		
	int h = figure_size.height;
	// int w = figure_size.width;

	// draw the curves
	for (auto &plot : plots)
	{
		// automatically change curve color
		if (plot.auto_color)
			plot.SetColor(GetAutoColor());

		Point prev_point;
		for (uint i=0; i<plot.data.size(); i++)
		{
			int y = cvRound(( plot.data.at(i) - y_min) * y_scale);
			int x = cvRound((   i  - x_min) * x_scale);
			Point next_point = Point(bs + x, h - (bs + y));
			circle(output, next_point, 1, plot.color, 1);
			
			// draw a line between two points
			if (i >= 1)
				line(output, prev_point, next_point, plot.color, 1, CV_AA);
			prev_point = next_point;
		}
	}
}

void Figure::DrawLabels(Mat &output, int posx, int posy)
{
	// character size
	// int chw = 6;
	int chh = 8;

	for (const auto &plot : plots)
	{
		string lbl = plot.label;
		// draw label if one is available
		if (lbl.length() > 0)
		{
			line(output, Point(posx, posy - chh / 2), Point(posx + 15, posy - chh / 2), plot.color, 2, CV_AA);
			putText(output, lbl.c_str(), Point(posx + 20, posy), CV_FONT_HERSHEY_PLAIN, 0.6, text_color, 1, CV_AA);
			posy += static_cast<int>(chh * 1.5);
		}
	}
}

/*
// whole process of draw a figure.
void Figure::Show()
{
	Initialize();

	IplImage *output = cvCreateImage(figure_size, IPL_DEPTH_8U, 3);
	cvSet(output, backgroud_color, 0);

	DrawAxis(output);

	DrawPlots(output);

	DrawLabels(output, figure_size.width - 100, 10);

	cvShowImage(figure_name.c_str(), output);
	cvWaitKey(1);
	cvReleaseImage(&output);

}



bool PlotManager::HasFigure(string wnd)
{
	return false;	
}

// search a named window, return null if not found.
Figure* PlotManager::FindFigure(string wnd)
{
	for(vector<Figure>::iterator iter = figure_list.begin();
		iter != figure_list.end();
		iter++)
	{
		if (iter->GetFigureName() == wnd)
			return &(*iter);
	}
	return NULL;
}

// plot a new curve, if a figure of the specified figure name already exists,
// the curve will be plot on that figure; if not, a new figure will be created.
void PlotManager::Plot(const string figure_name, const float *p, int count, int step,
					   int R, int G, int B)
{
	if (count < 1)
		return;

	if (step <= 0)
		step = 1;

	// copy data and create a series format.
	float *data_copy = new float[count];

	for (int i = 0; i < count; i++)
		*(data_copy + i) = *(p + step * i);

	Series s;
	s.SetData(count, data_copy);

	if ((R > 0) || (G > 0) || (B > 0))
		s.SetColor(R, G, B, false);

	// search the named window and create one if none was found
	active_figure = FindFigure(figure_name);
	if ( active_figure == NULL)
	{
		Figure new_figure(figure_name);
		figure_list.push_back(new_figure);
		active_figure = FindFigure(figure_name);
		if (active_figure == NULL)
			exit(-1);
	}

	active_series = active_figure->Add(s);
	active_figure->Show();

}

// add a label to the most recently added curve
void PlotManager::Label(string lbl)
{
	if((active_series!=NULL) && (active_figure != NULL))
	{
		active_series->label = lbl;
		active_figure->Show();
	}
}

// plot a new curve, if a figure of the specified figure name already exists,
// the curve will be plot on that figure; if not, a new figure will be created.
// static method
template<typename T>
void plot(const string figure_name, const T* p, int count, int step,
		  int R, int G, int B)
{
	if (step <= 0)
		step = 1;

	float  *data_copy = new float[count * step];

	float   *dst = data_copy;
	const T *src = p;

	for (int i = 0; i < count * step; i++)
	{
		*dst = (float)(*src);
		dst++;
		src++;
	}

	pm.Plot(figure_name, data_copy, count, step, R, G, B);

	delete [] data_copy;
}

// delete all plots on a specified figure
void clear(const string figure_name)
{
	Figure *fig = pm.FindFigure(figure_name);
	if (fig != NULL)
	{
		fig->Clear();	
	}
	
}
// add a label to the most recently added curve
// static method
void label(string lbl)
{
	pm.Label(lbl);
}


template
void plot(const string figure_name, const unsigned char* p, int count, int step,
		  int R, int G, int B);

template
void plot(const string figure_name, const int* p, int count, int step,
		  int R, int G, int B);

template
void plot(const string figure_name, const short* p, int count, int step,
		  int R, int G, int B);

*/
}; // namespace CvPlot
