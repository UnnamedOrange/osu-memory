// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <optional>

#include "osu_memory_module_base.hpp"

namespace osu_memory
{
	class osu_memory_module_mania_key : public osu_memory_module_base
	{
	private:
		inline static const std::vector<BYTE> bin{ 0x13, 0x00, 0x00, 0x00, 0x4D, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x69, 0x00, 0x61, 0x00, 0x4C, 0x00, 0x61, 0x00, 0x79, 0x00, 0x6F, 0x00, 0x75, 0x00, 0x74, 0x00, 0x53, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x63, 0x00, 0x74, 0x00, 0x65, 0x00, 0x64, 0x00 };
		PVOID base{};
		static constexpr size_t offset_4k = 0x1D0;

	private:
		virtual bool on_init() override
		{
			{
				auto addr = find_one(bin, PAGE_READWRITE);
				if (!addr)
					return false;
				else
					base = reinterpret_cast<PVOID>(reinterpret_cast<size_t>(*addr));
			}
			return true;
		}
		virtual void on_reset() override
		{
			base = nullptr;
		}

	public:
		std::optional<std::vector<int>> get_mania_layout(int key)
		{
			if (!before_read())
				return std::nullopt;

			std::vector<int> ret;
			if (key == 4)
			{
				for (int i = 0; i < key; i++)
				{
					auto t = read_memory<int32_t>(reinterpret_cast<LPCVOID>(reinterpret_cast<size_t>(base) + offset_4k + i * sizeof(int32_t)));
					if (!t)
						return std::nullopt;
					ret.push_back(*t);
				}
				return ret;
			}
			return std::nullopt;
		}
	};
}