#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


/*bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}*/


void displayProperties(VideoCapture& cap)
{
	// Display properties
	cout<<"CV_CAP_PROP_POS_MSEC "<<cap.get(CV_CAP_PROP_POS_MSEC)<<endl;
	cout<<"CV_CAP_PROP_POS_FRAMES "<<cap.get(CV_CAP_PROP_POS_FRAMES)<<endl;
	cout<<"CV_CAP_PROP_POS_AVI_RATIO "<<cap.get(CV_CAP_PROP_POS_AVI_RATIO)<<endl;
	cout<<"CV_CAP_PROP_FRAME_WIDTH "<<cap.get(CV_CAP_PROP_FRAME_WIDTH)<<endl;
	cout<<"CV_CAP_PROP_FRAME_HEIGHT "<<cap.get(CV_CAP_PROP_FRAME_HEIGHT)<<endl;
	cout<<"CV_CAP_PROP_FPS "<<cap.get(CV_CAP_PROP_FPS)<<endl;
	cout<<"CV_CAP_PROP_FOURCC "<<cap.get(CV_CAP_PROP_FOURCC)<<endl;
	cout<<"CV_CAP_PROP_FRAME_COUNT "<<cap.get(CV_CAP_PROP_FRAME_COUNT)<<endl;
	cout<<"CV_CAP_PROP_FORMAT "<<cap.get(CV_CAP_PROP_FORMAT)<<endl;
	cout<<"CV_CAP_PROP_MODE "<<cap.get(CV_CAP_PROP_MODE)<<endl;
	cout<<"CV_CAP_PROP_BRIGHTNESS "<<cap.get(CV_CAP_PROP_BRIGHTNESS)<<endl;
	cout<<"CV_CAP_PROP_CONTRAST "<<cap.get(CV_CAP_PROP_CONTRAST)<<endl;
	cout<<"CV_CAP_PROP_SATURATION "<<cap.get(CV_CAP_PROP_SATURATION)<<endl;
	cout<<"CV_CAP_PROP_HUE "<<cap.get(CV_CAP_PROP_HUE)<<endl;
	cout<<"CV_CAP_PROP_GAIN "<<cap.get(CV_CAP_PROP_GAIN)<<endl;
	cout<<"CV_CAP_PROP_EXPOSURE "<<cap.get(CV_CAP_PROP_EXPOSURE)<<endl;
	cout<<"CV_CAP_PROP_CONVERT_RGB "<<cap.get(CV_CAP_PROP_CONVERT_RGB)<<endl;
	// cout<<"CV_CAP_PROP_WHITE_BALANCE"<<cap.get(CV_CAP_PROP_WHITE_BALANCE)<<endl;
	cout<<"CV_CAP_PROP_RECTIFICATION"<<cap.get(CV_CAP_PROP_RECTIFICATION)<<endl;
}

int main(int argc, char** argv)
{
	assert(argc > 1);

	cout<<"Opening "<<argv[1]<<endl;
	VideoCapture cap(argv[1]); // open the default camera
	if(!cap.isOpened())  // check if we succeeded
	{
		cout<<"Cannot open file"<<endl;
		return -1;
	}


	displayProperties(cap);

	//Mat edges;
	// namedWindow("input",1);
	Mat frame;

	int cpt = 0;

	while(cap.grab())
	{
		cap.retrieve(frame);
		if(cpt%100 == 0)
		{
			cout<<endl;
			cout<<"Capture frame "<<frame.cols<<"x"<<frame.rows<<endl;
			displayProperties(cap);
		}

		// cvtColor(frame, edges, CV_BGR2GRAY);
		// GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
		// Canny(edges, edges, 0, 30, 3);

		// imshow("input", frame);
		// if(waitKey(30) >= 0) break;
		cpt++;
	}
	cout<<endl;
	cout<<"Read "<<cpt<<" frames"<<endl;
	displayProperties(cap);
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}
