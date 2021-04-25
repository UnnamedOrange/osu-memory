// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include "osu_memory_module_base.hpp"
#include "osu_memory_module_version.hpp"
#include "osu_memory_module_mania_key.hpp"
#include "osu_memory_module_hit.hpp"

namespace osu_memory
{
	class osu_memory_collection :
		public osu_memory_module_version,
		public osu_memory_module_mania_key,
		public osu_memory_module_hit
	{

	};
}