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
	/// mania_keys.
	/// @NotThreadSafe.
	/// </summary>
	class reader_mania_keys : protected reader_base_rulesets
	{
	private:
		static constexpr auto offsets = utils::trace(std::to_array<int32_t>({ -0xB, 0x4, 0xC8, 0x94, 0x4, 0x44 }));

	public:
		/// <summary>
		/// (virtual code, is down)
		/// </summary>
		std::optional<std::vector<std::pair<int, bool>>> get_mania_keys()
		{
			if (!call_before())
				return std::nullopt;

			auto optional_array_base = offsets.from(process, rulesets);
			if (!optional_array_base)
				return std::nullopt;
			auto array_base = *optional_array_base;

			auto key_size = process.read_memory<uint32_t>(array_base + 0xC);
			if (!key_size || !*key_size || *key_size > 20u)
				return std::nullopt;

			optional_array_base = process.read_memory<uint32_t>(array_base + 0x4);
			if (!optional_array_base)
				return std::nullopt;
			array_base = *optional_array_base;

			std::vector<std::pair<int, bool>> ret;
			for (size_t i = 0; i < *key_size; i++)
			{
				auto key_base_addr = process.read_memory<uint32_t>(array_base + 8 + i * 4);
				if (!key_base_addr || !*key_base_addr)
					break;
				auto key_code = process.read_memory<int32_t>(*key_base_addr + 0x30);
				if (!key_code)
					return std::nullopt;
				auto is_key_down = process.read_memory<uint8_t>(*key_base_addr + 0x3B);
				if (!is_key_down)
					return std::nullopt;
				ret.emplace_back(*key_code, *is_key_down);
			}
			return ret;
		}
	};
}