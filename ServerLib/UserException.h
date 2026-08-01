#pragma once

namespace custom_exception
{
	// 내 마음대로 예외 정의
	class timer_already_running_exception : public std::exception
	{
		virtual char const* what() const override
		{
			return "timer_already_running_exception";
		}
	};
}
