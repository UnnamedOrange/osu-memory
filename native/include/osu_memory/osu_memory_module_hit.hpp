// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <cinttypes>
#include <optional>

#include <osu_memory/osu_memory_module_base.hpp>

namespace osu_memory
{
	class osu_memory_module_hit : public osu_memory_module_base
	{
	private:
		inline static const std::vector<BYTE> binary{ 0x7D, 0x15, 0xA1, 000, 000, 000, 000, 0x85, 0xC0 };
		static constexpr std::string_view mask = "xxx????xx";
		inline static const std::vector<INT_PTR> offsets{ -0xB, 0x4, 0x60, 0x38 };
		PVOID base{};

	private:
		virtual bool on_init() override
		{
			auto address = find_one(binary, mask);
			if (address)
			{
				base = *address;
				return true;
			}
			return false;
		}
		virtual void on_reset() override
		{
			base = nullptr;
		}

	private:
		std::optional<int> get_hit(int32_t last_offset)
		{
			if (!before_read())
				return std::nullopt;

			PVOID crt = base;
			for (auto offset : offsets)
			{
				auto addr = read_memory<uint32_t>(reinterpret_cast<PVOID>(reinterpret_cast<INT_PTR>(crt) + offset));
				if (!addr)
					return std::nullopt;
				crt = reinterpret_cast<PVOID>(*addr);
			}

			return read_memory<int16_t>(reinterpret_cast<PVOID>(reinterpret_cast<INT_PTR>(crt) + last_offset));
		}
	public:
		std::optional<int> get_miss()
		{
			return get_hit(0x92);
		}
		std::optional<int> get_50()
		{
			return get_hit(0x8c);
		}
		std::optional<int> get_100()
		{
			return get_hit(0x88);
		}
		std::optional<int> get_200()
		{
			return get_hit(0x90);
		}
		std::optional<int> get_300()
		{
			return get_hit(0x8A);
		}
		std::optional<int> get_perfect()
		{
			return get_hit(0x8E);
		}
	};
}