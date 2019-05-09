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
#include <opencv2/video/background_segm.hpp>

#include "librealsense2/rs.hpp"


#define ASSERT(x) if (!(x)) __debugbreak();

using namespace cv;

void PrintRealsenseError(const rs2::error& e);
Rect FindTopMostContour(const Mat* src, const Mat* output, Rect initial_box, int r);
Rect GetTopRect(std::vector<Rect> Rects);
Rect FindBiggestContour(const Mat* src, const Mat* output, Rect initial_box, int r);
int GetMaxAreaContourId(std::vector<std::vector<Point>> contours);
void getXYPoint(Mat src, Rect region, double* x, double* y);

int thresh = 20;
int thresh_multiplier = 1;
int max_recursion = 10;
int frame_threshold = 2;

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
	namedWindow("knn", WINDOW_AUTOSIZE);
	namedWindow("hbp", WINDOW_FREERATIO);
	namedWindow("sbp", WINDOW_FREERATIO);
	namedWindow("vbp", WINDOW_FREERATIO);
	namedWindow("cknn", WINDOW_FREERATIO);

	std::cout << "Settings:\n\t" <<
		"Line Detection Threshold: " << thresh << "\n\t" <<
		"Threshold Multiplier For Recursion: " << thresh_multiplier << "\n\t" <<
		"Maximum Recursion Level: " << max_recursion << "\n\t" <<
		"Frame Detection Threshold: " << frame_threshold << std::endl;
	/// --- END OF OPENCV INIT ---

	/// --- SHADOW REMOVER INIT ---
	Ptr<BackgroundSubtractorKNN> bs_KNN = createBackgroundSubtractorKNN();
	bs_KNN->setShadowThreshold(0.01f);
	/// --- END OF SHADOW REMOVER INIT ---

	Rect TopBox_bc(0, 0, 0, 0);
	Rect PreciseBBox(0, 0, 0, 0);

	while (waitKey(1) == -1) {
		// If a new frame is available
		if (pipe.poll_for_frames(&frames)) {
			// Start timer.
			double timer = (double)getTickCount();

			// Get the newest color frame as a cv::Mat.
			new_image = Mat(Size(640, 480), CV_8UC3, (void*)frames.get_color_frame().get_data(), Mat::AUTO_STEP);

			// Remove the shadows
			Mat knn;
			bs_KNN->apply(new_image, knn);	// Apply Background Subtraction.
			blur(knn, knn, Size(10, 10), Point(-1, -1));	// Blur the resulting image.
			threshold(knn, knn, 200, 255, THRESH_BINARY);// Remove the shadows.
			imshow("knn", knn);	// Show the image.

			// --- DETECTION ---
			Mat output = new_image.clone();	// Duplicate the newest frame.

			// Recursively find the biggest contour in the frame.
			Rect bbox_bc = FindBiggestContour(&knn, &output, Rect(0, 0, 0, 0), 0);

			// If a contour has been found
			if (bbox_bc.area() > 0) {
				TopBox_bc = bbox_bc;
				// Crop the image to only have the region that is interesting for us.
				Mat cropped = new_image.clone();
				cropped = cropped(bbox_bc);
				Mat cg;
				cvtColor(cropped, cg, COLOR_BGR2GRAY);
				// Find the biggest contour in that region.
				PreciseBBox = FindBiggestContour(&cg, &output, bbox_bc, 0);

				// Get (x,y) coordinate of the point.
				double x = 0;
				double y = 0;
				getXYPoint(knn(PreciseBBox), PreciseBBox, &x, &y);
				if (x > 0 && y > 0) {
					std::cout << "Found a point!: (" << x << ", " << y << ")\n";
					circle(output, Point((int)x, (int)y), 5, Scalar(0, 0, 255), -1, 8, 0);
				}
			}
			// If no contour was found
			else {
				// Reset the boundary boxes.
				TopBox_bc = Rect(0, 0, 0, 0);
				PreciseBBox = Rect(0, 0, 0, 0);
			}

			// Draw a rectangle around the contour we found.
			rectangle(output, PreciseBBox, Scalar(0, 255, 0), 1, 8, 0);
			// --- END OF DETECTION ---


			// Calculate FPS.
			float fps = getTickFrequency() / ((double)getTickCount() - timer);
			// Display FPS on frame.
			std::ostringstream oss;
			oss << "FPS : " << (int(fps));
			putText(output, oss.str(), Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

			// Display the image.
			imshow("New", output);
		}
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

void getXYPoint(Mat src, Rect region, double* point_x, double* point_y)
{
	Point p = region.tl();
	double tot_x = 0;
	int count_x = 0;
	for (int x = 0; x < src.cols; x++) {
		if (src.data[x] != 0) {
			tot_x += x;
			count_x++;
		}
	}
	*point_x = p.x + (tot_x / count_x);
	*point_y = p.y;
	return;
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

Rect FindBiggestContour(const Mat * src, const Mat * output, Rect initial_box, int r) {
	if ((initial_box.area() <= 10 && initial_box.area() != 0) || r >= max_recursion) {
		return initial_box;
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
		Rect mapped_rect = Rect(initial_box.x + boundRect.x, initial_box.y + boundRect.y,
			boundRect.width, boundRect.height);
		if (boundRect.area() > 10) {
			Mat cropped = src->clone();
			cropped = cropped(boundRect);
			mapped_rect = FindBiggestContour(&cropped, output, mapped_rect, r + 1);
		}
		return mapped_rect;
	}
	return initial_box;
}

Rect FindTopMostContour(const Mat * src, const Mat * output, Rect initial_box, int r) {
	if ((initial_box.area() <= 10 && r != 0) || r >= max_recursion) {
		return initial_box;
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

	Rect mapped_rect = Rect(initial_box.x + boundRect.x, initial_box.y + boundRect.y,
		boundRect.width, boundRect.height);
	if (boundRect.area() > 10) {
		Mat cropped = src->clone();
		cropped = cropped(boundRect);
		mapped_rect = FindTopMostContour(&cropped, output, mapped_rect, r + 1);
	}
	return mapped_rect;
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