#pragma once

#include"Test.h"

namespace test
{
	/*使用指定颜色填充背景*/
	class TestClearColor :public Test
	{
	private:
		float m_ClearColor[4];
		const char* m_Label;
	public:
		TestClearColor();
		~TestClearColor();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	};
}