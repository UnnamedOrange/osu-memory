﻿// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <cinttypes>
#include <vector>
#include <string>
#include <array>
#include <stdexcept>
#include <thread>
#include <chrono>
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
		/// <summary>
		/// Get the time elapsed since the creation of the process.
		/// </summary>
		std::chrono::microseconds get_process_time() const;

	public:
		/// <summary>
		/// Read memory from process. The type should be trivial.
		/// Note that the function may fail because of the status of the process.
		/// <remarks>See try_detach.</remarks>
		/// </summary>
		/// <param name="p">The remote address.</param>
		/// <returns>An optional object. If the function fails, it is nullopt.</returns>
		template <typename T>
		std::optional<T> read_memory(LPCVOID p)
			requires std::is_trivial_v<T>
		{
			T ret;
			SIZE_T read;
			if (!ReadProcessMemory(hProcess, p, &ret, sizeof(ret), &read) || read != sizeof(ret))
				return std::nullopt;
			return ret;
		}
		/// <summary>
		/// Read memory from process.
		/// Note that the function may fail because of the status of the process.
		/// <remarks>See try_detach.</remarks>
		/// </summary>
		/// <param name="p">The remote address.</param>
		/// <param name="size">The size you want to read.</param>
		/// <returns>An optional std::vector(BYTE) object. If the function fails, it is nullopt.</returns>
		std::optional<std::vector<BYTE>> read_memory(LPCVOID p, size_t size)
		{
			std::vector<BYTE> ret(size);
			SIZE_T read;
			if (!ReadProcessMemory(hProcess, p, ret.data(), size, &read) || read != size)
				return std::nullopt;
			return ret;
		}

	private:
		struct memory_region
		{
			PVOID base_address;
			SIZE_T size;
			std::vector<BYTE> reserved; // size() is always 0.
		};
		using dumped_memory_t = std::vector<memory_region>;
		/// <summary>
		/// Get all memory regions.
		/// <remarks>See memory_region.</remarks>
		/// </summary>
		/// <param name="protect">Protect property of the pages.</param>
		/// <returns>An optional dumped_memory_t.</returns>
		std::optional<dumped_memory_t> get_memory_regions(DWORD protect) const;
	public:
		/// <summary>
		/// Find matched binary from memory.
		/// </summary>
		/// <param name="bin">Binary sequence.</param>
		/// <param name="protect">Protect property of the page.</param>
		/// <returns>A head address of matched binary. If there are more than one matched, only the first one will be returned.</returns>
		std::optional<PVOID> find_one(const std::vector<BYTE> bin, DWORD protect = PAGE_EXECUTE_READWRITE);
		/// <summary>
		/// Find matched binary from memory. Protect property of the pages is always PAEG_EXECUTE_READWRITE.
		/// </summary>
		/// <param name="bin">Binary sequence.</param>
		/// <param name="mask">Mask. '?' means one byte can be any.</param>
		/// <returns></returns>
		std::optional<PVOID> find_one(const std::vector<BYTE> bin, std::string_view mask);
	};
}