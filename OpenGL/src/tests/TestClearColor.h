#pragma once

#include"Test.h"

namespace test
{
	/*ʹ��ָ����ɫ��䱳��*/
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