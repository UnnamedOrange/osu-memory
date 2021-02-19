// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <optional>

#include "osu_memory_module_base.hpp"

namespace osu_memory
{
	class osu_memory_module_mode : public osu_memory_module_base
	{
	private:
		inline static const std::vector<BYTE> binary{ 0xEC, 0x57, 0x56, 0x53, 0x3B, 0x0D };
		PVOID base{};

	private:
		virtual bool on_init() override
		{
			auto addressed = find_one(binary);
			if (addressed)
			{
				base = *addressed;
				return true;
			}
			return false;
		}
		virtual void on_reset() override
		{
			base = nullptr;
		}

	public:
		std::optional<int> get_mode()
		{
			if (!before_read())
				return std::nullopt;
			auto addr = read_memory<int32_t>((PVOID)((size_t)base + binary.size()));
			if (!addr)
			{
				reset();
				return std::nullopt;
			}

			return read_memory<int32_t>(reinterpret_cast<PVOID>(addr.value()));
		}
	};
}