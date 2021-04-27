// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#include <osu_memory/process.h>

#include <tlhelp32.h>

namespace osu_memory::os
{
	process::process(native_id_t pid)
	{
		DWORD desired_access = PROCESS_VM_READ |
			PROCESS_QUERY_INFORMATION |
			SYNCHRONIZE;
		HANDLE hProcess = OpenProcess(desired_access, false, pid);
		if (!hProcess)
			throw open_process_error("Fail to OpenProcess.");
		handle = hProcess;
	}
	process::~process()
	{
		reset();
	}
	process::process(const process& another)
	{
		*this = another;
	}
	process::process(process&& another) noexcept
	{
		*this = std::move(another);
	}

	bool process::operator==(const process& another) const
	{
		return native_id() == another.native_id();
	}
	bool process::operator!=(const process& another) const
	{
		return native_id() != another.native_id();
	}

	void process::reset()
	{
		if (handle)
		{
			CloseHandle(handle);
			handle = 0;
		}
	}
	bool process::empty() const
	{
		_check();
		return !handle;
	}
	void process::_check() const
	{
		auto now = std::chrono::steady_clock::now();
		using namespace std::literals;
		if (now - cache_time > 16ms)
		{
			cache_time = now;
			if (DWORD code;
				handle && (!GetExitCodeProcess(handle, &code) || code != STILL_ACTIVE))
			{
				CloseHandle(handle);
				handle = 0;
			}
		}
	}
	void process::wait_until_exit()
	{
		_check();
		if (handle)
			WaitForSingleObject(handle, INFINITE);
	}
	native_handle_t process::native_handle() const
	{
		_check();
		return handle;
	}
	native_id_t process::native_id() const
	{
		_check();
		return GetProcessId(handle);
	}

	std::vector<process> process::open(std::wstring_view process_name)
	{
		std::vector<process> ret;
		do
		{
			HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hProcessSnap == INVALID_HANDLE_VALUE)
				break;

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
						ret.emplace_back(pe32.th32ProcessID);
					}
					catch (const open_process_error&) {}
				}
			} while (Process32NextW(hProcessSnap, &pe32));

			CloseHandle(hProcessSnap);
		} while (false);
		return ret;
	}
}