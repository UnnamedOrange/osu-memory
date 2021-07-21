// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <type_traits>
#include <array>

/// <summary>
/// Use to_array in C++17.
/// 
/// Copied from:
/// https://my.oschina.net/u/4526289/blog/4760178
/// </summary>

namespace std
{
	template<typename R, typename P, size_t N, size_t... I>
	constexpr array<R, N> _to_array_impl(P(&a)[N], std::index_sequence<I...>) noexcept
	{
		return { {static_cast<R>(a[I])...} };
	}

	template<typename R, typename P, size_t N, size_t... I>
	constexpr array<R, N> _to_array_impl(P(&& a)[N], std::index_sequence<I...>) noexcept
	{
		return { {move(static_cast<R>(a[I]))...} };
	}

	template<typename R, typename P, size_t N>
	constexpr auto to_array(P(&a)[N]) noexcept
	{
		return _to_array_impl<R, P, N>(a, std::make_index_sequence<N>{});
	}

	template<typename R, typename P, size_t N>
	constexpr auto to_array(P(&& a)[N]) noexcept
	{
		return _to_array_impl<R, P, N>(move(a), std::make_index_sequence<N>{});
	}
}