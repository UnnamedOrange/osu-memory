// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#include "osu_memory_reader.h"

using namespace osu_memory;

bool osu_memory_reader::select_osu()
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return false;

	PROCESSENTRY32W pe32{ sizeof(pe32) };
	if (!Process32FirstW(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return false;
	}

	bool ret = false;
	do
	{
		using namespace std::literals;
		if (pe32.szExeFile == L"osu!.exe"s)
		{
			try
			{
				select_process(pe32.th32ProcessID);
			}
			catch (const open_process_fail&)
			{
				continue;
			}
			ret = true;
			break;
		}
	} while (Process32NextW(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return ret;
}

bool osu_memory_reader::update_select_osu()
{
	try_detach();
	if (empty())
		return select_osu();
	return false;
}