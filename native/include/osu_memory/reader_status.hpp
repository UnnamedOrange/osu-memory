// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <array>
#include <string_view>

#include <osu_memory/reader_base.hpp>
#include <osu_memory/reader_base_status.hpp>
#include <osu_memory/to_array.hpp>
#include <osu_memory/reader_utils_trace.hpp>

namespace osu_memory
{
	enum class status_t : uint32_t
	{
		menu,
		edit,
		play,
		exit,
		select_edit,
		select_play,
		select_drawings,
		rank,
		update,
		busy,
		unknown,
		lobby,
		match_setup,
		select_multi,
		ranking_vs,
		online_selection,
		options_offset_wizard,
		ranking_tag_coop,
		ranking_team,
		beatmap_import,
		package_updater,
		benchmark,
		tourney,
		charts
	};

	/// <summary>
	/// status, previous_status.
	/// @NotThreadSafe.
	/// </summary>
	class reader_status : protected reader_base_status
	{
	private:
		static constexpr auto offsets = utils::trace(std::to_array<int32_t>({ 0xB }));
		static constexpr auto last_offsets = std::to_array<int32_t>({ 0x0, 0x4 });

	private:
		template <size_t which>
		std::optional<status_t> get_status_any()
		{
			static_assert(which < last_offsets.size(),
				"which < last_offsets.size().");

			if (!call_before())
				return std::nullopt;
			auto addr_last = offsets.from(process, status_base);
			if (!addr_last)
				return std::nullopt;
			auto status = process.read_memory<uint32_t>(*addr_last + last_offsets[which]);
			if (!status)
				return std::nullopt;
			return static_cast<status_t>(*status);
		}
	public:
		std::optional<status_t> get_status() { return get_status_any<0>(); }
		std::optional<status_t> get_previous_status() { return get_status_any<1>(); }
	};
}