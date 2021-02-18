// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#include "memory_reader.h"

using namespace osu_memory;

void memory_reader::exit_thread_proc()
{
	HANDLE waits[]{ hEvent_exit, hProcess };
	WaitForMultipleObjects(static_cast<DWORD>(std::size(waits)),
		waits, FALSE, INFINITE);
	ResetEvent(hEvent_exit);
	CloseHandle(hProcess);
	hProcess = nullptr;
}

bool memory_reader::empty() const
{
	return !exit_thread.joinable();
}
bool memory_reader::try_detach()
{
	if (empty())
		return false;
	bool ret = false;
	if (!hProcess)
	{
		detach();
		ret = true;
	}
	return ret;
}
void memory_reader::select_process(DWORD dwProcessId)
{
	detach();

	{
		HANDLE hProcess_temp = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
		DWORD dwError = GetLastError();
		// It is possible that the process has been terminated but OpenProcess succeeded.
		if (!hProcess_temp || WaitForSingleObject(hProcess_temp, 1) == WAIT_OBJECT_0)
		{
			if (hProcess_temp)
				CloseHandle(hProcess_temp);
			throw open_process_fail("Fail to OpenProcess. The error code is " + std::to_string(dwError) + ".");
		}
		hProcess = hProcess_temp;
		ResetEvent(hEvent_exit);
	}
	exit_thread = std::move(std::thread(&memory_reader::exit_thread_proc, this));
}
bool memory_reader::detach()
{
	if (empty())
		return false;

	SetEvent(hEvent_exit);
	exit_thread.join();

	return true;
}

std::optional<memory_reader::dumped_memory_t> memory_reader::dump_memory() const
{
	if (empty())
		return std::nullopt;

	PVOID min_address;
	PVOID max_address;
	{
		SYSTEM_INFO sys_info;
		GetSystemInfo(&sys_info);
		min_address = sys_info.lpMinimumApplicationAddress;
		max_address = sys_info.lpMaximumApplicationAddress;
	}

	std::vector<memory_region> regions;
	PVOID crt_address = min_address;
	while (crt_address < max_address)
	{
		MEMORY_BASIC_INFORMATION mem_info;
		if (!VirtualQueryEx(hProcess, crt_address, &mem_info, sizeof(mem_info)))
			return std::nullopt;

		if ((mem_info.Protect & PAGE_EXECUTE_READWRITE) && mem_info.State == MEM_COMMIT)
			regions.push_back({ mem_info.BaseAddress, mem_info.RegionSize });
		crt_address = reinterpret_cast<PVOID>(reinterpret_cast<SIZE_T>(crt_address) + mem_info.RegionSize);
	}

	for (auto& r : regions)
	{
		r.data.resize(r.size);
		SIZE_T read;
		if (!ReadProcessMemory(hProcess, r.base_address, r.data.data(), r.size, &read) || read != r.size)
			return std::nullopt;
	}
	return regions;
}