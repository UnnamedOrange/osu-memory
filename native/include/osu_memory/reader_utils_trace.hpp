// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <type_traits>
#include <optional>

#include <osu_memory/readable_process.hpp>

namespace osu_memory::utils
{
	/// <summary>
	/// Trace steps of offsets.
	/// Give the offsets on the initialization, and use trace_object.from.
	/// </summary>
	/// <typeparam name="array_t"></typeparam>
	template <typename array_t>
	class trace
	{
	private:
		array_t offsets;
	public:
		constexpr trace(const array_t& offsets) noexcept : offsets(offsets) {}

	public:
		template <typename addr_t>
		std::optional<addr_t> from(const os::readable_process& process, addr_t base) const
		{
			addr_t crt = base;
			for (auto offset : offsets)
			{
				using common_addr_offset_t = std::common_type_t<decltype(crt), decltype(offset)>;
				auto addr = process.read_memory<addr_t>(
					static_cast<common_addr_offset_t>(crt) + static_cast<common_addr_offset_t>(offset));
				if (!addr)
					return std::nullopt;
				crt = *addr;
			}
			return crt;
		}
	};

	template <typename array_t>
	trace(array_t)->trace<std::decay_t<array_t>>;
}