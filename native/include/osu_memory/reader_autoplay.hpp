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
	/// <summary>
	/// is_autoplay.
	/// @NotThreadSafe.
	/// </summary>
	class reader_autoplay : protected reader_base_rulesets
	{
	private:
		// static constexpr auto offsets = utils::trace(std::to_array<int32_t>({ -0xB, 0x4, 0x0, 0x2C, 0x18, 0x20 }));
		static constexpr auto offsets = utils::trace(std::to_array<int32_t>({ -0xB, 0x4, 0x48, 0x0, 0x30, 0xC, 0x11C }));
		static constexpr int32_t last_offset = 0;

	public:
		/// <summary>
		/// Only support osu!mania.
		/// </summary>
		std::optional<bool> get_is_autoplay()
		{
			if (!call_before())
				return std::nullopt;
			auto addr_last = offsets.from(process, static_base);
			if (!addr_last)
				return std::nullopt;
			return process.read_memory<bool>(*addr_last + last_offset);
		}
	};
}