#pragma once
#include <vector>
#include <string>
#include <functional>

#include "Texture.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"

namespace test {
	class Test {
	public:
		Test()
		: m_HasFailed(false)
		{
		}
		virtual ~Test() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}

		bool m_HasFailed;
	};

	class TestMenu :public Test {
	public:
		TestMenu(Test*& currentTestPointer);
		~TestMenu() {}

		void OnImGuiRender() override;

		template<typename T>
		void RegisterTest(const std::string& name) {
			std::cout << "Registering test " << name << std::endl;
			m_Tests.push_back(std::make_pair(name, [](){ return new T(); }));
		}
	private:
		Test*& m_CurrentTest;
		std::vector<std::pair<std::string, std::function<Test* ()>>> m_Tests;
	};
}