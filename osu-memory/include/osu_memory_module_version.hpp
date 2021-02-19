// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <optional>

#include "osu_memory_module_base.hpp"

namespace osu_memory
{
	class osu_memory_module_version : public osu_memory_module_base
	{
	private:
		inline static const std::vector<BYTE> bin{ 0x4C, 0x00, 0x61, 0x00, 0x73, 0x00, 0x74, 0x00, 0x56, 0x00, 0x65, 0x00, 0x72, 0x00, 0x73, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE4, 0x24, 0x7E, 0x6D };
		PVOID base{};

	private:
		virtual bool on_init() override
		{
			auto addr = find_one(bin, PAGE_READWRITE);
			if (addr)
			{
				base = *addr;
				return true;
			}
			return false;
		}
		virtual void on_reset() override
		{
			base = nullptr;
		}

	public:
		std::optional<std::string> get_version_string()
		{
			if (!before_read())
				return std::nullopt;

			auto length = read_memory<int32_t>(reinterpret_cast<LPCVOID>(reinterpret_cast<size_t>(base) + bin.size()));
			if (!length)
			{
				reset();
				return std::nullopt;
			}

			auto content = read_memory(reinterpret_cast<LPCVOID>(reinterpret_cast<size_t>(base) + bin.size() + sizeof(int32_t)), *length * sizeof(char16_t));
			if (!content)
			{
				reset();
				return std::nullopt;
			}

			std::string ret;
			for (size_t i = 0; i < (*content).size(); i += 2)
				ret.push_back((*content)[i]);
			return ret;
		}
	};
}