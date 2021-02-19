// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <optional>

#include "osu_memory_module_base.hpp"

namespace osu_memory
{
	class osu_memory_module_version : public osu_memory_module_base
	{
	private:
		PVOID base{};

	private:
		virtual bool on_init() override
		{
			return false;
		}
		virtual void on_reset() override
		{
			base = nullptr;
		}
	};
}