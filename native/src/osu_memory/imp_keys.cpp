// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <osu_memory/imp_keys.h>

namespace osu_memory::implementation
{
	bool imp_keys::on_construct(const os::process& process)
	{
		auto address = find(process, binary, mask);
		if (address)
		{
			base = *address;
			return true;
		}
		return false;
	}
	void imp_keys::on_reset()
	{
		base = 0;
	}

	std::optional<std::vector<std::pair<int, bool>>> imp_keys::_sync_get_keys(const os::process& process)
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

		auto key_capacity = read_memory<uint32_t>(hProcess, PVOID(crt + 4));
		if (!key_capacity || !*key_capacity || *key_capacity > 32u)
			return std::nullopt;
		std::vector<std::pair<int, bool>> ret;
		for (size_t i = 0; i < *key_capacity; i++)
		{
			auto key_base_addr = read_memory<uint32_t>(hProcess, PVOID(crt + 8 + i * 4));
			if (!key_base_addr || !*key_base_addr)
				break;
			auto key_code = read_memory<int32_t>(hProcess, PVOID(*key_base_addr + 0x30));
			if (!key_code)
				return std::nullopt;
			auto is_key_down = read_memory<uint8_t>(hProcess, PVOID(*key_base_addr + 0x3B));
			if (!is_key_down)
				return std::nullopt;
			ret.emplace_back(*key_code, *is_key_down);
		}
		return ret;
	}

	std::optional<std::vector<std::pair<int, bool>>> imp_keys::get_keys(const os::process& process, bool async, std::chrono::nanoseconds async_timeout)
	{
		if (busy())
			return std::nullopt;
		if (commit(std::bind(&imp_keys::_sync_get_keys, this, std::ref(process)), async, async_timeout))
			return std::any_cast<std::optional<std::vector<std::pair<int, bool>>>>(get_package());
		return std::nullopt;
	}
}