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
		virtual bool on_construct(const os::process& process) override
		{
			auto address = find(process, binary, mask);
			if (address)
			{
				base = *address;
				return true;
			}
			return false;
		}
		virtual void on_reset() override
		{
			base = 0;
		}

	private:
		std::optional<int32_t> _sync_get_hit(const os::process& process, intptr_t last_offset)
		{
			if (!construct(process))
				return std::nullopt;

			int32_t crt = int32_t(intptr_t(base)); // 32-bit osu!
			for (auto offset : offsets)
			{
				auto addr = read_memory<int32_t>(process, PVOID(crt + offset));
				if (!addr)
					return std::nullopt; // No need to reset.
				crt = *addr;
			}
			return read_memory<int16_t>(process, PVOID(crt + last_offset));
		}
		std::optional<int32_t> _sync_get_miss(const os::process& process)
		{
			return _sync_get_hit(process, 0x92);
		}

	public:
		std::optional<int32_t> get_miss(const os::process& process, bool async = true, std::chrono::nanoseconds async_timeout = std::chrono::microseconds(999))
		{
			if (busy())
				return std::nullopt;
			if (commit(std::bind(&imp_hits::_sync_get_miss, this, process), async, async_timeout))
				return std::any_cast<std::optional<int32_t>>(get_package());
			return std::nullopt;
		}
	};
}