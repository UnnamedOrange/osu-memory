// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <osu_memory/reader_hit.hpp>
#include <osu_memory/reader_mod.hpp>
#include <osu_memory/reader_status.hpp>
#include <osu_memory/reader_autoplay.hpp>
#include <osu_memory/reader_mania_keys.hpp>

namespace osu_memory
{
	/// <summary>
	/// Memory reader collection.
	/// </summary>
	class reader :
		public reader_hit,
		public reader_mod,
		public reader_status,
		public reader_autoplay,
		public reader_mania_keys
	{};
}