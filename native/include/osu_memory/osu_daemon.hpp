// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include <osu_memory/readable_process.hpp>
#include <osu_memory/singleton.hpp>
#include <osu_memory/stamp_recorder.hpp>

namespace osu_memory::os
{
	/// <summary>
	/// A singleton process object. It automatically check the status of osu!.exe.
	/// </summary>
	class osu_daemon final :
		public readable_process,
		public utils::enable_singleton<osu_daemon>,
		public utils::enable_stamp_recorder<true>
	{
		friend class utils::enable_singleton<osu_daemon>;
		osu_daemon() noexcept = default;
		osu_daemon(const osu_daemon&) = delete;
		osu_daemon(osu_daemon&&) = delete;
		osu_daemon& operator=(const osu_daemon&) = delete;
		osu_daemon& operator=(osu_daemon&&) = delete;

	private:
		using readable_process::operator=;

	private:
		std::thread thread_daemon_routine{ &osu_daemon::daemon_routine, this };
		bool exit{};
		std::mutex mutex_cv;
		std::condition_variable cv;
		void daemon_routine()
		{
			using namespace std::literals;
			while (true)
			{
				{
					std::unique_lock lock(mutex_cv);
					if (cv.wait_for(lock, 1s,
						[this]() { return exit; })) // Wait for 1s. If pred is true, exit at once.
					{
						break;
					}
				}

				// Do the routine.
				if (empty()) // If empty, find osu!.
				{
					auto lists = process::open(L"osu!.exe", PROCESS_VM_READ);
					if (lists.size())
					{
						*this = lists[0];
						increase_stamp();
					}
				}
				else // Else, check whether osu! is still alive.
				{
					if (!still_active())
					{
						reset();
						increase_stamp();
					}
				}
			}
		}

	public:
		~osu_daemon()
		{
			{
				std::lock_guard _(mutex_cv);
				exit = true;
			}
			cv.notify_one();
			thread_daemon_routine.join();
		}
	};
}