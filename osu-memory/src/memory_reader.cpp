// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#include "memory_reader.h"

void osu_memory::memory_reader::exit_thread_proc()
{
	HANDLE waits[]{ hEvent_exit, hProcess };
	WaitForMultipleObjects(static_cast<DWORD>(std::size(waits)),
		waits, FALSE, INFINITE);
	ResetEvent(hEvent_exit);
	CloseHandle(hProcess);
	hProcess = nullptr;
}

bool osu_memory::memory_reader::empty() const
{
	return !exit_thread.joinable();
}
bool osu_memory::memory_reader::try_detach()
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
void osu_memory::memory_reader::select_process(DWORD dwProcessId)
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
			throw std::runtime_error("Fail to OpenProcess. The error code is " + std::to_string(dwError) + ".");
		}
		hProcess = hProcess_temp;
		ResetEvent(hEvent_exit);
	}
	exit_thread = std::move(std::thread(&memory_reader::exit_thread_proc, this));
}
bool osu_memory::memory_reader::detach()
{
	if (empty())
		return false;

	SetEvent(hEvent_exit);
	exit_thread.join();

	return true;
}