// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <cstdint>
#include <optional>

#include <osu_memory/process.h>
#include <osu_memory/imp_hits.h>

namespace osu_memory
{
	class reader
	{
	private:
		os::process _p;
		implementation::imp_hits _imp_hits;

	private:
		reader() = default;
		static reader& singleton();
		void update_process();

	public:
		/// <summary>
		/// Get the count of miss.
		/// Note that maybe the result is not nullopt but it is invalid.
		/// </summary>
		static std::optional<int32_t> get_miss(bool async = true, std::chrono::nanoseconds async_timeout = std::chrono::microseconds(999));
		/// <summary>
		/// Get the count of 50.
		/// Note that maybe the result is not nullopt but it is invalid.
		/// </summary>
		static std::optional<int32_t> get_50(bool async = true, std::chrono::nanoseconds async_timeout = std::chrono::microseconds(999));
		/// <summary>
		/// Get the count of 100.
		/// Note that maybe the result is not nullopt but it is invalid.
		/// </summary>
		static std::optional<int32_t> get_100(bool async = true, std::chrono::nanoseconds async_timeout = std::chrono::microseconds(999));
		/// <summary>
		/// Get the count of 200.
		/// Note that maybe the result is not nullopt but it is invalid.
		/// </summary>
		static std::optional<int32_t> get_200(bool async = true, std::chrono::nanoseconds async_timeout = std::chrono::microseconds(999));
		/// <summary>
		/// Get the count of 300 (yellow).
		/// Note that maybe the result is not nullopt but it is invalid.
		/// </summary>
		static std::optional<int32_t> get_300(bool async = true, std::chrono::nanoseconds async_timeout = std::chrono::microseconds(999));
		/// <summary>
		/// Get the count of perfect.
		/// Note that maybe the result is not nullopt but it is invalid.
		/// </summary>
		static std::optional<int32_t> get_perfect(bool async = true, std::chrono::nanoseconds async_timeout = std::chrono::microseconds(999));
	};
}