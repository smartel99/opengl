#pragma once
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"

class VertexArray {
private:

public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	void Bind();
};

