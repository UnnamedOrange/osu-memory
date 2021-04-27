// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <osu_memory/imp_hits.h>

namespace osu_memory::implementation
{
	bool imp_hits::on_construct(const os::process& process)
	{
		auto address = find(process, binary, mask);
		if (address)
		{
			base = *address;
			return true;
		}
		return false;
	}
	void imp_hits::on_reset()
	{
		base = 0;
	}

	std::optional<int32_t> imp_hits::_sync_get_hit(const os::process& process, intptr_t last_offset)
	{
		if (!construct(process))
			return std::nullopt;

		HANDLE hProcess = process.native_handle();
		int32_t crt = int32_t(intptr_t(base)); // 32-bit osu!
		for (auto offset : offsets)
		{
			auto addr = read_memory<int32_t>(hProcess, PVOID(crt + offset));
			if (!addr)
				return std::nullopt; // No need to reset.
			crt = *addr;
		}
		return read_memory<int16_t>(hProcess, PVOID(crt + last_offset));
	}
	std::optional<int32_t> imp_hits::_sync_get_miss(const os::process& process)
	{
		return _sync_get_hit(process, 0x92);
	}

	std::optional<int32_t> imp_hits::get_miss(const os::process& process, bool async, std::chrono::nanoseconds async_timeout)
	{
		if (busy())
			return std::nullopt;
		if (commit(std::bind(&imp_hits::_sync_get_miss, this, std::ref(process)), async, async_timeout))
			return std::any_cast<std::optional<int32_t>>(get_package());
		return std::nullopt;
	}
}