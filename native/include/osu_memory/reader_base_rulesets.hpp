// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <osu_memory/reader_base.hpp>
#include <osu_memory/reader_utils_find_base.hpp>

namespace osu_memory
{
	class reader_base_rulesets : public reader_base
	{
	protected:
		uint32_t rulesets{};

	private:
		virtual void on_reset() override
		{
			rulesets = uint32_t{};
		}
		virtual bool on_initialize() override
		{
			auto base = utils::base_rulesets.find(process);
			if (!base)
				return false;
			rulesets = static_cast<uint32_t>(*base); // osu! is 32-bit.
			return true;
		}

	public:
		using reader_base::reader_base;
		using reader_base::operator=;
	};
}