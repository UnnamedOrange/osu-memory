// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#include <osu_memory/imp_base.h>

namespace osu_memory::implementation
{
	std::optional<std::vector<uint8_t>> imp_base::read_memory(const os::process& process, LPCVOID address_start, SIZE_T size)
	{
		HANDLE hProcess = process.native_handle();
		if (!hProcess)
			return std::nullopt;

		std::vector<uint8_t> ret(size);
		SIZE_T read{};
		if (!ReadProcessMemory(hProcess, address_start, ret.data(), size, &read) ||
			read != size)
			return std::nullopt;
		return ret;
	}
	std::optional<PVOID> imp_base::find(const os::process& process, const std::vector<uint8_t> bin, std::string_view mask)
	{
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
}