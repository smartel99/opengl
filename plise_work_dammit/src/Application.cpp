// http://docs.gl/ 
// OpenGl Documentation.

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "utils/Utils.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"
#include "tests/TestRealSense.h"
#include "tests/TestComputeShader.h"

// --- Function Declaration ---
void window_size_callback(GLFWwindow* window, int width, int height);
// ----------------------------

// --- Global Variable Declaration ---
float gWindowHeight, gWindowWidth;
bool ShouldResize;
// -----------------------------------

int main(void) {
	GLFWwindow* window;
	// Set initial window state.
	gWindowHeight = 540.0f;
	gWindowWidth = 960.0f;
	ShouldResize = false;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(int(gWindowWidth), int(gWindowHeight), "Blinky Recty", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	// Set the callback for window resize.
	glfwSetWindowSizeCallback(window, window_size_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(0);

	if (glewInit() != GLEW_OK)
		std::cout << "Error at glewInit!" << std::endl;

	std::cout << "Using OpenGL Version " << glGetString(GL_VERSION) << std::endl;
	{
		Renderer renderer;

		// Initialize ImGui related stuff.
		const char* glsl_version = "#version 130";
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		test::Test* currentTest = nullptr;
		test::TestMenu* testMenu = new test::TestMenu(currentTest);
		currentTest = testMenu;

		testMenu->RegisterTest<test::TestClearColor>("Clear Color");
		testMenu->RegisterTest<test::TestTexture2D>("2D Texture");
		testMenu->RegisterTest<test::TestRealSense>("RealSense");
		testMenu->RegisterTest<test::TestComputeShader>("Compute Shaders");

		while (!glfwWindowShouldClose(window)) {
			GLCall(glClearColor(RENDER_COLOR_BLACK));
			renderer.Clear();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (currentTest) {
				ImGui::Begin("Test");
				if ((currentTest != testMenu && ImGui::Button("<-") || currentTest->m_HasFailed)) {
					delete currentTest;
					currentTest = testMenu;
				}
				currentTest->OnUpdate(0.0f);
				currentTest->OnRender();
				currentTest->OnImGuiRender();
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);

			glfwPollEvents();
		}
		if (currentTest != testMenu)
			delete testMenu;
		delete currentTest;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}



void window_size_callback(GLFWwindow * window, int width, int height) {
	glViewport(0, 0, width, height);
	gWindowHeight = float(height);
	gWindowWidth = float(width);
	ShouldResize = true;
}
