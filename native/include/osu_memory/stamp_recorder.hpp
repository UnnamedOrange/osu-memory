// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <type_traits>
#include <atomic>

namespace osu_memory::utils
{
	/// <summary>
	/// Stamp recorder.
	/// </summary>
	template <bool is_atomic_stamp>
	class enable_stamp_recorder
	{
	public:
		using stamp_base_t = unsigned;
		using stamp_t = std::conditional_t<is_atomic_stamp,
			std::atomic<stamp_base_t>, stamp_base_t>;

	public:
		enable_stamp_recorder() noexcept = default;
		enable_stamp_recorder(const enable_stamp_recorder&) = delete;
		enable_stamp_recorder(enable_stamp_recorder&&) = delete;
		enable_stamp_recorder& operator=(const enable_stamp_recorder&) = delete;
		enable_stamp_recorder& operator=(enable_stamp_recorder&&) = delete;

	private:
		stamp_t stamp{};
	protected:
		/// <summary>
		/// Reset the stamp.
		/// </summary>
		void reset_stamp() noexcept
		{
			stamp = stamp_t{};
		}
		/// <summary>
		/// Add 1 to the stamp.
		/// </summary>
		void increase_stamp() noexcept
		{
			stamp++;
		}

	public:
		/// <summary>
		/// Get the stamp.
		/// </summary>
		stamp_base_t get_stamp() const noexcept
		{
			return stamp;
		}
	};
}