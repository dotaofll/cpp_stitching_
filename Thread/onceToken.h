#pragma once
#include <functional>
#include <type_traits>

using namespace std;

namespace stiching {
	class onceToken {
	public:
		typedef function<void(void)> task;
		onceToken(const task& onConstructed, const task& onDestructed = nullptr) {
			if (onConstructed) {
				onConstructed();
			}
			_onDestructed = onDestructed;
		}
		onceToken(const task& onConstructed, task&& onDestructed) {
			if (onConstructed) {
				onConstructed();
			}
			_onDestructed = std::move(onDestructed);
		}
		~onceToken() {
			if (_onDestructed) {
				_onDestructed();
			}
		}
	private:
		onceToken();
		onceToken(const onceToken&);
		onceToken(onceToken&&);
		onceToken& operator =(const onceToken&);
		onceToken& operator =(onceToken&&);
		task _onDestructed;
	};
}