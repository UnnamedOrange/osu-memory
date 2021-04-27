// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <cstdint>
#include <vector>
#include <type_traits>
#include <optional>

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
		std::optional<PVOID> find(const os::process& process, const std::vector<uint8_t> bin, std::string_view mask);
	};
}