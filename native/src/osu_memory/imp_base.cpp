﻿// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
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

	bool imp_base::construct(const os::process& process)
	{
		bool process_changed = false;
		if (binded_process != process)
		{
			binded_process = process;
			process_changed = true;
		}
		if (process_changed && !binded_process.empty() || !is_constructed)
		{
			if (on_construct(process))
				is_constructed = true;
			else
				reset();
		}
		else if (binded_process.empty())
			reset();
		return is_constructed;
	}
	void imp_base::reset()
	{
		on_reset();
		binded_process.reset();
		is_constructed = false;
	}
}