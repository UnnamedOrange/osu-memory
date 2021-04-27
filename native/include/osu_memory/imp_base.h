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
		static std::optional<trivial_t> read_memory(const os::process& process, LPCVOID address_start)
		{
			static_assert(std::is_trivial_v<trivial_t>, "trivial_t must be trivial.");
			static_assert(!std::is_pointer_v<trivial_t>, "trivial_t cannot be a pointer type");

			HANDLE hProcess = process.native_handle();
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
		bool construct(const os::process& process);
	private:
		virtual void on_reset() = 0;
	protected:
		void reset();

	private:
		std::any package;
		int status{0}; // 0 for ready, 1 for work, -1 for exit.
		std::timed_mutex mutex_status;
		std::condition_variable_any cv_status;
		std::function<std::any()> commited_func;
		std::thread thread_worker;
	private:
		void worker()
		{
			while (true)
			{
				std::unique_lock lock(mutex_status);
				cv_status.wait(lock, [this]
					{
						return status;
					});
				if (!~status)
					break;

				package = commited_func();
				
				if (~status)
					status = 0;
			}
		}
	protected:
		bool busy()
		{
			return status;
		}
		const std::any get_package() const
		{
			return package;
		}
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
				std::unique_lock lock(mutex_status);
				status = 1;
				commited_func = [f]() -> std::any { return f(); };
				lock.unlock();
				cv_status.notify_one();

				if (lock.try_lock_for(async_timeout) && !status)
					return true;
				else
					return false;
			}
		}

	public:
		imp_base() : thread_worker(&imp_base::worker, this)
		{

		}
		virtual ~imp_base()
		{
			std::lock_guard _(mutex_status);
			status = -1;
			cv_status.notify_one();
		}
		imp_base(const imp_base&) = delete;
		imp_base(imp_base&&) noexcept = delete;
		imp_base& operator=(const imp_base&) = delete;
		imp_base& operator=(imp_base&&) noexcept = delete;
	};
}