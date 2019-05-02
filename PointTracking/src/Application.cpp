// https://docs.opencv.org/2.4/opencv_tutorials.pdf

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "librealsense2/rs.hpp"

#include <ctype.h>
#include <time.h>
#include <sstream>
#include <iostream>

//#define DEBUG

//using namespace std;
using namespace cv;

int edgeThresh = 1;
int lowThreshold = 20;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
int scale = 1;
int delta = 0;
int ddepth = CV_16S;
int c;

const char* canny_name = "Canny edge detection";


//double SobelEdgeDetection() {
//
//	Mat grad;
//	const char* window_name = "Sobel Demo - Simple Edge Detector";
//
//#ifdef DEBUG
//	auto start = std::chrono::steady_clock::now();
//#endif
//
//	// Calculate the derivatives in x and y directions using the Sobel function.
//	/// Generate grad_x and grad_y.
//	Mat grad_x, grad_y;
//	Mat abs_grad_x, abs_grad_y;
//
//	// Gradient X.
//	Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
//	convertScaleAbs(grad_x, abs_grad_x);
//
//	// Gradient Y.
//	Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
//	convertScaleAbs(grad_y, abs_grad_y);
//
//	/// Total Gradient (approximate).
//	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
//#ifdef DEBUG
//	auto end = std::chrono::steady_clock::now();
//	auto diff = end - start;
//	return std::chrono::duration<double, std::milli>(diff).count();
//#endif
//#ifndef DEBUG
//	/// Create window.
//	namedWindow(window_name, WINDOW_AUTOSIZE);
//	namedWindow("Original", WINDOW_AUTOSIZE);
//	imshow("Original", src);
//	imshow(window_name, grad);
//	imwrite("res/img/rald_simple_sobel_1.png", grad);
//	waitKey();
//	return 0;
//#endif
//}
//
//double LaplaceEdgeDetection() {
//
//	Mat abs_dst;
//	const char* window_name = "Laplace Demo - Simple Edge Detector";
//
//#ifdef DEBUG
//	auto start = std::chrono::steady_clock::now();
//#endif
//
//	/// Apply Laplace function.
//	Laplacian(src_gray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
//	convertScaleAbs(dst, abs_dst);
//#ifdef DEBUG
//	auto end = std::chrono::steady_clock::now();
//	auto diff = end - start;
//	return std::chrono::duration<double, std::milli>(diff).count();
//#endif
//#ifndef DEBUG
//	/// Create window.
//	namedWindow(window_name, WINDOW_AUTOSIZE);
//	namedWindow("Original", WINDOW_AUTOSIZE);
//	imshow(window_name, abs_dst);
//	imshow("Original", src);
//	imwrite("res/img/Rald-Laplace-simple-1.png", abs_dst);
//	waitKey();
//	return 0;
//#endif
//}

Mat CannyThreshold(Mat src) {
	Mat detected_edges, src_gray, dst;

	/// Convert to gray scale.
	cvtColor(src, src_gray, COLOR_BGR2GRAY);

	/// Reduce noise with a kernel 3x3.
	blur(src_gray, detected_edges, Size(3, 3));
	/// Canny detector
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);

	src.copyTo(dst, detected_edges);
	return dst;
}

cv::Mat frame_to_mat(const rs2::frame& f)
{
	using namespace cv;
	using namespace rs2;

	auto vf = f.as<video_frame>();
	const int w = vf.get_width();
	const int h = vf.get_height();

	if (f.get_profile().format() == RS2_FORMAT_BGR8)
	{
		return Mat(Size(w, h), CV_8UC3, (void*)f.get_data(), Mat::AUTO_STEP);
	}
	else if (f.get_profile().format() == RS2_FORMAT_RGB8)
	{
		auto r = Mat(Size(w, h), CV_8UC3, (void*)f.get_data(), Mat::AUTO_STEP);
		cvtColor(r, r, COLOR_RGB2BGR);
		return r;
	}
	else if (f.get_profile().format() == RS2_FORMAT_Z16)
	{
		return Mat(Size(w, h), CV_16UC1, (void*)f.get_data(), Mat::AUTO_STEP);
	}
	else if (f.get_profile().format() == RS2_FORMAT_Y8)
	{
		return Mat(Size(w, h), CV_8UC1, (void*)f.get_data(), Mat::AUTO_STEP);
	}

	throw std::runtime_error("Frame format is not supported yet!");
}

int main(int argv, char** argc)
{
	rs2::pipeline pipe;

	// Create a configuration for configuring the pipeline with a non default profile.
	rs2::config cfg;

	// Add desired streams to config.
	cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);

	// Instruct pipeline to start streaming with the requested configuration.
	pipe.start(cfg);
	namedWindow("Original", WINDOW_AUTOSIZE);
	namedWindow("Canny", WINDOW_AUTOSIZE);

	// Camera warm up - dropping several first frames to let auto-exposure stabilize.
	rs2::frameset frames;
	for (int i = 0; i < 30; i++) {
		// Wait for all configured streams to produce a frame.
		frames = pipe.wait_for_frames();
	}

	int fps = 0;
	auto time = std::chrono::steady_clock::now();

	while (waitKey(1)==-1) {
		// Get each frame.
		frames = pipe.wait_for_frames();
		rs2::frame color_frame = frames.get_color_frame();

		// Creating OpenCV Matrix from a color image.
		Mat color(Size(640, 480), CV_8UC3, (void*)color_frame.get_data(), Mat::AUTO_STEP);
		Mat lines = CannyThreshold(color);

		fps++;
		if (std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - time).count() >= 1000) {
			time = std::chrono::steady_clock::now();
			std::cout << "FPS: " << fps << "      \r";
			fps = 0;
		}
		// Display in a GUI.
		imshow("Original", color);
		imshow("Canny", lines);
	}



	return 0;
}