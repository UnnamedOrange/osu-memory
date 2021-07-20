// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#pragma once

#include <osu_memory/osu_daemon.hpp>
#include <osu_memory/stamp_receiver_impl.hpp>

namespace osu_memory
{
	/// <summary>
	/// Base class of any memory reader.
	/// @NotThreadSafe.
	/// </summary>
	class reader_base :
		public utils::enable_stamp_receiver_impl
	{
	protected:
		const os::readable_process& process{ os::osu_daemon::get_instance() };

	private:
		bool inited{};
		/// <summary>
		/// Implementation of resetting the reader.
		/// </summary>
		virtual void on_reset() = 0;
		/// <summary>
		/// Implementation of initializing the reader.
		/// </summary>
		/// <returns>Whether the initialization succeeds. A false means not ready and should be initialized again.</returns>
		virtual bool on_initialize() = 0;

	public:
		/// <summary>
		/// Reset the reader.
		/// </summary>
		void reset()
		{
			on_reset();
			inited = 0;
		}
		/// <summary>
		/// Initialize the reader.
		/// If the reader has been initialized, the behavior is not defined.
		/// </summary>
		/// <returns></returns>
		bool initialize()
		{
			return inited = on_initialize();
		}

	protected:
		/// <summary>
		/// Call this before doing any routine.
		/// Do not do any routine after call_before returns false.
		/// </summary>
		bool call_before()
		{
			if (update())
				reset();
			if (!inited)
				return initialize();
			else
				return true;
		}
	};
}