#include "TestRealSense.h"
#include "Renderer.h"

#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

namespace test {
	TestRealSense::TestRealSense()
		: m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
		m_Model(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
		m_DepthScale(0.0f)
	{
		float textured_square_positions[] = {
			//  Xr,     Yr,   Xt,   Yt
			  0.0f,   0.0f, 1.0f, 1.0f,	// 0 Bottom Left.
			960.0f,   0.0f, 0.0f, 1.0f,	// 1 Bottom Right.
			960.0f, 540.0f, 0.0f, 0.0f,	// 2 Top Right.
			  0.0f, 540.0f, 1.0f, 0.0f	// 3 Top Left.
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
			m_Pipe = std::make_unique<rs2::pipeline>();
			m_Profile = m_Pipe->start();

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
				frameset = m_Pipe->wait_for_frames(1000);

			m_Texture = std::make_unique<Texture>(frameset.get_color_frame());
			m_Shader->Bind();
			m_Shader->SetUniform1i("u_Texture", 0);
		}
		catch (const rs2::error & e) {
			Print_Error(e);
			m_HasFailed = true;
		}
	}
	TestRealSense::~TestRealSense()
	{
		try
		{
			m_Pipe->stop();
		}
		catch (const rs2::error & e)
		{

		}
	}
	void TestRealSense::OnUpdate(float deltaTime)
	{
		rs2::frameset frames;
		if (m_Pipe->poll_for_frames(&frames))
		{
			//rs2::colorizer color_map;
			//rs2::depth_frame df = frames.get_depth_frame();
			//rs2::video_frame vf = frames.get_color_frame();

			//// for each row in the depth image.
			//	// for each pixel in the row
			//		// get depth value of the pixel
			//		// if the pixel is the closest one we've seen so far
			//			// save it's value and position (X, Y)
			//// draw a red circle over the closest lowest pixel.
			//// The pixel we want is the one that corresponds to the tip of a finger near the wall,
			//// and the way the camera is installed (i.e. face down, on top of the wall),
			//// the wall corresponds to the bottom of the image (it's x value is the highest).
			//const uint16_t* p_df_data = reinterpret_cast<const uint16_t*>(df.get_data());
			//uint8_t* vf_data = reinterpret_cast<uint8_t*>(const_cast<void*>(vf.get_data()));
			//int width = df.get_width();
			//int height = df.get_height();
			//int BPP = vf.get_bytes_per_pixel();

			//const int SlotSizeFactor = 5;
			//const int NoOfSlot = 65536 >> SlotSizeFactor;
			//uint16_t SlotCounts[NoOfSlot];

			//// Clear the depth counters in each slot.
			//for (int i = 0; i < sizeof(SlotCounts) / sizeof(*SlotCounts); i++) {
			//	SlotCounts[i] = 0;
			//}


			//// Categorize each pixels.
			//for (int y = 0; y < height; y++) {
			//	auto depth_pixel_index = y * width;
			//	for (int x = 0; x < width; x++, ++depth_pixel_index) {
			//		// Get the depth value of the current pixel.
			//		auto pixels_distance = p_df_data[depth_pixel_index];

			//		// If invalid value
			//		if (pixels_distance * m_DepthScale <= 0.f || pixels_distance * m_DepthScale > 10.0f)
			//			continue;

			//		// Find the slot and increase the counter.
			//		SlotCounts[pixels_distance >> SlotSizeFactor]++;
			//	}
			//}

			//// Now find the depth with the most pixels.
			//int max_count = 0;
			//int max_pos = 0;
			//for (int i = 0; i < sizeof(SlotCounts) / sizeof(*SlotCounts); i++) {
			//	// Try to remove background and noise.
			//	if (SlotCounts[i] <= 300 || SlotCounts[i] >= 30000) {
			//		/*std::cout << "[" << (i << SlotSizeFactor) * m_DepthScale << ","
			//			<< ((i + 1) << SlotSizeFactor) * m_DepthScale << "[: "
			//			<< SlotCounts[i] << std::endl;*/
			//		SlotCounts[i] = 0;
			//	}
			//	else if (SlotCounts[i] > max_count) {
			//		max_count = SlotCounts[i];
			//		max_pos = i;
			//	}
			//}

			//double x_total = 0, y_total = 0, z_total = 0;
			//for (int y = 0; y < height; y++) {
			//	auto depth_pixel_index = y * width;
			//	for (int x = 0; x < width; x++, ++depth_pixel_index) {
			//		// Get the depth value of the current pixel.
			//		auto pixels_distance = p_df_data[depth_pixel_index];
			//		// Calculate the offset in other frame's buffer to current pixel.
			//		auto offset = depth_pixel_index * vf.get_bytes_per_pixel();
			//		if (pixels_distance >> SlotSizeFactor == max_pos) {
			//			x_total += x;
			//			y_total += y;
			//			z_total += double(pixels_distance) * m_DepthScale;
			//			//std::memset( &p_other_frame[offset], 0x00, other_bpp );
			//		}
			//	}
			//}

			//double x, y, z;

			//if (SlotCounts[max_pos] == 0) {
			//	x = width / 2;
			//	y = height / 2;
			//	z = 0.0f;
			//}
			//else {
			//	x = x_total / SlotCounts[max_pos];
			//	y = y_total / SlotCounts[max_pos];
			//	z = z_total / SlotCounts[max_pos];
			//	std::cout << "Biggest mass of pixels: (" << x << ',' << y << ',' << z << ")         \r";
			//}


			///*std::unique_ptr<Texture> temp = std::make_unique<Texture>(vf);
			//m_Texture.swap(temp);*/
		}

	}
	void TestRealSense::OnRender()
	{
		GLCall(glClearColor(RENDER_COLOR_BLACK));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;

		m_Texture->Bind();
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", m_Proj);

		renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
	}
	void TestRealSense::OnImGuiRender()
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	void TestRealSense::Print_Error(const rs2::error & e)
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
}

