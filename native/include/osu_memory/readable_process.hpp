// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <cstdint>
#include <vector>
#include <type_traits>
#include <optional>

#include <osu_memory/process.hpp>

namespace osu_memory::os
{
	/// <summary>
	/// CRTP reading process memory support.
	/// </summary>
	template <typename T>
	class enable_readable_process
	{
		static_assert(std::is_same_v<decltype(T().native_handle()), process::native_handle_t>,
			"native_handle() of T must be native_handle_t.");

	public:
		virtual ~enable_readable_process() noexcept = default;

	public:
		/// <summary>
		/// Read memory from the specific process.
		/// If it fails, the return value is std::nullopt.
		/// </summary>
		template <typename addr_t, typename preferred_size_t>
		std::optional<std::vector<uint8_t>> read_memory(addr_t address_start, preferred_size_t size) const
		{
			static_assert(sizeof(addr_t) >= 4,
				"A pointer is at least 32 bits.");
			static_assert(sizeof(LPCVOID) >= sizeof(addr_t),
				"addr_t should not be smaller than uintptr_t.");
			static_assert(std::is_integral_v<preferred_size_t> && std::is_unsigned_v<preferred_size_t>,
				"preferred_size_t should be unsigned integer.");

			const T* __this = dynamic_cast<const T*>(this);
			if (__this->empty())
				return std::nullopt;

			std::vector<uint8_t> ret(size);
			SIZE_T read{};
			if (!ReadProcessMemory(__this->native_handle(),
				reinterpret_cast<LPCVOID>(address_start), ret.data(), size, &read) ||
				read != size)
				return std::nullopt;
			return ret;
		}
		/// <summary>
		/// Read memory from the specific process.
		/// If it fails, the return value is std::nullopt.
		/// </summary>
		template <typename trivial_t, typename addr_t>
		std::optional<trivial_t> read_memory(addr_t address_start) const
		{
			static_assert(std::is_trivial_v<trivial_t>,
				"trivial_t must be trivial.");
			static_assert(!std::is_pointer_v<trivial_t>,
				"trivial_t cannot be a pointer type");
			static_assert(sizeof(addr_t) >= 4,
				"A pointer is at least 32 bits.");
			static_assert(sizeof(LPCVOID) >= sizeof(addr_t),
				"addr_t should not be smaller than uintptr_t.");

			const T* __this = dynamic_cast<const T*>(this);
			if (__this->empty())
				return std::nullopt;

			trivial_t ret{};
			SIZE_T read{};
			if (!ReadProcessMemory(__this->native_handle(),
				reinterpret_cast<LPCVOID>(address_start), &ret, sizeof(ret), &read) ||
				read != sizeof(ret))
				return std::nullopt;
			return ret;
		}

	public:
		/// <summary>
		/// Find matched binary from memory.
		/// Only address of the first matched sequence will be returned.
		/// If failed, or no sequence matched, it returns std::nullopt.
		/// </summary>
		/// <param name="bin">Binary sequence.</param>
		/// <param name="mask">Mask. '?' means one byte can be any.</param>
		template <typename vector_t>
		std::optional<uintptr_t> find(const vector_t& bin, std::string_view mask) const
		{
			static_assert(std::is_same_v<typename vector_t::value_type, uint8_t>, "Value type of bin must be uint8_t.");
			if (bin.size() != mask.size())
				throw std::invalid_argument("Sizes of bin and mask must be the same.");

			const T* __this = dynamic_cast<const T*>(this);
			if (__this->empty())
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
				if (!VirtualQueryEx(__this->native_handle(), crt_address, &mem_info, sizeof(mem_info)))
					return std::nullopt;

				if ((mem_info.Protect & PAGE_EXECUTE_READWRITE) && mem_info.State == MEM_COMMIT)
				{
					// Do matches here.
					auto region_data = read_memory(mem_info.BaseAddress, mem_info.RegionSize);
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
							return uintptr_t(mem_info.BaseAddress) + i;
					}

				}
				crt_address = PVOID(uintptr_t(crt_address) + mem_info.RegionSize);
			}
			return std::nullopt;
		}
	};

	template <typename T>
	class make_readable_process : public T, public enable_readable_process<T>
	{
	public:
		using T::T;
		make_readable_process(const T& base) : T(base) {}
		make_readable_process(T&& base) : T(std::move(base)) {}
	};

	/// <summary>
	/// Process object type for Windows. A helper to hold the handle of the process.
	/// Enable reading memory from the process.
	/// @NotThreadSafe.
	/// </summary>
	using readable_process = make_readable_process<process>;
}