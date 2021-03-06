// Matlab style plot functions for OpenCV by Changbo (zoccob@gmail).
// plot and label:
//
// template<typename T>
// void plot(const string figure_name, const T* p, int count, int step = 1,
//		     int R = -1, int G = -1, int B = -1);
//
// figure_name: required. multiple calls of this function with same figure_name
//              plots multiple curves on a single graph.
// p          : required. pointer to data.
// count      : required. number of data.
// step       : optional. step between data of two points, default 1.
// R, G,B     : optional. assign a color to the curve.
//              if not assigned, the curve will be assigned a unique color automatically.
//
// void label(string lbl):
//
// label the most recently added curve with lbl.
//
//
// https://code.google.com/p/cvplot/
// Available under the Apache License 2.0
//
// Adaptations by L.Winkler

#ifndef CV_PLOT_H
#define CV_PLOT_H

#if WIN32
#define snprintf sprintf_s
#endif

#include <vector>
#include <opencv2/opencv.hpp>

namespace CvPlot
{
// A curve.
class Series
{
public:
	// name of the curve
	std::string label;

	// allow automatic curve color
	bool auto_color;
	cv::Scalar color;

	Series(const Series& s);
	explicit Series(const std::vector<float>& rx_data);

	// release memory
	void Clear();

	void SetColor(cv::Scalar color, bool auto_color = true);

	const std::vector<float>& data;
};

// a figure comprises of several curves
class Figure
{
private:
	// window name
	std::string figure_name;
	cv::Size figure_size;

	// margin size
	int    border_size;

	// cv::Scalar background_color;
	cv::Scalar axis_color;
	cv::Scalar text_color;

	// several curves
	std::vector<Series> plots;

	// manual or automatic range
	bool custom_range_y = false;
	float y_max = 0;
	float y_min = 0;

	float y_scale = 0;

	bool custom_range_x = false;
	float x_max = 0;
	float x_min = 0;

	float x_scale = 0;

	// automatically change color for each curve
	size_t color_index = 0;

public:
	explicit Figure(const std::string& name);
	~Figure();

	inline std::string GetFigureName() const {return figure_name;}
	Series* Add(const Series &s);
	void Clear();
	void DrawLabels(cv::Mat &output, int posx, int posy);

	// show plot window
	void Show();

	// private:
	Figure();
	void DrawAxis(cv::Mat& output);
	void DrawPlots(cv::Mat& output);

	// call before plot
	void Initialize();
	cv::Scalar GetAutoColor();

};

// manage plot windows
/*
class PlotManager
{
private:
	std::vector<Figure> figure_list;
	Series *active_series;
	Figure *active_figure;

public:

	// now useless
	bool HasFigure(std::string wnd);
	Figure* FindFigure(std::string wnd);

	void Plot(const std::string figure_name, const float* p, int count, int step,
			  int R, int G, int B);

	void Label(std::string lbl);

};
*/

// handle different data types; static mathods;

template<typename T>
void plot(const std::string& figure_name, const T* p, int count, int step = 1,
		  int R = -1, int G = -1, int B = -1);
void clear(const std::string& figure_name);

void label(std::string lbl);

}; // namespace CvPlot
#endif
