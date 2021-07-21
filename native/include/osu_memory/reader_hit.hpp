// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <array>
#include <string_view>

#include <osu_memory/reader_base.hpp>
#include <osu_memory/to_array.hpp>
#include <osu_memory/reader_utils_trace.hpp>

namespace osu_memory
{
	/// <summary>
	/// hit_perfect, hit_300, hit_200, hit_100, hit_50, hit_miss.
	/// @NotThreadSafe.
	/// </summary>
	class reader_hit : protected reader_base
	{
	private:
		static constexpr auto sequence = std::to_array<uint8_t>({ 0x7D, 0x15, 0xA1, 000, 000, 000, 000, 0x85, 0xC0 });
		static constexpr auto mask = std::string_view("xxx????xx");
		static constexpr auto offsets = utils::trace(std::to_array<int32_t>({ -0xB, 0x4, 0x60, 0x38 }));

		static constexpr auto last_offsets = std::to_array<int32_t>({ 0x8E, 0x8A, 0x90, 0x88, 0x8C, 0x92 });
		enum class hit_t
		{
			hit_perfect,
			hit_300,
			hit_200,
			hit_100,
			hit_50,
			hit_miss,
			hit_geki = hit_perfect,
			hit_katsu = hit_200
		};

	private:
		uint32_t rulesets{};

	private:
		virtual void on_reset() override
		{
			rulesets = uint32_t{};
		}
		virtual bool on_initialize() override
		{
			auto base = process.find(sequence, mask);
			if (!base)
				return false;
			rulesets = static_cast<uint32_t>(*base); // osu! is 32-bit.
			return true;
		}

	private:
		std::optional<int32_t> get_hit_any(int32_t last_offset)
		{
			if (!call_before())
				return std::nullopt;
			auto addr_last = offsets.from(process, rulesets);
			if (!addr_last)
				return std::nullopt;
			return process.read_memory<int16_t>(*addr_last + last_offset);
		}
		std::optional<int32_t> get_hit_any(hit_t hit)
		{
			return get_hit_any(last_offsets[static_cast<size_t>(hit)]);
		}
	public:
#define __gen_get_hit(name) std::optional<int32_t> get_hit_##name() { return get_hit_any(hit_t::hit_##name); } enum {}
		__gen_get_hit(perfect);
		__gen_get_hit(300);
		__gen_get_hit(200);
		__gen_get_hit(100);
		__gen_get_hit(50);
		__gen_get_hit(miss);
		__gen_get_hit(geki);
		__gen_get_hit(katsu);
#undef __gen_get_hit
	};
}