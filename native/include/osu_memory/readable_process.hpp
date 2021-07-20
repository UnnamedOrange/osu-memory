// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <cstdint>
#include <vector>
#include <type_traits>
#include <optional>

#include <osu_memory/process.hpp>

namespace osu_memory::os
{
	/// <summary>
	/// CRTP reading process memory support.
	/// </summary>
	template <typename T>
	class enable_readable_process
	{
		static_assert(std::is_same_v<decltype(T().native_handle()), process::native_handle_t>,
			"native_handle() of T must be native_handle_t.");

	public:
		virtual ~enable_readable_process() noexcept = default;

	public:
		/// <summary>
		/// Read memory from the specific process.
		/// If it fails, the return value is std::nullopt.
		/// </summary>
		template <typename addr_t, typename preferred_size_t>
		std::optional<std::vector<uint8_t>> read_memory(addr_t address_start, preferred_size_t size)
		{
			static_assert(sizeof(addr_t) >= 4,
				"A pointer is at least 32 bits.");
			static_assert(sizeof(LPCVOID) >= sizeof(addr_t),
				"addr_t should not be smaller than uintptr_t.");
			static_assert(std::is_integral_v<preferred_size_t> && std::is_unsigned_v<preferred_size_t>,
				"preferred_size_t should be unsigned integer.");

			T* __this = reinterpret_cast<T*>(this);
			if (!__this->native_handle())
				return std::nullopt;

			std::vector<uint8_t> ret(size);
			SIZE_T read{};
			if (!ReadProcessMemory(__this->native_handle(),
				reinterpret_cast<LPCVOID>(address_start), ret.data(), size, &read) ||
				read != size)
				return std::nullopt;
			return ret;
		}
		/// <summary>
		/// Read memory from the specific process.
		/// If it fails, the return value is std::nullopt.
		/// </summary>
		template <typename trivial_t, typename addr_t>
		std::optional<trivial_t> read_memory(addr_t address_start)
		{
			static_assert(std::is_trivial_v<trivial_t>,
				"trivial_t must be trivial.");
			static_assert(!std::is_pointer_v<trivial_t>,
				"trivial_t cannot be a pointer type");
			static_assert(sizeof(addr_t) >= 4,
				"A pointer is at least 32 bits.");
			static_assert(sizeof(LPCVOID) >= sizeof(addr_t),
				"addr_t should not be smaller than uintptr_t.");

			T* __this = dynamic_cast<T*>(this);
			if (!__this->native_handle())
				return std::nullopt;

			trivial_t ret{};
			SIZE_T read{};
			if (!ReadProcessMemory(__this->native_handle(),
				reinterpret_cast<LPCVOID>(address_start), &ret, sizeof(ret), &read) ||
				read != sizeof(ret))
				return std::nullopt;
			return ret;
		}
	};

	template <typename T>
	class make_readable_process : public T, public enable_readable_process<T>
	{
	public:
		using T::T;
		make_readable_process(const T& base) : T(base) {}
		make_readable_process(T&& base) : T(std::move(base)) {}
	};

	/// <summary>
	/// Process object type for Windows. A helper to hold the handle of the process.
	/// Enable reading memory from the process.
	/// @NotThreadSafe.
	/// </summary>
	using readable_process = make_readable_process<process>;
}