#pragma once

#include"Test.h"

namespace test
{
	/*使用指定颜色填充背景*/
	class TestClearColor :public Test
	{
	private:
		const char* m_Label;
		float m_ClearColor[4];

	public:
		TestClearColor();
		~TestClearColor();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	};
}