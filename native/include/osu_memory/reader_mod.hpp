// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <array>
#include <string_view>

#include <osu_memory/reader_base.hpp>
#include <osu_memory/reader_base_rulesets.hpp>
#include <osu_memory/to_array.hpp>
#include <osu_memory/reader_utils_trace.hpp>

namespace osu_memory
{
	enum class mod_t : uint32_t
	{
		no_fail = 0x1,
		easy = 0x2,
		touch_device = 0x4,
		hidden = 0x8,
		hard_rock = 0x10,
		sudden_death = 0x20,
		double_time = 0x40,
		relax = 0x80,
		half_time = 0x100,
		nightcore = 0x200, // Always with double_time.
		flashlight = 0x400,
		auto_play = 0x800,
		spun_out = 0x1000,
		auto_pilot = 0x2000,
		perfect = 0x4000, // Always with sudden_death.
		key4 = 0x8000,
		key5 = 0x10000,
		key6 = 0x20000,
		key7 = 0x40000,
		key8 = 0x80000,
		fade_in = 0x100000,
		random = 0x200000,
		cinema = 0x400000, // Always with auto_play.
		target_practice = 0x800000,
		key9 = 0x1000000,
		co_op = 0x2000000,
		key1 = 0x4000000,
		key3 = 0x8000000,
		key2 = 0x10000000,
		score_v2 = 0x20000000,
		mirror = 0x40000000,
	};
	inline constexpr bool operator&(mod_t a, mod_t b)
	{
		return static_cast<bool>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
	}
	inline constexpr mod_t operator|(mod_t a, mod_t b)
	{
		return static_cast<mod_t>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	/// <summary>
	/// mod.
	/// @NotThreadSafe.
	/// </summary>
	class reader_mod : protected reader_base_rulesets
	{
	private:
		static constexpr auto offsets = utils::trace(std::to_array<int32_t>({ -0xB, 0x4, 0x60, 0x38, 0x1C }));
		static constexpr int32_t last_offset = 0x8;

	public:
		std::optional<mod_t> get_mod()
		{
			if (!call_before())
				return std::nullopt;
			auto addr_last = offsets.from(process, rulesets);
			if (!addr_last)
				return std::nullopt;
			auto mod_2 = process.read_memory<uint64_t>(*addr_last + last_offset);
			if (!mod_2)
				return std::nullopt;
			return static_cast<mod_t>(static_cast<uint32_t>(*mod_2) ^ static_cast<uint32_t>((*mod_2) >> 32));
		}
	};
}