#pragma once

#include <functional>


namespace jh
{
	class Job
	{
	public:
		Job(CallbackType callback) : m_callback{ callback } {}

		template<typename T, typename Func, typename... Args>
		Job(std::shared_ptr<T> owner, Func memFunc, Args&&... args)
		{
			// 값캡쳐된 변수들의 수정을 위해 mutable 키워드 사용
			m_callback = [owner, memFunc, tup = std::make_tuple(std::forward<Args>(args)...) /*std::tuple<Args...>*/]() mutable
				{
					// apply는 튜플에 들어있는 값들을 함수에 적용시켜주는 기능을 한다. C++17
					std::apply(
						// template T&& 타입 추론과 동일한 형태의 타입 추론.
						[&](auto&& ...params)
						{
							// 인자들을 모두 매개변수로 받는 멤버함수의 호출
							(owner.get()->*memFunc)(std::forward<decltype(params)>(params)...);
						}, tup);

				};
		}

		void Execute()
		{
			m_callback();
		}

	private:
		CallbackType m_callback;
	};

}





