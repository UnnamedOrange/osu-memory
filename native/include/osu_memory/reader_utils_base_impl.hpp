// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <osu_memory/reader_base.hpp>
#include <osu_memory/reader_utils_find_base.hpp>

namespace osu_memory
{
	template <typename find_base_container>
	class reader_utils_base_impl : public reader_base
	{
	protected:
		uint32_t static_base{};

	private:
		virtual void on_reset() override
		{
			static_base = uint32_t{};
		}
		virtual bool on_initialize() override
		{
			auto base = find_base_container::base.find(process);
			if (!base)
				return false;
			static_base = static_cast<uint32_t>(*base); // osu! is 32-bit.
			return true;
		}

	public:
		using reader_base::reader_base;
		using reader_base::operator=;
	};
}