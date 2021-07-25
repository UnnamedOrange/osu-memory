// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <osu_memory/reader_utils_find_base.hpp>
#include <osu_memory/reader_utils_base_impl.hpp>

namespace osu_memory
{
	class base_rulesets_container
	{
	public:
		static constexpr utils::find_base base{
			std::to_array<uint8_t>({ 0x7D, 0x15, 0xA1, 000, 000, 000, 000, 0x85, 0xC0 }),
			"xxx????xx"
		};
	};
	using reader_base_rulesets = reader_utils_base_impl<base_rulesets_container>;
}