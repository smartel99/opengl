#include "Realsense.h"

#include <iostream>

#include "vendor/imgui/imgui.h"

Realsense::Realsense(bool d) : m_Proj(glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f)),
m_DepthScale(0.0f), m_has_failed(false), m_debug(d),
m_thresh(20), m_thresh_multiplier(1),
m_detected_zone(0, 0, 0, 0), m_detected_zone_coordinate(-1.f, -1.f, -1.f),
m_align(RS2_STREAM_COLOR), m_emitter_activated(false), m_max_recursion(8),
m_detection_flag(false)
{
	float textured_square_positions[] = {
		//  Xr,     Yr,   Xt,   Yt
		  0.0f,   0.0f, 0.0f, 1.0f,	// 0 Bottom Left.
		640.0f,   0.0f, 1.0f, 1.0f,	// 1 Bottom Right.
		640.0f, 480.0f, 1.0f, 0.0f,	// 2 Top Right.
		  0.0f, 480.0f, 0.0f, 0.0f	// 3 Top Left.
	};

	// The values of this array represent the index of a vector in the position array.
	// No matter what type that is chosen for this array, it must be unsigned.
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	// --- SHADOW REMOVER INIT ---
	m_bg_sub = createBackgroundSubtractorKNN();
	m_bg_sub->setShadowThreshold(0.01f);
	// --- END OF SHADOW REMOVER INIT ---

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
		rs2::config cfg;
		cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_RGB8, 30);
		cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_ANY, 30);
		m_Profile = pipe->start(cfg);

		// Turn the laser off is possible.
		rs2::device selected_device = m_Profile.get_device();
		auto depth_sensor = selected_device.first<rs2::depth_sensor>();

		if (depth_sensor.supports(RS2_OPTION_EMITTER_ENABLED))
			depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 0.f);
		m_DepthScale = depth_sensor.get_depth_scale();

		if (depth_sensor.supports(RS2_OPTION_ENABLE_AUTO_EXPOSURE))
			depth_sensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 1);
		std::cout << "Depth Scale:" << m_DepthScale << std::endl;

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
		m_has_failed = true;
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

		rs2::frameset aligned_frames = m_align.process(frames);

		Mat new_frame = Mat(Size(640, 480), CV_8UC3, (void*)aligned_frames.get_color_frame().get_data(),
			Mat::AUTO_STEP);

		// Remove the shadows.
		Mat foreground;
		m_bg_sub->apply(new_frame, foreground);	// Apply background subtraction.
		blur(foreground, foreground, Size(10, 10), Point(-1, -1));	// Blur the resulting image.
		threshold(foreground, foreground, 200, 255, THRESH_BINARY);	// Remove the shadows.
		imshow("knn", foreground);
		// --- DETECTION ---

		// Recursively find the biggest contour in the frame.
		Rect bc_box = FindBiggestContour(&foreground, Rect(0, 0, 0, 0), 0);

		// If a contour has been found.
		if (bc_box.area() > 0) {
			// Crop the image to only have the region that is interesting for us.
			Mat cropped = new_frame.clone();
			cropped = cropped(bc_box);

			Mat cropped_gray;
			cvtColor(cropped, cropped_gray, COLOR_BGR2GRAY);
			// Find the biggest contour in that cropped region.
			m_detected_zone = FindBiggestContour(&cropped_gray, bc_box, 0);

			// Get (x, y) coordinate of the point.
			getXYPoint(foreground(m_detected_zone), m_detected_zone);
			if (m_detected_zone_coordinate.x >= 0 && m_detected_zone_coordinate.y >= 0) {
				circle(new_frame, Point((int)m_detected_zone_coordinate.x,
					(int)m_detected_zone_coordinate.y), 5, Scalar(0, 0, 255), -1, 8, 0);
				// Draw a rectangle around the contour we found.
				rectangle(new_frame, m_detected_zone, Scalar(0, 255, 0), 1, 8, 0);


				// Get distance (z) to detected point.
				rs2::depth_frame depth_frame = aligned_frames.get_depth_frame();
				Mat mat_depth_frame = Mat(Size(640, 480), CV_16SC1, (void*)depth_frame.get_data(), Mat::AUTO_STEP);
				m_detected_zone_coordinate.z = getZPoint(mat_depth_frame, Point(m_detected_zone_coordinate.x, m_detected_zone_coordinate.y), 1);

				// If everything is ok, a detection is confirmed.
				m_detection_flag = true;
			}
		}
		// If no contours was found
		else {
			// Reset the points.
			m_detected_zone_coordinate = Point3f(-1.f, -1.f, -1.f);
			m_detected_zone = Rect(0, 0, 0, 0);
		}

		Frame = std::make_unique<Texture>(new_frame);
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
	ImGui::Text("Point location (x,y,z): (%f, %f, %f)", m_detected_zone_coordinate.x,
		m_detected_zone_coordinate.y,
		m_detected_zone_coordinate.z);
	ImGui::Checkbox("Debug", &m_debug);
	if (m_debug) {
		if (ImGui::Button("Toggle emitter")) toggleEmitter();
		if (m_emitter_activated)
			ImGui::Text("Emitter Enabled");
		else
			ImGui::Text("Emitter Disabled");
		ImGui::SliderInt("Max Rec", &m_max_recursion, 1, 10);
		if (ImGui::Button("Screenshot"))
			saveFrame();
	}
}

bool Realsense::poll_for_detection(Point3f* var)
{
	if(!m_detection_flag)
		return false;
	else {
		m_detection_flag = false;
		*var = m_detected_zone_coordinate;
		return true;
	}
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

Rect Realsense::FindBiggestContour(const Mat * src, Rect loc, int r) {
	if ((loc.area() <= 10 && loc.area() != 0) || r >= m_max_recursion) {
		return loc;
	}

	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierachy;

	Mat to;	// Threshold Output.

	// Detect edges in the image using threshold.
	threshold(*src, to, (double)m_thresh * ((double)m_thresh_multiplier + r), 255, THRESH_BINARY);
	// Find the contours of the detected edges.
	findContours(to, contours, hierachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	Rect boundRect;
	int biggestContour = GetMaxAreaContourId(contours);
	if (biggestContour != -1) {
		boundRect = boundingRect(Mat(contours[biggestContour]));
		Rect mapped_rect = Rect(loc.x + boundRect.x, loc.y + boundRect.y,
			boundRect.width, boundRect.height);
		if (boundRect.area() > 10) {
			Mat cropped = src->clone();
			cropped = cropped(boundRect);
			mapped_rect = FindBiggestContour(&cropped, mapped_rect, r + 1);
		}
		return mapped_rect;
	}
	else
		return loc;
}

int Realsense::GetMaxAreaContourId(std::vector<std::vector<Point>> contours)
{
	// Threshold for biggest area.
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

void Realsense::getXYPoint(Mat src, Rect loc)
{
	Point origin = loc.tl();	// (0,0) point of the Rect in the frame.

	double tot_x = 0;
	int count_x = 0;

	for (int x = 0; x < src.cols; x++) {
		if (src.data[x] != 0) {
			tot_x += x;
			count_x++;
		}
	}
	m_detected_zone_coordinate.x = origin.x + (tot_x / count_x);
	m_detected_zone_coordinate.y = origin.y;
}

double Realsense::getZPoint(const Mat & src, Point loc, int r)
{
	if (r > 30 || loc.x < 0 || loc.y < 0)
		return -1;
	int count_z = 0;
	double total_z = 0.f;

	// For each pixel in the selected area
	for (int y = loc.y; y < loc.y + r; y++) {
		for (int x = loc.x; x < loc.x + r; x++) {
			// If the pixel has a valid value
			double val = (double)src.at<uint16_t>(Point(x, y)) * m_DepthScale;
			if (val > 0.f) {
				total_z += val;
				count_z++;
			}
		}
	}
	// If no values were found, increase the search area.
	if (count_z == 0) {
		return getZPoint(src, loc, r + 1);
	}

	return total_z / count_z;
}

void Realsense::toggleEmitter()
{
	rs2::device selected_device = m_Profile.get_device();
	auto depth_sensor = selected_device.first<rs2::depth_sensor>();

	m_emitter_activated = ~m_emitter_activated;
	if (m_emitter_activated) {
		if (depth_sensor.supports(RS2_OPTION_EMITTER_ENABLED))
		{
			depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 1.f);
			auto state = depth_sensor.get_option(RS2_OPTION_EMITTER_ENABLED);
			ASSERT(state);
			return;
		}
	}
	else if (!m_emitter_activated) {
		if (depth_sensor.supports(RS2_OPTION_EMITTER_ENABLED))
		{
			depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 0.f);
			auto state = depth_sensor.get_option(RS2_OPTION_EMITTER_ENABLED);
			ASSERT(!state);
			return;
		}
	}
	fprintf(stderr, "Couldn't toggle emitter!");
}

void Realsense::saveFrame()
{
	Mat im = *Frame->GetData();
	imwrite("res/img/screenshot.jpg", im);
}
