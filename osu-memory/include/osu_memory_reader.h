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
	class osu_memory_reader : public memory_reader
	{
	private:
		using memory_reader::select_process;
		/// <summary>
		/// Select osu! process. Assume that there's only one process named "osu!.exe".
		/// </summary>
		/// <returns>Whether it succeeded.</returns>
		bool select_osu();

	public:
		/// <summary>
		/// Call this function before any memory reading.
		/// </summary>
		/// <returns>Whether osu is selected again. Returning true means you should re-initialize something. Note that you may still fail if this function returns true.</returns>
		bool update_select_osu();
	};
}