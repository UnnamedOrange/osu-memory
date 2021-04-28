// Copyright (c) UnnamedOrange. Licensed under the MIT Licence.
// See the LICENSE file in the repository root for full licence text.

#include <osu_memory/reader.h>

namespace osu_memory
{
	reader& reader::singleton()
	{
		static reader _;
		return _;
	}
	void reader::update_process()
	{
		if (_p.empty())
		{
			auto t = os::process::open(L"osu!.exe");
			if (t.size())
				_p = t[0];
		}
	}

	std::optional<int32_t> reader::get_miss(bool async, std::chrono::nanoseconds async_timeout)
	{
		auto& s = singleton();
		s.update_process();
		return s._imp_hits.get_miss(s._p, async, async_timeout);
	}
}