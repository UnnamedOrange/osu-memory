// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <type_traits>

namespace osu_memory::utils
{
	/// <summary>
	/// CRTP singleton template.
	/// The descentant must have a private default constructor.
	/// To reduce warnings, the descentant should delete copy and move constructor,
	/// and delete copy and move assignment operator.
	/// </summary>
	template <typename T>
	class enable_singleton
	{
	protected:
		enable_singleton() noexcept = default; // Must be protected default.
	private:
		enable_singleton(const enable_singleton&) = delete;
		enable_singleton(enable_singleton&&) = delete;
		enable_singleton& operator=(const enable_singleton&) = delete;
		enable_singleton& operator=(enable_singleton&&) = delete;
	public:
		virtual ~enable_singleton() noexcept = default;
	public:
		static T& get_instance() noexcept(std::is_nothrow_constructible<T>::value)
		{
			static T instance;
			return instance;
		}
	};
}