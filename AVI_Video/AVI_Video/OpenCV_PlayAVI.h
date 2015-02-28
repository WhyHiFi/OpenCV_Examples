#ifndef OPENCV_PLAYAVI_H
#define OPENCV_PLAYAVI_H
// Info: OpenCV1 stuff; is going to be depricated!
namespace OpenCV {
	// args: argv[1] = path and name of the AVI-File
	// returns 0==failure; 1==success
	int PlayAVI_RunWithSlider(int argc, char** argv);
}
#endif //OPENCV_PLAYAVI_H