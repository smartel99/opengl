// https://docs.opencv.org/2.4/opencv_tutorials.pdf
// https://www.learnopencv.com/object-tracking-using-opencv-cpp-python/
// https://www.intorobotics.com/how-to-detect-and-track-object-with-opencv/
// https://www.learnopencv.com/selective-search-for-object-detection-cpp-python/
// https://docs.opencv.org/3.0-beta/doc/py_tutorials/py_feature2d/py_table_of_contents_feature2d/py_table_of_contents_feature2d.html

#include <sstream>
#include <iostream>
#include <ctime>

#include <opencv2/ximgproc/segmentation.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "librealsense2/rs.hpp"

using namespace cv;
using namespace cv::ximgproc::segmentation;


void PrintRealsenseError(const rs2::error& e);

int main(int argc, char** argv) {
	/// --- REALSENSE INIT ---
	rs2::pipeline pipe;
	rs2::frameset frames;

	rs2::config cfg;
	cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);
	try {
		pipe.start(cfg);
	}
	catch (const rs2::error & e) {
		PrintRealsenseError(e);
		waitKey(0);
		return(-1);
	}
	for (int i = 0; i < 30; i++) {
		// Let auto exposure stabilize.
		frames = pipe.wait_for_frames();
	}
	rs2::video_frame vf = frames.get_color_frame();
	/// --- END OF REALSENSE INIT ---

	/// --- OPENCV INIT ---
	// Speed up using multithreads.
	setUseOptimized(true);
	setNumThreads(12);

	// Create selective search segmentation object using default parameters.
	Ptr<SelectiveSearchSegmentation> ss = createSelectiveSearchSegmentation();

	namedWindow("Tracking", WINDOW_AUTOSIZE);
	/// --- END OF OPENCV INIT ---

	// Number of region proposals to show.
	int numShowRects = 100;
	// Increment to increase/decrease total number of region proposals to be shown.
	int increment = 20;

	Mat im = Mat(Size(640, 480), CV_8UC3, (void*)vf.get_data(), Mat::AUTO_STEP);
	ss->setBaseImage(im);
	ss->switchToSelectiveSearchFast();
	int newHeight = 200;
	int newWidth = im.cols * newHeight / im.rows;

	while (1) {

		if (pipe.poll_for_frames(&frames))
		{
			// Start timer.
			double timer = (double)getTickCount();

			im = Mat(Size(640, 480), CV_8UC3, (void*)frames.get_color_frame().get_data(), Mat::AUTO_STEP);
			// Resize image.
			resize(im, im, Size(newWidth, newHeight));
			ss->setBaseImage(im);
			ss->switchToSelectiveSearchFast();


			// Run selective search segmentation on input image.
			std::vector<Rect> rects;
			ss->process(rects);

			// Calculate FPS.
			float fps = getTickFrequency() / ((double)getTickCount() - timer);
			std::cout << "FPS: " << fps << '\r';

			// Iterate over all the region proposals.
#pragma omp parallel for schedule(dynamic)  // Using OpenMP to try to parallelise the loop.
			for (int i = 0; i < rects.size(); i++) {
				if (i < numShowRects) {
					rectangle(im, rects[i], Scalar(0, 255, 0));
				}
				else
					break;
			}

			// Show output.
			imshow("Tracking", im);

			// Record key press.
			int k = waitKey(1);

			if (k != -1) {
				switch (k) {
					// m is pressed.
				case 109:
					// Increase total number of rectangles to show by increment.
					numShowRects += increment;
					break;
					// l is pressed
				case 108:
					if (numShowRects > increment)
						// Decrease total number of rectangles to show by increment.
						numShowRects -= increment;
					break;
					// q is pressed.
				case 113:
					return(0);
				}
			}
		}
	}
}

void PrintRealsenseError(const rs2::error & e) {
	const char* function = e.get_failed_function().c_str();
	const char* what = e.what();
	const char* type;
	switch (e.get_type())
	{
	case RS2_EXCEPTION_TYPE_BACKEND:
		type = "Back end";
		break;
	case RS2_EXCEPTION_TYPE_CAMERA_DISCONNECTED:
		type = "Camera Disconnected";
		break;
	case RS2_EXCEPTION_TYPE_COUNT:
		type = "Count";
		break;
	case RS2_EXCEPTION_TYPE_DEVICE_IN_RECOVERY_MODE:
		type = "Device in Recovery Mode";
		break;
	case RS2_EXCEPTION_TYPE_INVALID_VALUE:
		type = "Invalid Value";
		break;
	case RS2_EXCEPTION_TYPE_IO:
		type = "IO (What is that even supposed to be?)";
		break;
	case RS2_EXCEPTION_TYPE_NOT_IMPLEMENTED:
		type = "Not Implemented (Oh We ArE iNtEl, OnE oF tHe BiGgEsT tEcH cOmPaNiEs "
			"In ThE wOrLd, YeT wE cAn'T pRoViDe GoOd CoMpLeTe ApIs)";
		break;
	case RS2_EXCEPTION_TYPE_UNKNOWN:
		type = "Unknown (Oof)";
		break;
	case RS2_EXCEPTION_TYPE_WRONG_API_CALL_SEQUENCE:
		type = "Wrong API Call Sequence";
		break;
	default:
		type = "Something probably went really wrong if you see this message";
	}

	std::cout << "An error occurred!: " <<
		"\n\tType: " << type <<
		"\n\tIn Function: " << function <<
		"\n\tError Message: " << what << std::endl;
}