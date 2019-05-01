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

/// Global variables
Mat src, src_gray, dst;
Mat detected_edges;

int edgeThresh = 1;
int lowThreshold = 6;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
int scale = 1;
int delta = 0;
int ddepth = CV_16S;
int c;

const char* canny_name = "Canny edge detection";


double SobelEdgeDetection() {

	Mat grad;
	const char* window_name = "Sobel Demo - Simple Edge Detector";

#ifdef DEBUG
	auto start = std::chrono::steady_clock::now();
#endif

	// Calculate the derivatives in x and y directions using the Sobel function.
	/// Generate grad_x and grad_y.
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	// Gradient X.
	Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);

	// Gradient Y.
	Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);

	/// Total Gradient (approximate).
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
#ifdef DEBUG
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	return std::chrono::duration<double, std::milli>(diff).count();
#endif
#ifndef DEBUG
	/// Create window.
	namedWindow(window_name, WINDOW_AUTOSIZE);
	namedWindow("Original", WINDOW_AUTOSIZE);
	imshow("Original", src);
	imshow(window_name, grad);
	imwrite("res/img/rald_simple_sobel_1.png", grad);
	waitKey();
	return 0;
#endif
}

double LaplaceEdgeDetection() {

	Mat abs_dst;
	const char* window_name = "Laplace Demo - Simple Edge Detector";

#ifdef DEBUG
	auto start = std::chrono::steady_clock::now();
#endif

	/// Apply Laplace function.
	Laplacian(src_gray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(dst, abs_dst);
#ifdef DEBUG
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	return std::chrono::duration<double, std::milli>(diff).count();
#endif
#ifndef DEBUG
	/// Create window.
	namedWindow(window_name, WINDOW_AUTOSIZE);
	namedWindow("Original", WINDOW_AUTOSIZE);
	imshow(window_name, abs_dst);
	imshow("Original", src);
	imwrite("res/img/Rald-Laplace-simple-1.png", abs_dst);
	waitKey();
	return 0;
#endif
}

double CannyThreshold() {
	Mat abs_dst;
#ifdef DEBUG
	auto start = std::chrono::steady_clock::now();
#endif
	/// Canny detector
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);
	

#ifdef DEBUG
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	return std::chrono::duration<double, std::milli>(diff).count();
#endif
#ifndef DEBUG
	/// Using Canny's output as a mask, we display our result
	dst = Scalar::all(0);
	src.copyTo(dst, detected_edges);
	imshow(canny_name, dst);
	return 0;
#endif
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
	double total_sobel = 0;
	double total_laplace = 0;
	double total_canny = 0;
	int loops = 50;

	rs2::pipeline pipe;
	pipe.start();

	rs2::frameset frameset;

	namedWindow(canny_name, WINDOW_AUTOSIZE);

	while (true) {
		frameset = pipe.wait_for_frames();
		rs2::video_frame vf = frameset.get_color_frame();
		src = frame_to_mat(vf);
		cvtColor(src, src_gray, COLOR_BGR2GRAY);
		CannyThreshold();
	}

	/*/// Load an image.
	src = imread("res/img/rald.png");

	if (!src.data)
		return 0;

	/// Create a matrix of the same type and size as src (for dst).
	dst.create(src.size(), src.type());


	/// Apply a Gaussian Blur to the image to reduce the noise.
	GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);

	/// Convert it to gray.
	cvtColor(src, src_gray, COLOR_BGR2GRAY);

	for (int i = 0; i <= loops; i++) {
		total_sobel += SobelEdgeDetection();
		total_laplace += LaplaceEdgeDetection();
		total_canny += CannyThreshold();
	}
	std::cout << "Average Time for Sobel edge detection: " << total_sobel / loops << " ms" << std::endl;
	std::cout << "Average Time for Laplace edge detection: " << total_laplace / loops << " ms" << std::endl;
	std::cout << "Average Time for Canny edge detection: " << total_canny / loops << " ms" << std::endl;
	while (true);
	return 0;*/
}