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

std::optional<PVOID> osu_memory::memory_reader::find_one(const std::vector<BYTE> bin, DWORD protect)
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

	// Do not load the whole memory.
	std::vector<memory_region> regions;
	PVOID crt_address = min_address;
	while (crt_address < max_address)
	{
		MEMORY_BASIC_INFORMATION mem_info;
		if (!VirtualQueryEx(hProcess, crt_address, &mem_info, sizeof(mem_info)))
			return std::nullopt;

		if ((mem_info.Protect & protect) && mem_info.State == MEM_COMMIT)
			regions.push_back({ mem_info.BaseAddress, mem_info.RegionSize });
		crt_address = reinterpret_cast<PVOID>(reinterpret_cast<SIZE_T>(crt_address) + mem_info.RegionSize);
	}

	std::vector<size_t> f;
	// KMP.
	{
		f.resize(bin.size() + 1);
		f[0] = f[1] = 0;
		size_t matched = 0;
		for (size_t i = 1; i < bin.size(); i++)
		{
			while (matched && bin[matched] != bin[i])
				matched = f[matched];
			if (bin[matched] == bin[i])
				matched++;

			if (i + 1 < bin.size() && bin[i + 1] == bin[matched])
				f[i + 1] = f[matched];
			else
				f[i + 1] = matched;
		}
	}

	for (const auto& r : regions)
	{
		constexpr static SIZE_T cache_size = 8192;
		std::array<BYTE, cache_size> cache;
		size_t matched{};
		for (size_t crt = reinterpret_cast<size_t>(r.base_address);
			crt - reinterpret_cast<size_t>(r.base_address) < r.size;
			crt += cache_size)
		{
			SIZE_T read;
			SIZE_T to_read = std::min(cache_size, reinterpret_cast<size_t>(r.base_address) + r.size - crt);
			if (!ReadProcessMemory(hProcess, reinterpret_cast<PVOID>(crt), cache.data(),
				to_read, &read) || read != to_read)
				return std::nullopt;

			for (size_t i = 0; i < read; i++)
			{
				while (matched == bin.size() || (matched && cache[i] != bin[matched]))
					matched = f[matched];
				if (cache[i] == bin[matched])
					matched++;
				if (matched == bin.size())
					return reinterpret_cast<PVOID>(crt + i - bin.size() + 1);
			}
		}
	}
	return std::nullopt;
}