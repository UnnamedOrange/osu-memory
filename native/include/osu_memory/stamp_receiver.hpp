// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <type_traits>

#include <osu_memory/stamp_recorder.hpp>

namespace osu_memory::utils
{
	/// <summary>
	/// Stamp receiver.
	/// </summary>
	template <bool is_atomic_stamp>
	class enable_stamp_receiver
	{
	public:
		using stamp_base_t = typename enable_stamp_recorder<is_atomic_stamp>::stamp_base_t;
		using stamp_t = typename enable_stamp_recorder<is_atomic_stamp>::stamp_t;

	public:
		/// <summary>
		/// Reduce C5204.
		/// </summary>
		virtual ~enable_stamp_receiver() = default;

	private:
		stamp_t crt{};
	protected:
		/// <summary>
		/// Returns the stamp from the source.
		/// </summary>
		virtual stamp_base_t get_stamp() = 0;
	public:
		/// <summary>
		/// Update the current stamp to the latest.
		/// </summary>
		/// <returns>If the stamp is different from the source, returns true. Otherwise false.</returns>
		bool update()
		{
			stamp_base_t crt_stamp = get_stamp();
			crt = crt_stamp;
			return crt != crt_stamp;
		}
	};
}