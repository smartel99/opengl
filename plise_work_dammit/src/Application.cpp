// http://docs.gl/ 
// OpenGl Documentation.


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>

#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

//#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

// --- Function Declaration ---
void HSVtoRGB(int H, float S, float V, float* r, float* g, float* b);
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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "Error at glewInit!" << std::endl;

	std::cout << "Using OpenGL Version " << glGetString(GL_VERSION) << std::endl;
	{
		// Create an openGL buffer.
		//float textured_square_positions[] = {
		//	//	            Xr,                        Yr,         Xt,   Yt
		//	(gWindowWidth / 4),       (gWindowHeight / 4),       0.0f, 0.0f,	// 0 Bottom Left.
		//	((gWindowWidth / 4) * 3), (gWindowHeight / 4),       1.0f, 0.0f,	// 1 Bottom Right.
		//	((gWindowWidth / 4) * 3), ((gWindowHeight / 4) * 3), 1.0f, 1.0f,	// 2 Top Right.
		//	(gWindowWidth / 4),       ((gWindowHeight / 4) * 3), 0.0f, 1.0f		// 3 Top Left.
		//};
		float textured_square_positions[] = {
			//	Xr,     Yr,   Xt,   Yt
			-50.0f, -50.0f, 0.0f, 0.0f,	// 0 Bottom Left.
			 50.0f, -50.0f, 1.0f, 0.0f,	// 1 Bottom Right.
			 50.0f,  50.0f, 1.0f, 1.0f,	// 2 Top Right.
			-50.0f,  50.0f, 0.0f, 1.0f	// 3 Top Left.
		};

		//float rgb_square_positions[] = {
		//	 (gWindowWidth - 100.0f), (gWindowHeight - 100), 	// 0 Bottom Left.
		//	  gWindowWidth,           (gWindowHeight - 100), 	// 1 Bottom Right.
		//	  gWindowWidth,            gWindowHeight, 			// 2 Top Right.
		//	 (gWindowWidth - 100.0f),  gWindowHeight, 			// 3 Top Left.
		//};

		// The values of this array represent the index of a vector in the position array.
		// No matter what type that is chosen for this array, it must be unsigned.
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0,
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		VertexArray t_va;
		//VertexArray rgb_va;
		VertexBuffer t_vb(textured_square_positions, 4 * 4 * sizeof(float));
		//VertexBuffer rgb_vb(rgb_square_positions, 4 * 2 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		//rgb_va.AddBuffer(rgb_vb, layout);
		layout.Push<float>(2);
		t_va.AddBuffer(t_vb, layout);

		IndexBuffer ib(indices, 6);

		// Create a orthographic projection matrix which is equivalent to the resolution of the 
		// window.
		glm::mat4 proj = glm::ortho(0.0f, gWindowWidth, 0.0f, gWindowHeight, -1.0f, 1.0f);

		// Create a view matrix that will simulate a camera translation of 100 units to the right.
		// Since there is no such thing as a camera per say, me instead move the scene 100 units to
		// the left to accomplish this effect.
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		Shader t_shader("res/shaders/Texture.shader");
		//Shader rgb_shader("res/shaders/Color.shader");

		Texture texture("res/textures/rald.png");
		texture.Bind();						// Bind to the desired texture slot, in this case slot 0.
		t_shader.Bind();
		t_shader.SetUniform1i("u_Texture", 0);// Set the uniform to be the desired texture, in this
											// case the one in texture slot 0.

		t_va.Unbind();
		//rgb_va.Unbind();
		t_vb.Unbind();
		//rgb_vb.Unbind();
		ib.Unbind();
		t_shader.Unbind();
		//rgb_shader.Unbind();

		Renderer renderer;

		// Initialize ImGui related stuff.
		const char* glsl_version = "#version 130";
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		//float sat = 1.0f;	// HSV Saturation.
		//int hue = 0;		// HSV Hue.
		//
		//float r = 0.0f;
		//float g = 0.0f;
		//float b = 0.0f;
		//float a = 1.f;
		//float increment_y = gWindowHeight/100.0f;
		//float increment_x = gWindowWidth/100.f;

		glm::vec3 translationA(gWindowWidth / 2, gWindowHeight / 2, 0);
		glm::vec3 translationB(0, 0, 0);
		float slider_valsA[3] = {
			translationA.x, 
			translationA.y, 
			translationA.z
		};
		float slider_valsB[3] = {
			translationB.x,
			translationB.y,
			translationB.z
		};

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window)) {
			/* Render here */
			renderer.Clear();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (ShouldResize) {
				ShouldResize = false;
				t_vb.UpdateBufferData(textured_square_positions, 4 * 4 * sizeof(float));
				proj = glm::ortho(0.0f, gWindowWidth, 0.0f, gWindowHeight, -1.0f, 1.0f);
			}

			//renderer.Draw(rgb_va, ib, rgb_shader);

			//renderer.Draw(t_va, ib, t_shader);

			{
				// Create a model matrix that will translate the model according to the values of the slider.
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
				// Create a model view projection matrix.
				glm::mat4 mvp = proj * view * model;
				t_shader.Bind();
				t_shader.SetUniformMat4f("u_MVP", mvp);

				renderer.Draw(t_va, ib, t_shader);
			}
			{
				// Create a model matrix that will translate the model according to the values of the slider.
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
				// Create a model view projection matrix.
				glm::mat4 mvp = proj * view * model;
				t_shader.Bind();
				t_shader.SetUniformMat4f("u_MVP", mvp);

				renderer.Draw(t_va, ib, t_shader);
			}

			//rgb_shader.SetUniformMat4f("u_MVP", mvp);
			//rgb_shader.SetUniform4f("u_Color", r, g, b, a);

			//if (hue > 360)
			//	hue = 0;
			//HSVtoRGB(hue, sat, 1.0f, &r, &g, &b);
			//hue++;

			//if (rgb_square_positions[2] >= gWindowWidth) {	// Bottom Right corner is at right edge of the window?
			//	increment_x *= -1;
			//}
			//else if (rgb_square_positions[0] <= 0.0f) {		// Bottom Left corner is at left edge of the window?
			//	increment_x *= -1;
			//}
			//if (rgb_square_positions[5] >= gWindowHeight) {	// Top Right corner is at top edge of the window?
			//	increment_y *= -1;
			//}
			//else if (rgb_square_positions[1] <= 0.0f) {		// Bottom Left corner is at bottom edge of the window?
			//	increment_y *= -1;
			//}
			//for (int i = 0; i < 8; i++) {
			//	if (i % 2) // Y value of the vertex?
			//		rgb_square_positions[i] += increment_y;
			//	else       // X value of the verte.x
			//		rgb_square_positions[i] += increment_x;
			//}
			//rgb_vb.UpdateBufferData(rgb_square_positions, 4 * 2 * sizeof(float));

			{
				// Slider with value between 0.0f and 100% of the width of the window in pixels.
				ImGui::SliderFloat3("TranslationA", slider_valsA, -1.0f, 1.0f);
				translationA.x = slider_valsA[0] * gWindowWidth;
				translationA.y = slider_valsA[1] * gWindowHeight;
				translationA.z = slider_valsA[2];
				ImGui::SliderFloat3("TranslationB", slider_valsB, -1.0f, 1.0f);
				translationB.x = slider_valsB[0] * gWindowWidth;
				translationB.y = slider_valsB[1] * gWindowHeight;
				translationB.z = slider_valsB[2];
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void HSVtoRGB(int H, float S, float V, float* r, float* g, float* b)
{
	float C = S * V;
	float X = C * (1 - float(abs(fmod(H / 60.0, 2) - 1)));
	float m = V - C;
	float Rs, Gs, Bs;

	if (H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;
	}
	else if (H >= 60 && H < 120) {
		Rs = X;
		Gs = C;
		Bs = 0;
	}
	else if (H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;
	}
	else if (H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;
	}
	else if (H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;
	}

	*r = (Rs + m);
	*g = (Gs + m);
	*b = (Bs + m);
}

void window_size_callback(GLFWwindow * window, int width, int height) {
	glViewport(0, 0, width, height);
	gWindowHeight = float(height);
	gWindowWidth = float(width);
	ShouldResize = true;
}
