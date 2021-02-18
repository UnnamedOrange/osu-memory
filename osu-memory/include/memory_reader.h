// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <string>
#include <stdexcept>
#include <thread>
#include <optional>
#include <concepts>

#include <Windows.h>
#undef min
#undef max

namespace osu_memory
{
	class open_process_fail : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};

	/// <summary>
	/// Read memory from a process.
	/// Select process by id first, and then you can read the memory.
	/// </summary>
	class memory_reader
	{
	private:
		HANDLE hProcess{};
		HANDLE hEvent_exit{ CreateEventW(nullptr, TRUE, FALSE, nullptr) };

	private:
		std::thread exit_thread;
		void exit_thread_proc();

	public:
		memory_reader() noexcept = default;
		memory_reader(DWORD dwProcessId) : memory_reader()
		{
			select_process(dwProcessId);
		}
		memory_reader(const memory_reader&) = delete;
		memory_reader(memory_reader&&) = delete;
		memory_reader& operator=(const memory_reader&) = delete;
		memory_reader& operator=(memory_reader&&) = delete;
		virtual ~memory_reader()
		{
			detach();
			CloseHandle(hEvent_exit);
		}

	public:
		/// <summary>
		/// Returns whether it is NOT avaliable to read the process memory.
		/// Note that the function returning false doesn't mean reading memory always succeeds.
		/// It may still fail because of the process status.
		/// Namely, the process may have ended while empty() is false.
		/// <remarks>See try_detach.</remarks>
		/// </summary>
		bool empty() const;
		/// <summary>
		/// Detach only if the process has been terminated.
		/// If the process hasn't been terminated, the operation won't be done.
		/// It's recommanded that you call this function before any memory reading.
		/// </summary>
		/// <returns>Whether the detach operation is done.</returns>
		bool try_detach();
		/// <summary>
		/// Select a process by its PID to read memory.
		/// If you have already selected a process, the previous one will be abandoned.
		/// If it fails to open the process (PID is invalid, or access denied, etc.),
		/// an open_process_fail will be thrown.
		/// </summary>
		void select_process(DWORD dwProcessId);
		/// <summary>
		/// Forcedly detach. This function will be automatically called when this memory_reader is begin destructed.
		/// </summary>
		/// <returns>Whether the detach is done. Equals to !empty().</returns>
		bool detach();

	public:
		/// <summary>
		/// Read memory from process. The type should be trivial.
		/// Note that the function may fail because of the status of the process.
		/// <remarks>See try_detach</remarks>
		/// </summary>
		/// <param name="p">The remote address.</param>
		/// <returns>An optional object. If the function fails, it is nullopt.</returns>
		template <typename T>
		std::optional<T> read_memory(LPCVOID p)
			requires std::is_trivial_v<T>
		{
			T ret;
			size_t read;
			if (!ReadProcessMemory(hProcess, p, &ret, sizeof(ret), &read) || read != sizeof(ret))
				return std::nullopt;
			return ret;
		}
	};
}