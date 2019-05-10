#include "Realsense.h"

#include <iostream>

#include "vendor/imgui/imgui.h"

Realsense::Realsense() : m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
m_Model(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
m_DepthScale(0.0f), has_failed(false)
{
	float textured_square_positions[] = {
		//  Xr,     Yr,   Xt,   Yt
		  0.0f,   0.0f, 0.0f, 1.0f,	// 0 Bottom Left.
		960.0f,   0.0f, 1.0f, 1.0f,	// 1 Bottom Right.
		960.0f, 540.0f, 1.0f, 0.0f,	// 2 Top Right.
		  0.0f, 540.0f, 0.0f, 0.0f	// 3 Top Left.
	};

	// The values of this array represent the index of a vector in the position array.
	// No matter what type that is chosen for this array, it must be unsigned.
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	m_Shader = std::make_unique<Shader>("res/shaders/Texture.shader");
	m_VAO = std::make_unique<VertexArray>();
	m_VertexBuffer = std::make_unique<VertexBuffer>(textured_square_positions, 4 * 4 * sizeof(float));
	VertexBufferLayout layout;

	layout.Push<float>(2);
	layout.Push<float>(2);
	m_VAO->AddBuffer(*m_VertexBuffer, layout);

	m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

	// Start first device with its default stream.
	// The function returns the pipeline profile which the pipeline used to start the device.
	try {
		pipe = std::make_unique<rs2::pipeline>();
		m_Profile = pipe->start();

		// Turn the laser off is possible.
		rs2::device selected_device = m_Profile.get_device();
		auto depth_sensor = selected_device.first<rs2::depth_sensor>();

		if (depth_sensor.supports(RS2_OPTION_EMITTER_ENABLED))
			depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 0.f);
		m_DepthScale = depth_sensor.get_depth_scale();
		std::cout << "Depth Scale: " << m_DepthScale << " meters" << std::endl;

		rs2::frameset frameset;

		// Wait for auto exposure to settle.
		for (int i = 0; i < 30; i++)
			frameset = pipe->wait_for_frames(1000);

		Frame = std::make_unique<Texture>(frameset.get_color_frame());
		m_Shader->Bind();
		m_Shader->SetUniform1i("u_Texture", 0);
	}
	catch (const rs2::error & e) {
		Print_Error(e);
		has_failed = true;
	}
}

Realsense::~Realsense()
{
	try
	{
		pipe->stop();
	}
	catch (const rs2::error & e)
	{

	}
}

bool Realsense::OnUpdate()
{
	// Returns true if Frame was updated, false otherwise.
	bool has_updated = false;
	rs2::frameset frames;
	if (pipe->poll_for_frames(&frames)) {
		has_updated = true;

		rs2::video_frame vf = frames.get_color_frame();
		Frame = std::make_unique<Texture>(vf);
	}

	return has_updated;
}

void Realsense::OnRender()
{
	GLCall(glClearColor(RENDER_COLOR_BLACK));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	Renderer r;

	Frame->Bind();
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("u_MVP", m_Proj);

	r.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
}

void Realsense::OnImGuiRender()
{
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, 
		ImGui::GetIO().Framerate);
}

void Realsense::Print_Error(const rs2::error & e)
{
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
