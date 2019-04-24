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
		float positions[] = {
			//	            Xr,                        Yr,         Xt,   Yt
			(gWindowWidth / 4),       (gWindowHeight / 4),       0.0f, 0.0f,	// 0 Bottom Left.
			((gWindowWidth / 4) * 3), (gWindowHeight / 4),       1.0f, 0.0f,	// 1 Bottom Right.
			((gWindowWidth / 4) * 3), ((gWindowHeight / 4) * 3), 1.0f, 1.0f,	// 2 Top Right.
			(gWindowWidth / 4),       ((gWindowHeight / 4) * 3), 0.0f, 1.0f		// 3 Top Left.
		};

		float positions_2[] = {
			 (gWindowWidth - 100.0f), (gWindowHeight - 100), 	// 0 Bottom Left.
			  gWindowWidth,           (gWindowHeight - 100), 	// 1 Bottom Right.
			  gWindowWidth,            gWindowHeight, 			// 2 Top Right.
			 (gWindowWidth - 100.0f),  gWindowHeight, 			// 3 Top Left.
		};

		// The values of this array represent the index of a vector in the position array.
		// No matter what type that is chosen for this array, it must be unsigned.
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0,
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		VertexArray va;
		VertexArray va2;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));
		VertexBuffer vb2(positions_2, 4 * 2 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		va2.AddBuffer(vb2, layout);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		// Create a orthographic projection matrix which is equivalent to the resolution of the 
		// window.
		glm::mat4 proj = glm::ortho(0.0f, gWindowWidth, 0.0f, gWindowHeight, -1.0f, 1.0f);

		// Create a view matrix that will simulate a camera translation of 100 units to the right.
		// Since there is no such thing as a camera per say, me instead move the scene 100 units to
		// the left to accomplish this effect.
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		// Create a model matrix that will move the model 200 units up and 200 units to the right.
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		// Create a model view projection matrix.
		glm::mat4 mvp = proj * view * model;

		Shader shader("res/shaders/Texture.shader");
		shader.SetUniformMat4f("u_MVP", mvp);
		Shader shader2("res/shaders/Color.shader");
		shader2.SetUniformMat4f("u_MVP", mvp);

		Texture texture("res/textures/rald.png");
		texture.Bind();						// Bind to the desired texture slot, in this case slot 0.
		shader.SetUniform1i("u_Texture", 0);// Set the uniform to be the desired texture, in this
											// case the one in texture slot 0.

		va.Unbind();
		va2.Unbind();
		vb.Unbind();
		vb2.Unbind();
		ib.Unbind();
		shader.Unbind();
		shader2.Unbind();

		Renderer renderer;

		float sat = 1.0f;	// HSV Saturation.
		int hue = 0;		// HSV Hue.

		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 1.f;
		float increment_y = gWindowHeight/100.0f;
		float increment_x = gWindowWidth/100.f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window)) {
			/* Render here */
			renderer.Clear();

			if (ShouldResize) {
				ShouldResize = false;
				// Update projection matrix.
				glm::mat4 mvp = proj * view * model;
				shader.SetUniformMat4f("u_MVP", mvp);
				shader2.SetUniformMat4f("u_MVP", mvp);
				// Update texture size.
				vb.UpdateBufferData(positions, 4 * 4 * sizeof(float));
			}

			renderer.Draw(va2, ib, shader2);
			renderer.Draw(va, ib, shader);

			shader2.SetUniform4f("u_Color", r, g, b, a);
			if (hue > 360)
				hue = 0;
			HSVtoRGB(hue, sat, 1.0f, &r, &g, &b);
			hue++;

			if (positions_2[2] >= gWindowWidth) {	// Bottom Right corner is at right edge of the window?
				increment_x *= -1;
			}
			else if (positions_2[0] <= 0.0f) {		// Bottom Left corner is at left edge of the window?
				increment_x *= -1;
			}
			if (positions_2[5] >= gWindowHeight) {	// Top Right corner is at top edge of the window?
				increment_y *= -1;
			}
			else if (positions_2[1] <= 0.0f) {		// Bottom Left corner is at bottom edge of the window?
				increment_y *= -1;
			}
			for (int i = 0; i < 8; i++) {
				if (i % 2) // Y value of the vertex?
					positions_2[i] += increment_y;
				else       // X value of the verte.x
					positions_2[i] += increment_x;
			}
			vb2.UpdateBufferData(positions_2, 4 * 2 * sizeof(float));

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}
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

void window_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	gWindowHeight = float(height);
	gWindowWidth = float(width);
	ShouldResize = true;
}
