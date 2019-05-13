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
#include <opencv2/video/background_segm.hpp>

#include <librealsense2/rs.hpp>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#define GLEW_STATIC
#include <gl/glew.h>

#include <GLFW/glfw3.h>
#include "Realsense.h"


#define ASSERT(x) if (!(x)) __debugbreak();

void glfw_error_callback(int error, const char* description);

using namespace cv;

int main(int argc, char** argv) {
	// Initialize GLFW.
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit()) {
		// Initialization failed.
		glfw_error_callback(0, "Couldn't initialize GLFW!");
		exit(EXIT_FAILURE);
	}

	// Decide GL+GLSL versions.
#if __APPLE__
	// GL 3.2 + GLSL 150.
	const char* glsl_version = "#version 150"); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

	// Create window with graphics context.
	GLFWwindow* window = glfwCreateWindow(800, 600, "Eldryn", NULL, NULL);
	if (!window) {
		// Window or context creation failed.
		glfw_error_callback(0, "Couldn't create window or context!");
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	// Initialize OpenGL loader (GLEW).
	bool err = glewInit() != GLEW_OK;
	if (err) {
		fprintf(stderr, "Failed to initialize GLEW OpenGL loader!\n");
		exit(EXIT_FAILURE);
	}

	// Setup Dear ImGui context.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style.
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings.
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	Realsense camera(true);

	while (!glfwWindowShouldClose(window)) {
		// Poll and handle events (inputs, window resize, etc.).
		glfwPollEvents();

		// Start the Dear ImGui frame.
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (camera.get_has_failed()) {
			fprintf(stderr, "Camera has failed!");
			exit(EXIT_FAILURE);
		}
		camera.OnUpdate();
		camera.OnRender();
		camera.OnImGuiRender();

		// Rendering.
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Cleanup.
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error: %s\n", description);
}
