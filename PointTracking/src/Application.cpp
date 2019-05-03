// https://docs.opencv.org/2.4/opencv_tutorials.pdf
// https://www.learnopencv.com/object-tracking-using-opencv-cpp-python/

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/core/ocl.hpp"

#include "librealsense2/rs.hpp"

#include <ctype.h>
#include <time.h>
#include <sstream>
#include <iostream>

using namespace cv;
// Global Variables.
Mat src;
Mat hsv;
Mat mask;

int low = 20, up = 20;

// Function Headers.
void Hist_and_Backproj();
void pickPoint(int event, int x, int y, int, void*);


int main(int argv, char** argc)
{
	rs2::pipeline pipe;

	// Create a configuration for configuring the pipeline with a non default profile.
	rs2::config cfg;

	// Add desired streams to config.
	cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);

	// Instruct pipeline to start streaming with the requested configuration.
	pipe.start(cfg);

	// Camera warm up - dropping several first frames to let auto-exposure stabilize.
	rs2::frameset frames;
	for (int i = 0; i < 30; i++) {
		// Wait for all configured streams to produce a frame.
		frames = pipe.wait_for_frames();
	}

	// Tracking init stuff.
	std::string trackerType = "KCF";

	Ptr<Tracker> tracker;

	// GUI stuff.
	namedWindow("Original", WINDOW_AUTOSIZE);
	namedWindow("Mask", WINDOW_AUTOSIZE);
	namedWindow("BackProj", WINDOW_AUTOSIZE);

	// Set track bars for flood fill thresholds.
	createTrackbar("Low thresh", "BackProj", &low, 255, 0);
	createTrackbar("High thresh", "BackProj", &up, 255, 0);
	// Set a mouse callback
	setMouseCallback("BackProj", pickPoint, 0);

	while (waitKey(1) == -1) {
		frames = pipe.wait_for_frames();
		Mat temp = Mat(Size(640, 480), CV_8UC3, (void*)frames.get_color_frame().get_data(), Mat::AUTO_STEP);
		bilateralFilter(temp, src, 5, 10, 2.5);
		cvtColor(src, hsv, COLOR_BGR2HSV);
		imshow("Original", temp);
		Hist_and_Backproj();
	}
	return 0;
}

void pickPoint(int event, int x, int y, int, void*) {
	if (event != EVENT_LBUTTONDOWN)
		return;

	// Fill and get the mask.
	Point seed = Point(x, y);

	int newMaskVal = 255;
	Scalar newVal = Scalar(120, 120, 120);

	int connectivity = 8;
	int flags = connectivity + (newMaskVal << 8) + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;

	Mat mask2 = Mat::zeros(src.rows + 2, src.cols + 2, CV_8U);
	floodFill(src, mask2, seed, newVal, 0, Scalar(low, low, low), Scalar(up, up, up), flags);
	mask = mask2(Range(1, mask2.rows - 1), Range(1, mask2.cols - 1));

	imshow("Mask", mask);

	Hist_and_Backproj();
}

void Hist_and_Backproj() {
	Mat hist;
	int h_bins = 30, s_bins = 32;
	int histSize[] = { h_bins, s_bins };

	float h_range[] = { 0, 180 };
	float s_range[] = { 0, 256 };
	const float* ranges[] = { h_range, s_range };

	int channels[] = { 0, 1 };

	// Get the Histogram and normalize it.
	calcHist(&hsv, 1, channels, mask, hist, 2, histSize, ranges, true, false);

	normalize(hist, hist, 0, 255, NORM_MINMAX, -1, Mat());

	// Get back projection.
	Mat backproj;
	calcBackProject(&hsv, 1, channels, hist, backproj, ranges, 1, true);

	// Draw the backproj.
	imshow("BackProj", backproj);
}
