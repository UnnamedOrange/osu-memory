// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <array>
#include <string_view>
#include <optional>

#include <osu_memory/readable_process.hpp>
#include <osu_memory/to_array.hpp>

namespace osu_memory::utils
{
	template <typename array_t>
	class find_base
	{
	private:
		array_t sequence;
		std::string_view mask;
	public:
		constexpr find_base(const array_t& sequence, std::string_view mask) noexcept :
			sequence(sequence), mask(mask) {}

		std::optional<uintptr_t> find(const os::readable_process& process) const
		{
			auto base = process.find(sequence, mask);
			if (!base)
				return std::nullopt;
			return static_cast<uintptr_t>(*base);
		}
	};

	template <typename array_t>
	find_base(array_t, std::string_view)->find_base<std::decay_t<array_t>>;

	inline constexpr find_base base_rulesets{
		std::to_array<uint8_t>({ 0x7D, 0x15, 0xA1, 000, 000, 000, 000, 0x85, 0xC0 }),
		"xxx????xx"
	};
}