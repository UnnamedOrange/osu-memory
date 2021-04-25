// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#include <osu_memory/memory_reader.h>

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
		if (DWORD dwExitCode; !hProcess_temp ||
			!GetExitCodeProcess(hProcess_temp, &dwExitCode) ||
			dwExitCode != STILL_ACTIVE)
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
std::chrono::microseconds memory_reader::get_process_time() const
{
	if (!hProcess)
		return std::chrono::microseconds();
	uint64_t create_time;
	FILETIME __unused[1];
	GetProcessTimes(hProcess, (LPFILETIME)&create_time, __unused, __unused, __unused);
	uint64_t system_time;
	GetSystemTimeAsFileTime((LPFILETIME)&system_time);
	return std::chrono::microseconds((system_time - create_time) / 10);
}

std::optional<memory_reader::dumped_memory_t> memory_reader::get_memory_regions(DWORD protect) const
{
	PVOID min_address;
	PVOID max_address;
	{
		SYSTEM_INFO sys_info;
		GetSystemInfo(&sys_info);
		min_address = sys_info.lpMinimumApplicationAddress;
		max_address = sys_info.lpMaximumApplicationAddress;
	}

	// Do not load the whole memory.
	std::vector<memory_region> ret;
	PVOID crt_address = min_address;
	while (crt_address < max_address)
	{
		MEMORY_BASIC_INFORMATION mem_info;
		if (!VirtualQueryEx(hProcess, crt_address, &mem_info, sizeof(mem_info)))
			return std::nullopt;

		if ((mem_info.Protect & protect) && mem_info.State == MEM_COMMIT)
			ret.push_back({ mem_info.BaseAddress, mem_info.RegionSize });
		crt_address = reinterpret_cast<PVOID>(reinterpret_cast<SIZE_T>(crt_address) + mem_info.RegionSize);
	}
	return ret;
}

std::optional<PVOID> memory_reader::find_one(const std::vector<BYTE> bin, DWORD protect)
{
	if (empty())
		return std::nullopt;

	auto regions = get_memory_regions(protect);
	if (!regions)
		return std::nullopt;

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

	for (const auto& r : *regions)
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
				break;

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
std::optional<PVOID> memory_reader::find_one(const std::vector<BYTE> bin, std::string_view mask)
{
	if (bin.size() != mask.length())
		throw std::invalid_argument("Sizes of bin and mask must be the same.");

	if (empty())
		return std::nullopt;

	auto regions = get_memory_regions(PAGE_EXECUTE_READWRITE);
	if (!regions)
		return std::nullopt;

	for (const auto& r : *regions)
	{
		auto region_data = read_memory(r.base_address, r.size);
		if (!region_data)
			continue;

		for (size_t i = 0; i + bin.size() - 1 < (*region_data).size(); i++)
		{
			bool ok = true;
			for (size_t j = 0; j < bin.size(); j++)
			{
				if (!(mask[j] == '?' || (*region_data)[i + j] == bin[j]))
				{
					ok = false;
					break;
				}
			}
			if (ok)
				return reinterpret_cast<PVOID>(reinterpret_cast<size_t>(r.base_address) + i);
		}
	}
	return std::nullopt;
}