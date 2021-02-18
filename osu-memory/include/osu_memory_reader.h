// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <string>

#include <Windows.h>
#undef min
#undef max
#include <tlhelp32.h>

#include "memory_reader.h"

namespace osu_memory
{
	class osu_memory_reader : virtual public memory_reader
	{
	public:
		bool select_osu();
	};
}