#include "Texture.h"
#include "stb_image/stb_image.h"
#include <iostream>

Texture::Texture()
	:m_RendererID(0), m_FilePath("None"), m_LocalBuffer(nullptr),
	m_Width(512), m_Height(512), m_BPP(0)
{
	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glActiveTexture(GL_TEXTURE0));
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT,
		NULL);
	glBindImageTexture(0, m_RendererID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}
Texture::Texture(const std::string& path)
	: m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), 
	m_Width(0), m_Height(0), m_BPP(0)
{
	// Flips the texture vertically because OpenGL expects textures to start at the bottom left,
	// not the top left. 
	// The (0, 0) point of a texture in OpenGL is at the bottom left,
	// whereas the (0, 0) point of a PNG file is at the top left.
	stbi_set_flip_vertically_on_load(1);

	// Load the image into the local buffer. 4 is the number of channels desired (RGBA in this case)
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	// !!! The next four parameters are mandatory to render the texture properly !!!
	// What to do if the texture is bigger than the render area.
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

	// What to do if the texture is smaller than the render area.
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	// -----------------------------------------------------------------------------

	// Send the texture data to OpenGL.
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
		m_LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}

Texture::Texture(const rs2::video_frame& frame)
	: m_RendererID(0), m_FilePath("Video Frame"), m_LocalBuffer(nullptr),
	m_Width(0), m_Height(0), m_BPP(0)
{
	// Load the image into the local buffer.
	m_LocalBuffer = reinterpret_cast<unsigned char*>(const_cast<void*>(frame.get_data()));
	m_Width = frame.get_width();
	m_Height = frame.get_height();
	m_BPP = frame.get_bytes_per_pixel();
	//std::cout << "Bytes per pixel: " << m_BPP << "\r";
	ASSERT(m_BPP == 3);

	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	// !!! The next four parameters are mandatory to render the texture properly !!!
	// What to do if the texture is bigger than the render area.
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

	// What to do if the texture is smaller than the render area.
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	// -----------------------------------------------------------------------------

	// Send the texture data to OpenGL.
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE,
		m_LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
