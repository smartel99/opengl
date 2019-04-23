#pragma once
#include <vector>
#include <GL/glew.h>
// TODO video at 21:12
struct VertexBufferElement {
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type) {
		switch (type) {
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4;
		case GL_UNSIGNED_BYTE: return 1;
		}
	}
};

class VertexBufferLayout {
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout()
		: m_Stride(0) {}

	template<typename T>
	void Push(int count) {
		static_assert(false);
	}

	template<>
	void Push<float>(int count) {
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += sizeof(GLfloat);
	}
	
	template<>
	void Push<unsigned int>(int count) {
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += sizeof(GLuint);
	}

	template<>
	void Push<unsigned char>(int count) {
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += sizeof(GLbyte);
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const {return m_Stride; }
};

