// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <osu_memory/reader_base.hpp>
#include <osu_memory/reader_utils_find_base.hpp>

namespace osu_memory
{
	class reader_base_status : public reader_base
	{
	protected:
		uint32_t status_base{};

	private:
		virtual void on_reset() override
		{
			status_base = uint32_t{};
		}
		virtual bool on_initialize() override
		{
			auto base = utils::base_status.find(process);
			if (!base)
				return false;
			status_base = static_cast<uint32_t>(*base);
			return true;
		}

	public:
		using reader_base::reader_base;
		using reader_base::operator=;
	};
}