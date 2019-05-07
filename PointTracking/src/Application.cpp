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
#include <opencv2/tracking.hpp>

#include "librealsense2/rs.hpp"

#define ASSERT(x) if (!(x)) __debugbreak();

using namespace cv;

int GetMaxAreaContourId(std::vector<std::vector<Point>> contours);
void PrintRealsenseError(const rs2::error& e);
void FindBiggestContour(const Mat* src, const Mat* output, Rect inital_box, int r);
void FindTopMostContour(const Mat* src, const Mat* output, Rect initial_box, int r);

int thresh = 20;
int thresh_multiplier = 5;
int max_recursion = 10;


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
	Mat old_image = Mat(Size(640, 480), CV_8UC3, (void*)vf.get_data(), Mat::AUTO_STEP);
	Mat new_image = Mat(Size(640, 480), CV_8UC3, (void*)vf.get_data(), Mat::AUTO_STEP);
	/// --- END OF REALSENSE INIT ---

	/// --- OPENCV INIT ---
	// Speed up using multithreads.
	setUseOptimized(true);
	setNumThreads(12);

	namedWindow("New", WINDOW_AUTOSIZE);
	namedWindow("Thresh", WINDOW_FREERATIO);

	createTrackbar(" Threshold", "New", &thresh, 255, nullptr);
	createTrackbar(" Threshold Multiplier", "New", &thresh_multiplier, 5, nullptr);
	createTrackbar(" Max Recursion", "New", &max_recursion, 10, nullptr);

	/// --- END OF OPENCV INIT ---

	int updateInterval = 0;

	while (waitKey(1) == -1) {
		if (pipe.poll_for_frames(&frames)) {

			// Start timer.
			double timer = (double)getTickCount();

			new_image = Mat(Size(640, 480), CV_8UC3, (void*)frames.get_color_frame().get_data(), Mat::AUTO_STEP);

			// --- DETECTION ---
			Mat diff, diff_gray;
			Mat output = new_image.clone();
			absdiff(new_image, old_image, diff);

			// Convert to gray scale and apply blur.
			cvtColor(diff, diff_gray, COLOR_BGR2GRAY);
			blur(diff_gray, diff_gray, Size(3, 3));

			FindTopMostContour(&diff_gray, &output, Rect(0, 0, 0, 0), 0);

			// --- END OF DETECTION ---

			// Calculate FPS.
			float fps = getTickFrequency() / ((double)getTickCount() - timer);
			// Display FPS on frame.
			std::ostringstream oss;
			oss << "FPS : " << (int(fps));
			putText(output, oss.str(), Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

			imshow("New", output);
			//imshow("Thresh", cropImage);
		}
	}
}

void FindBiggestContour(const Mat * src, const Mat * output, Rect initial_box, int r) {
	if ((initial_box.area() <= 10 && initial_box.area() != 0) || r >= 5) {
		return;
	}

	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;

	Mat threshold_output;

	// Detect edges using Threshold.
	threshold(*src, threshold_output, thresh * (thresh_multiplier + r), 255, THRESH_BINARY);
	// Find contours
	findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	Rect boundRect;
	int biggestContour = GetMaxAreaContourId(contours);
	if (biggestContour != -1) {
		boundRect = boundingRect(Mat(contours[biggestContour]));
		if (boundRect.area() > 10) {
			Rect mapped_rect = Rect(initial_box.x + boundRect.x, initial_box.y + boundRect.y,
				boundRect.width, boundRect.height);
			rectangle(*output, mapped_rect, Scalar(0, 0, 255), 2, 8, 0);
			Mat cropped = src->clone();
			cropped = cropped(boundRect);
			std::ostringstream oss;
			oss << "Rec " << r;
			namedWindow(oss.str(), WINDOW_FREERATIO);
			imshow(oss.str(), cropped);
			FindBiggestContour(&cropped, output, mapped_rect, r + 1);

		}
	}
}

Rect GetTopRect(std::vector<Rect> Rects)
{
	// Threshold
	double maxY = 999999;
	int topRectId = -1;
	for (int j = 0; j < Rects.size(); j++) {
		double y = Rects[j].y;
		if (y < maxY) {
			topRectId = j;
		}
	}
	if (topRectId == -1)
		return Rect(0, 0, 0, 0);
	else
		return Rects[topRectId];
}

void FindTopMostContour(const Mat * src, const Mat * output, Rect initial_box, int r) {
	if ((initial_box.area() <= 10 && initial_box.area() != 0) || r >= 5) {
		return;
	}

	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;

	Mat threshold_output;

	// Detect edges using Threshold.
	threshold(*src, threshold_output, thresh * (thresh_multiplier + r), 255, THRESH_BINARY);
	// Find contours
	findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	std::vector<Rect> boundRects(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		boundRects[i] = boundingRect(Mat(contours[i]));
	}
	Rect boundRect = GetTopRect(boundRects);


	if (boundRect.area() > 10) {
		Rect mapped_rect = Rect(initial_box.x + boundRect.x, initial_box.y + boundRect.y,
			boundRect.width, boundRect.height);
		rectangle(*output, mapped_rect, Scalar(0, 0, 255), 2, 8, 0);
		Mat cropped = src->clone();
		cropped = cropped(boundRect);
		std::ostringstream oss;
		oss << "Rec " << r;
		namedWindow(oss.str(), WINDOW_FREERATIO);
		imshow(oss.str(), cropped);
		FindTopMostContour(&cropped, output, mapped_rect, r + 1);
	}

}

int GetMaxAreaContourId(std::vector<std::vector<Point>> contours) {
	// Threshold
	double maxArea = 40;
	int maxAreaContourId = -1;
	for (int j = 0; j < contours.size(); j++) {
		double newArea = contourArea(contours.at(j));
		if (newArea > maxArea) {
			maxArea = newArea;
			maxAreaContourId = j;
		}
	}
	return maxAreaContourId;
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