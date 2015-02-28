#include <iostream>
#include <string.h>
#include "OpenCV_FaceDetection.h"
#include "OpenCV_PlayAVI.h"

using namespace std;

// Win - VS13 - project properties:
// cmd-arguments: .\PeaceHeaven480p.avi
// C/C++ include dir: $(OPENCV_DIR)\..\..\include
// Linker-Additional dir: $(OPENCV_DIR)\lib
// environment var: $(OPENCV_DIR) = C:\Program Files\opencv\build\x86\vc12

// info - needed files in dir of executable:
// PeaceHeaven480p.avi
// haarcascade_frontalface_alt.xml
// opencv_world300.dll


int main(int argc, char** argv){
	int res = 0;
	
	res = OpenCV::PlayAVI_RunWithSlider(argc, argv);
	res = OpenCV::FaceDetection_RunWithWebcam();

	if (res != 1){
		std::cout << "Press any key to continue...";
		char in;
		cin >> in;
	}

	return 0;
}
