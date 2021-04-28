// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <type_traits>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <any>

#include <osu_memory/process.h>

namespace osu_memory::implementation
{
	class imp_base
	{
	public:
		/// <summary>
		/// Read memory from the specific process.
		/// If it fails, the return value is std::nullopt.
		/// </summary>
		template <typename trivial_t>
		static std::optional<trivial_t> read_memory(HANDLE hProcess, LPCVOID address_start)
		{
			static_assert(std::is_trivial_v<trivial_t>, "trivial_t must be trivial.");
			static_assert(!std::is_pointer_v<trivial_t>, "trivial_t cannot be a pointer type");

			if (!hProcess)
				return std::nullopt;
			trivial_t ret{};
			SIZE_T read{};
			if (!ReadProcessMemory(hProcess, address_start, &ret, sizeof(ret), &read) ||
				read != sizeof(ret))
				return std::nullopt;
			return ret;
		}
		/// <summary>
		/// Read memory from the specific process.
		/// If it fails, the return value is std::nullopt.
		/// </summary>
		template <typename trivial_t>
		static std::optional<trivial_t> read_memory(const os::process& process, LPCVOID address_start)
		{
			return read_memory<trivial_t>(process.native_handle(), address_start);
		}
		/// <summary>
		/// Read memory from the specific process.
		/// If it fails, the return value is std::nullopt.
		/// </summary>
		static std::optional<std::vector<uint8_t>> read_memory(HANDLE hProcess, LPCVOID address_start, SIZE_T size);
		/// <summary>
		/// Read memory from the specific process.
		/// If it fails, the return value is std::nullopt.
		/// </summary>
		static std::optional<std::vector<uint8_t>> read_memory(const os::process& process, LPCVOID address_start, SIZE_T size);

	public:
		/// <summary>
		/// Find matched binary from memory.
		/// </summary>
		/// <param name="bin">Binary sequence.</param>
		/// <param name="mask">Mask. '?' means one byte can be any.</param>
		template <typename vector_t>
		std::optional<PVOID> find(const os::process& process, const vector_t& bin, std::string_view mask)
		{
			static_assert(std::is_same_v<typename vector_t::value_type, uint8_t>, "Value type of bin must be uint8_t.");
			if (bin.size() != mask.length())
				throw std::invalid_argument("Sizes of bin and mask must be the same.");

			HANDLE hProcess = process.native_handle();
			if (!hProcess)
				return std::nullopt;

			PVOID min_address;
			PVOID max_address;
			{
				SYSTEM_INFO sys_info;
				GetSystemInfo(&sys_info);
				min_address = sys_info.lpMinimumApplicationAddress;
				max_address = sys_info.lpMaximumApplicationAddress;
			}

			PVOID crt_address = min_address;
			while (crt_address < max_address)
			{
				MEMORY_BASIC_INFORMATION mem_info;
				if (!VirtualQueryEx(hProcess, crt_address, &mem_info, sizeof(mem_info)))
					return std::nullopt;

				if ((mem_info.Protect & PAGE_EXECUTE_READWRITE) && mem_info.State == MEM_COMMIT)
				{
					// Do matches here.
					auto region_data = read_memory(process,
						mem_info.BaseAddress, mem_info.RegionSize);
					if (!region_data)
						continue;

					for (size_t i = 0; i + bin.size() - 1 < (*region_data).size(); i++)
					{
						bool ok = true;
						for (size_t j = 0; j < bin.size(); j++)
							if (!(mask[j] == '?' || (*region_data)[i + j] == bin[j]))
							{
								ok = false;
								break;
							}
						if (ok)
							return PVOID(uintptr_t(mem_info.BaseAddress) + i);
					}

				}
				crt_address = PVOID(uintptr_t(crt_address) + mem_info.RegionSize);
			}
			return std::nullopt;
		}

	private:
		bool is_constructed{};
		os::process binded_process;
	private:
		virtual bool on_construct(const os::process& process) = 0;
	protected:
		/// <summary>
		/// Call this if you want to use your prepared data.
		/// You need to implement on_construct.
		/// </summary>
		/// <param name="process"></param>
		/// <returns></returns>
		bool construct(const os::process& process);
	private:
		virtual void on_reset() = 0;
	protected:
		/// <summary>
		/// Call this if you find your data outdated.
		/// You need to implement on_reset.
		/// It will be called automatically in construct if the process has changed.
		/// </summary>
		void reset();

	private:
		std::any package;
		int status{ 0 }; // 0 for ready, 1 for work, -1 for exit.
		std::timed_mutex mutex_status;
		std::condition_variable_any cv_status;
		std::mutex mutex_finish;
		std::condition_variable cv_finish;
		std::function<std::any()> commited_func;
		std::thread thread_worker;
	private:
		void worker();
	protected:
		/// <summary>
		/// Whether the async workflow is busy. commit fails if is.
		/// </summary>
		/// <returns></returns>
		bool busy() const;
		/// <summary>
		/// Get the result of the workflow. You should use this only if commit returns true.
		/// </summary>
		/// <returns></returns>
		const std::any get_package() const;
		/// <summary>
		/// Commit a task to the workflow.
		/// </summary>
		/// <param name="f">return_t()</param>
		/// <param name="async">Whether the task is done in another thread or in current thread; the latter is faster.</param>
		/// <param name="async_timeout">If async is true, the longest time commit will return. Note the difference between 999us and 1000us; the latter is actually 16ms or so at least. If async is false, this parameter is ignored</param>
		template <typename func_t>
		bool commit(func_t f, bool async, std::chrono::nanoseconds async_timeout)
		{
			if (!async)
			{
				package = f();
				return true;
			}
			else
			{
				if (status)
					return false;
				std::unique_lock lock_notify(mutex_status);
				status = 1;
				commited_func = [f]() -> std::any { return f(); };
				lock_notify.unlock();
				cv_status.notify_one();

				std::unique_lock lock_reveiver(mutex_finish);
				cv_finish.wait_for(lock_reveiver, async_timeout, [this]
					{
						return !status;
					});
				if (!status)
					return true;
				else
					return false;
			}
		}

	public:
		imp_base() : thread_worker(&imp_base::worker, this) {}
		virtual ~imp_base();
		imp_base(const imp_base&) = delete;
		imp_base(imp_base&&) noexcept = delete;
		imp_base& operator=(const imp_base&) = delete;
		imp_base& operator=(imp_base&&) noexcept = delete;
	};
}