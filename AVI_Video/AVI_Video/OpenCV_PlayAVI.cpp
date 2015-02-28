#include "OpenCV_PlayAVI.h"
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include <iostream>
#include <string.h>
#include <opencv2/core/core.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;


/*************************************************************************/
/* FORWARD FUNCTION DEFINTIONS:
/*************************************************************************/
int			g_slider_position = 0;
CvCapture*	g_capture = 0;
int			g_fps = 0;
int			g_current_frame = 0;

void onTrackbarSlide(int pos) {
	g_current_frame = pos*g_fps;
	cvSetCaptureProperty(
		g_capture,
		CV_CAP_PROP_POS_FRAMES,
		pos*g_fps
		);
}

IplImage* CreateSmoothImage(IplImage* in){
	if (in == 0) return 0;
	IplImage* out = cvCreateImage(cvGetSize(in), IPL_DEPTH_8U, 3);
	cvSmooth(in, out, CV_GAUSSIAN, 3, 3);
	return out;
}

IplImage* doPyrDown(IplImage* in) {
	// Best to make sure input image is divisible by two.
	//
	assert(in->width % 2 == 0 && in->height % 2 == 0);
	IplImage* out = cvCreateImage(
		cvSize(in->width / 2, in->height / 2),
		in->depth,
		in->nChannels
		);
	cvPyrDown(in, out);
	return(out);
}


IplImage* doRGB2Gray(IplImage* in){
	IplImage* out = cvCreateImage(cvGetSize(in), IPL_DEPTH_8U, 1);
	cvCvtColor(in, out, CV_RGB2GRAY);
	return out;
}

IplImage* doCanny(IplImage* in, double lowThresh, double highThresh, double aperture) {
	if (in->nChannels != 1) return 0; //Canny only handles gray scale images
	IplImage* out = cvCreateImage(cvGetSize(in), IPL_DEPTH_8U, 1);
	cvCanny(in, out, lowThresh, highThresh, aperture);
	return out;
}


/*************************************************************************/
/* FUNCTION DEFINITIONS:
/*************************************************************************/
int OpenCV::PlayAVI_RunWithSlider(int argc, char** argv) {
	// print header
	std::cout << "***********************************\n";
	std::cout << "Current DEMO: PlayAVI_RunWithSlider\n";
	std::cout << "***********************************\n";	

	std::string WindowName = "AVI_Example";
	std::string WindowName2 = "AVI_Processed";
	std::string SliderName = "Time";

	// start capturing frames from a video file (AVI):
	g_capture = cvCreateFileCapture(argv[1]);
	if (g_capture == 0){
		std::cout << "Error " << __FILE__ << " : " << __LINE__ << " - " << argv[1] << " could not be found! \n";
		std::cout << "try: <name>.exe .\PeaceHeaven480p.avi \n";
		return 0;
	}

	// create a window and load video:
	cvNamedWindow(WindowName.c_str(), CV_WINDOW_AUTOSIZE);
	cvNamedWindow(WindowName2.c_str(), CV_WINDOW_AUTOSIZE);
	// get nummer of frames per seconds:
	int frames = (int)cvGetCaptureProperty(g_capture, CV_CAP_PROP_FRAME_COUNT);
	g_fps = (int)cvGetCaptureProperty(g_capture, CV_CAP_PROP_FPS);
	// create a trackbar in the window:
	if (frames){
		cvCreateTrackbar(SliderName.c_str(), WindowName.c_str(), &g_slider_position, frames / g_fps, onTrackbarSlide);
	}

	// define variable to modify the behaviour of the Canny and GaussianBlur by a Trackbar:
	int threshold = 20;
	int const maxThreshold = 70;
	cvCreateTrackbar("high-Threshhold", WindowName2.c_str(), &threshold, maxThreshold);


	int cur_nr_frame = 0;
	char c = 0;
	// capture and show video:
	IplImage* frame;
	std::cout << "Quit with ESC\n";
	while (1) {
		// update frame and display it:
		frame = cvQueryFrame(g_capture);
		if (!frame) break;
		cvShowImage(WindowName.c_str(), frame);


		{// ## START PROCESS IMAGE :

			// creates a new buffer for our image, process it , display it and release buffer again!!!
			IplImage* frame1 = doPyrDown(frame);
			IplImage* gray = doRGB2Gray(frame1);
			IplImage* frame2 = doCanny(gray, 30, threshold + 30, 3);
			cvShowImage(WindowName2.c_str(), frame2);

			cvReleaseImage(&frame1);
			cvReleaseImage(&gray);
			cvReleaseImage(&frame2);

		}// ## END PROCESS IMAGE:


		cur_nr_frame = (int)cvGetCaptureProperty(g_capture, CV_CAP_PROP_POS_FRAMES);
		// update slider position:
		cvSetTrackbarPos(SliderName.c_str(), WindowName.c_str(), cur_nr_frame / g_fps);

		// wait for next sample and user input.
		c = cvWaitKey((int)1000 / g_fps);
		if (c == 27) break;
	}

	// release memory and destroy all windows:
	cvReleaseCapture(&g_capture);
	cvDestroyWindow(SliderName.c_str());
	cvDestroyWindow(WindowName.c_str());
	cvDestroyWindow(WindowName2.c_str());
	if (c == 27) return 1;
	return 0;
}