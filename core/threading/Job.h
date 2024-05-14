#pragma once

#include "Core.h"
#include <thread>
#include "Function.h"
#include <future>
#include "core/utility/Log.h"

namespace HBE {
	template<typename Return>
	class Job {
		std::future<Return> result;
		std::function<void(Return)> finish_callback;
		bool has_callback = false;
		bool running = false;;
	public:
		Return getResult() {
			return result.get();
		}

		bool update() {
			if (isFinish()) {
				onFinish();
				return true;
			}
			return false;
		}

		void wait() {
			result.wait();
		}

		template<typename... Args>
		void run(std::function<void(Args...)> static_function, Args... args) {
			HB_ASSERT(!running, "Trying to start a job already running!");
			running = true;
			this->result = std::async(std::launch::async, static_function, args...);
		}

		template<typename Object, typename ...Args>
		void run(Object *instance, Return(Object::* member_function)(Args...), Args... args) {
			HB_ASSERT(!running, "Trying to start a job already running!");
			running = true;
			this->result = std::async(member_function, instance, args...);
		}

		void setCallback(void(*static_callback)(Return)) {
			has_callback = true;
			finish_callback = std::bind(static_callback);
		}

		template<typename Object>
		void setCallback(Object *instance, void(Object::* member_function)(Return)) {
			has_callback = true;
			finish_callback = Attach(member_function, instance);
		}

		bool isFinish() {
			return result.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
		}

		void onFinish() {
			running = false;
			if (has_callback)
				finish_callback(result.get());
			has_callback = false;
		}

		Job() {}
	};
}

