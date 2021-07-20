// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <type_traits>

#include <osu_memory/osu_daemon.hpp>
#include <osu_memory/stamp_receiver.hpp>

namespace osu_memory::utils
{
	/// <summary>
	/// Stamp receiver. Implenmented with osu_daemon.
	/// </summary>
	class enable_stamp_receiver_impl : public osu_memory::utils::enable_stamp_receiver<false>
	{
		virtual stamp_base_t get_stamp() override
		{
			return osu_memory::os::osu_daemon::get_instance().get_stamp();
		}
	};
}