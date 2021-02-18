﻿// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include "osu_memory_reader.h"

namespace osu_memory
{
	class osu_memory_module_base : public virtual osu_memory_reader
	{
	private:
		bool is_inited{};

	private:
		/// <summary>
		/// Try to initialize. If is_inited is true, it directly returns true.
		/// </summary>
		/// <returns>is_inited after calling this function.</returns>
		bool init()
		{
			if (empty())
			{
				is_inited = false;
				return false;
			}
			if (is_inited)
				return true;
			return is_inited = on_init();
		}
	protected:
		/// <summary>
		/// Initialize things in the decendent class.
		/// You can assume you are able to read memory, but it may still fail.
		/// </summary>
		/// <returns>If initializing succeesfully, you should return true. Otherwise, false.</returns>
		virtual bool on_init() = 0;
		/// <summary>
		/// Set is_inited to false manually.
		/// </summary>
		void reset_is_init()
		{
			is_inited = false;
		}

	protected:
		/// <summary>
		/// For decendent class, call this function before any memory reading. At most one on_init is done.
		/// </summary>
		/// <returns>Only returning true means that you can read memory. Returning false means you should announce a failure.</returns>
		bool before_read()
		{
			if (update_select_osu())
			{
				if (init())
					return true;
				else
					return false;
			}
			if (empty())
				return false;
			else if (is_inited)
				return true;
			else
				return init();
		}
	};
}