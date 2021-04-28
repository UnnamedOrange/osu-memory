// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <array>
#include <functional>
#include <chrono>

#include <osu_memory/process.h>
#include <osu_memory/imp_base.h>

namespace osu_memory::implementation
{
	class imp_hits : public imp_base
	{
	private:
		static constexpr std::array<uint8_t, 9> binary{ 0x7D, 0x15, 0xA1, 000, 000, 000, 000, 0x85, 0xC0 };
		static constexpr std::string_view mask = "xxx????xx";
		static constexpr std::array<intptr_t, 4> offsets{ -0xB, 0x4, 0x60, 0x38 };
		PVOID base{};

	private:
		virtual bool on_construct(const os::process& process) override;
		virtual void on_reset() override;

	private:
		std::optional<int32_t> _sync_get_hit(const os::process& process, intptr_t last_offset);
		std::optional<int32_t> _sync_get_miss(const os::process& process);

	public:
		/// <summary>
		/// Get the count of miss.
		/// Note that maybe the result is not nullopt but it is invalid.
		/// </summary>
		std::optional<int32_t> get_miss(const os::process& process, bool async, std::chrono::nanoseconds async_timeout);
	};
}