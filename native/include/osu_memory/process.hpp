// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <stdexcept>
#include <vector>
#include <chrono>
#include <string_view>
#include <Windows.h>

#include <tlhelp32.h>

namespace osu_memory::os
{
	/// <summary>
	/// Will be thrown on an error occurring when opening the process or duplicating the handle.
	/// </summary>
	class open_process_error : public std::runtime_error
	{
	public:
		open_process_error() = delete; // Reduce C4623.
		using std::runtime_error::runtime_error;
	};

	/// <summary>
	/// Process object type for Windows. A helper to hold the handle of the process.
	/// @NotThreadSafe.
	/// </summary>
	class process
	{
	public:
		/// <summary>
		/// Type of native process id.
		/// </summary>
		using native_id_t = DWORD;
		/// <summary>
		/// Type of native process handle.
		/// </summary>
		using native_handle_t = HANDLE;

	protected:
		native_handle_t handle{};

	public:
		process() noexcept = default;
		process(native_id_t pid, DWORD desired_access)
		{
			HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION | desired_access, false, pid);
			if (!hProcess)
				throw open_process_error("Fail to OpenProcess.");
			handle = hProcess;
		}
		virtual ~process() noexcept
		{
			reset();
		}
		process(const process& another)
		{
			*this = another;
		}
		process(process&& another) noexcept
		{
			*this = std::move(another);
		}
		process& operator=(const process& another)
		{
			if (this != &another)
			{
				if (another.native_handle())
				{
					HANDLE hProcess{};
					if (!DuplicateHandle(GetCurrentProcess(), another.native_handle(),
						GetCurrentProcess(), &hProcess,
						NULL, false, DUPLICATE_SAME_ACCESS))
						throw open_process_error("Fail to DuplicateHandle.");
					reset();
					handle = hProcess;
				}
			}
			return *this;
		}
		process& operator=(process&& another) noexcept
		{
			if (this != &another)
			{
				handle = another.native_handle();
				another.handle = 0;
			}
			return *this;
		}

	public:
		/// <summary>
		/// Whether the two process objects hold the same process.
		/// </summary>
		[[nodiscard]] bool operator==(const process& another) const noexcept
		{
			return native_id() == another.native_id();
		}
		/// <summary>
		/// Whether the two process objects hold two different processes.
		/// </summary>
		[[nodiscard]] bool operator!=(const process& another) const noexcept
		{
			return !operator==(another);
		}

	public:
		/// <summary>
		/// Close the handle (if not empty) and empty the object.
		/// </summary>
		void reset() noexcept
		{
			if (native_handle())
			{
				CloseHandle(native_handle());
				handle = 0;
			}
		}
		/// <summary>
		/// Whether the object is empty.
		/// </summary>
		[[nodiscard]] bool empty() const noexcept
		{
			return !native_handle();
		}
	public:
		/// <summary>
		/// Wait until the process exits.
		/// It will return immediately if the process has exited (500ns).
		/// </summary>
		void wait_until_exit() const noexcept
		{
			if (native_handle())
				WaitForSingleObject(native_handle(), INFINITE);
		}
		/// <summary>
		/// Whether the process is still active.
		/// If the process object is empty, false is to be returned.
		/// </summary>
		[[nodiscard]] bool still_active() const noexcept
		{
			if (!native_handle())
				return false;
			DWORD exit_code{};
			GetExitCodeProcess(native_handle(), &exit_code); // Assume it always successful.
			return exit_code == STILL_ACTIVE;
		}
	public:
		/// <summary>
		/// Get the native handle (HANDLE) of the process.
		/// </summary>
		[[nodiscard]] native_handle_t native_handle() const noexcept
		{
			return handle;
		}
		/// <summary>
		/// Get the native pid (DWORD) of the process.
		/// </summary>
		[[nodiscard]] native_id_t native_id() const noexcept
		{
			return GetProcessId(native_handle());
		}

	public:
		/// <summary>
		/// Open the processes specified by the name. Multiple processes will be opened if there exist.
		/// </summary>
		/// <param name="process_name">The process name.</param>
		/// <returns>The process objects.</returns>
		[[nodiscard]] static std::vector<process> open(std::wstring_view process_name, DWORD desired_access) noexcept
		{
			std::vector<process> ret;
			do // This is not a loop.
			{
				HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
				if (hProcessSnap == INVALID_HANDLE_VALUE)
					break; // Use of do-while.

				PROCESSENTRY32W pe32{ sizeof(pe32) };
				if (!Process32FirstW(hProcessSnap, &pe32))
				{
					CloseHandle(hProcessSnap);
					break;
				}

				do
				{
					using namespace std::literals;
					if (pe32.szExeFile == process_name)
					{
						try
						{
							ret.emplace_back(pe32.th32ProcessID, desired_access);
						}
						catch (const open_process_error&) {}
					}
				} while (Process32NextW(hProcessSnap, &pe32));

				CloseHandle(hProcessSnap);
			} while (false);
			return ret;
		}
	};
}