// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#pragma once

#include <stdexcept>
#include <vector>
#include <string_view>
#include <Windows.h>

namespace osu_memory::os
{
	/// <summary>
	/// Type of native process id.
	/// </summary>
	using native_id_t = DWORD;
	/// <summary>
	/// Type of native process handle.
	/// </summary>
	using native_handle_t = HANDLE;
	/// <summary>
	/// Will be thrown on an error occurring when opening the process or duplicating the handle.
	/// </summary>
	class open_process_error : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};

	/// <summary>
	/// Process object type for Windows.
	/// It is reset when the process is terminated, in logic.
	/// To realize that, each function has 500ns of lag or so.
	/// Cache the handle or id in dense access.
	/// </summary>
	class process
	{
	private:
		mutable native_handle_t handle{};

	public:
		process() noexcept = default;
		explicit process(native_id_t pid);
		virtual ~process();
		process(const process& another);
		process(process&& another) noexcept;
		process& operator=(const process& another)
		{
			if (this != &another)
			{
				reset();
				another._check();
				if (another.handle)
				{
					HANDLE hProcess{};
					if (!DuplicateHandle(GetCurrentProcess(), another.handle,
						GetCurrentProcess(), &hProcess,
						NULL, false, DUPLICATE_SAME_ACCESS))
						throw open_process_error("Fail to DuplicateHandle.");
					handle = hProcess;
				}
			}
			return *this;
		}
		process& operator=(process&& another) noexcept
		{
			if (this != &another)
			{
				handle = another.handle;
				another.handle = 0;
			}
			return *this;
		}

	public:
		bool operator==(const process& another) const;
		bool operator!=(const process& another) const;

	public:
		/// <summary>
		/// Close the handle (if not empty) and empty the object.
		/// </summary>
		void reset();
		/// <summary>
		/// Whether the object is empty.
		/// </summary>
		bool empty() const;
	private:
		/// <summary>
		/// Call this at the entry of any function.
		/// This will cause 500ns of lag.
		/// </summary>
		void _check() const;
	public:
		/// <summary>
		/// Wait until the process exits.
		/// It will return immediately if the process has exited (500ns).
		/// </summary>
		void wait_until_exit();
	public:
		/// <summary>
		/// Get the native handle (HANDLE) of the process.
		/// </summary>
		native_handle_t native_handle() const;
		/// <summary>
		/// Get the native pid (DWORD) of the process.
		/// </summary>
		native_id_t native_id() const;

	public:
		/// <summary>
		/// Open the process specified by its name.
		/// </summary>
		/// <param name="process_name">The process name.</param>
		/// <returns>The process objects.</returns>
		static std::vector<process> open(std::wstring_view process_name);
	};
}