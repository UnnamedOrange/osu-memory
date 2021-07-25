// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <osu_memory/reader_utils_find_base.hpp>
#include <osu_memory/reader_utils_base_impl.hpp>

namespace osu_memory
{
	class base_status_container
	{
	public:
		static constexpr utils::find_base base{
			std::to_array<uint8_t>({ 0x75, 0x07, 0x8B, 0x45, 0x90, 0xC6, 0x40, 0x2A, 0x00, 0x83, 0x3D, 000, 000, 000, 000, 0x0F }),
			"xxxxxxxxxxx????x"
		};
	};
	using reader_base_status = reader_utils_base_impl<base_status_container>;
}