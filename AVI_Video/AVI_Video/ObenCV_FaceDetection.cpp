#include "OpenCV_FaceDetection.h"
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
/* FORWARD DECLARATIONS:
/*************************************************************************/
void detectAndDisplay(Mat& frame, CascadeClassifier& face_cascade, std::string& windowName, int filenumber);

/*************************************************************************/
/* FUNCTION DEFINITIONS:
/*************************************************************************/
int filterSize = 7;
static void FilterSizeCallback(int val, void*){
	if (val % 2 == 0) filterSize += 1;
}

int OpenCV::FaceDetection_RunWithWebcam(){
	// http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html
	// http://docs.opencv.org/trunk/modules/highgui/doc/reading_and_writing_images_and_video.html
	// print header:
	std::cout << "*****************************************\n";
	std::cout << "Current DEMO: FaceDetection_RunWithWebcam\n";
	std::cout << "*****************************************\n";	

	// window names:
	std::string WindowName = "WebCam_Gaussian";
	std::string WindowName2 = "WebCam_Gaussian_Processed";
	std::string WindowName3 = "WebCam_FaceDetection";

	int const cFSP = 30;
	VideoCapture webcam;
	if (!webcam.open(0)){
		std::cout << "Error: " << __FILE__ << " : " << __LINE__ << " could not access webcam: check connection!\n";
	}

	// load face recognition defintions from a xml file:
	// TODO: destinguish between WIN and UNIX for the search path!!!
	std::string face_cascade_name = "C:/Program\ Files/opencv/facialdetect/haarcascade_frontalface_alt.xml";
	CascadeClassifier face_cascade;
	// Load the cascade
	if (!face_cascade.load(face_cascade_name)) {
		face_cascade_name = "haarcascade_frontalface_alt.xml";
		if (!face_cascade.load(face_cascade_name)){
			std::cout << "Error " << __FILE__ << " : " << __LINE__ << " needed File with face recognition definitions : <" << face_cascade_name << " not found in current directory!\n";
			return (-1);
		}
	};

	char c = 0; 
	if (webcam.isOpened()) {
		// create windows:
		namedWindow(WindowName, 1);
		namedWindow(WindowName2, 1);
		
		// define variable to modify the behaviour of the Canny and GaussianBlur by a Trackbar:
		int threshold = 20;
		int const maxThreshold = 40;
		int const maxFilterSize = 15;
		cvCreateTrackbar("Threshhold", WindowName2.c_str(), &threshold, maxThreshold);
		cvCreateTrackbar("FilterSize", WindowName.c_str(), &filterSize, maxFilterSize, (CvTrackbarCallback)FilterSizeCallback);
		FilterSizeCallback(filterSize, 0);	// must be called!!!

		Mat edges;
		std::cout << "Quit with ESC\n";
		while (1) {
			Mat frame;

			// update frame and display it:
			webcam >> frame;

			cvtColor(frame, edges, COLOR_BGR2GRAY);
			GaussianBlur(edges, edges, Size(filterSize, filterSize), 2.5, 2.5);
			Canny(edges, edges, 1, threshold + 5, 3);
			imshow(WindowName2, edges);

			detectAndDisplay(frame.clone(), face_cascade, WindowName3, 1000);

			GaussianBlur(frame, frame, Size(filterSize, filterSize), 2.5, 2.5);
			imshow(WindowName, frame);
			// wait for next sample and user input.
			c = cvWaitKey((int)1000 / cFSP);
			if (c == 27) break;	
		}

		// release memory and destroy all windows:
		cvDestroyWindow(WindowName.c_str());
		cvDestroyWindow(WindowName2.c_str());
	}
	webcam.release();
	if (c == 27) return 1;
	return 0;
}


/*************************************************************************/
/* LOCAL FUNCTION DEFINITIONS:
/*************************************************************************/
// Function detectAndDisplay
void detectAndDisplay(Mat& frame, CascadeClassifier& face_cascade, std::string& windowName, int filenumber) {
	std::vector<Rect> faces;
	Mat frame_gray;
	Mat crop;
	Mat res;
	Mat gray;
	Mat biggest;
	string text;
	stringstream sstm;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	// Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	// Set Region of Interest
	cv::Rect roi_b;
	cv::Rect roi_c;

	size_t ic = 0; // ic is index of current element
	int ac = 0; // ac is area of current element

	size_t ib = 0; // ib is index of biggest element
	int ab = 0; // ab is area of biggest element

	for (ic = 0; ic < faces.size(); ic++) {// Iterate through all current elements (detected faces)

		roi_c.x = faces[ic].x;
		roi_c.y = faces[ic].y;
		roi_c.width = (faces[ic].width);
		roi_c.height = (faces[ic].height);

		ac = roi_c.width * roi_c.height; // Get the area of current element (detected face)

		roi_b.x = faces[ib].x;
		roi_b.y = faces[ib].y;
		roi_b.width = (faces[ib].width);
		roi_b.height = (faces[ib].height);

		ab = roi_b.width * roi_b.height; // Get the area of biggest element, at beginning it is same as "current" element

		if (ac > ab)
		{
			ib = ic;
			roi_b.x = faces[ib].x;
			roi_b.y = faces[ib].y;
			roi_b.width = (faces[ib].width);
			roi_b.height = (faces[ib].height);
			biggest = frame(roi_b);
		}
		else {
			crop = frame(roi_b);
		}

#if 0		
		resize(crop, res, Size(128, 128), 0, 0, INTER_LINEAR); // This will be needed later while saving images
		cvtColor(crop, gray, CV_BGR2GRAY); // Convert cropped image to Grayscale

		// Form a filename
		std::string filename = "";
		stringstream ssfn;
		ssfn << filenumber << ".png";
		filename = ssfn.str();
		filenumber++;

		imwrite(filename, gray);
#endif
		Point pt1(faces[ic].x, faces[ic].y); // Display detected faces on main window - live stream from camera
		Point pt2((faces[ic].x + faces[ic].height), (faces[ic].y + faces[ic].width));

		unsigned color = ic *(255 / faces.size());
		rectangle(frame, pt1, pt2, Scalar(0, color & 0xFF, 0), 2, 8, 0);
	}

	// Show image
	sstm << "Crop area size: " << roi_b.width << "x" << roi_b.height << " Detected faces: " << faces.size();
	text = sstm.str();

	putText(frame, text, cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
	imshow(windowName.c_str(), frame);

	if (!biggest.empty())
	{
		imshow("biggest - detected", biggest);
	}
	else if ((faces.size() == 1) && !crop.empty())
	{
		imshow("biggest - detected", crop);
	}
	else
		destroyWindow("biggest - detected");
}

