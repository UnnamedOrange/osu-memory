#include <iostream>
#include <vector>
#include <numeric>
#include <array>
#include <chrono>

#include <osu_memory/osu_memory.h>

int main(int argn, char** argv)
{
	UNREFERENCED_PARAMETER(argn);
	UNREFERENCED_PARAMETER(argv);

	using namespace osu_memory;
	using namespace std::chrono;
	using namespace std::literals;

	reader r;

	while (true)
	{
		std::system("cls");

		{
			auto t = r.get_hit_miss();
			if (t)
				std::cout << "miss: " << *t << std::endl;
		}
		{
			auto t = r.get_hit_50();
			if (t)
				std::cout << "50: " << *t << std::endl;
		}
		{
			auto t = r.get_hit_100();
			if (t)
				std::cout << "100: " << *t << std::endl;
		}
		{
			auto t = r.get_hit_200();
			if (t)
				std::cout << "200: " << *t << std::endl;
		}
		{
			auto t = r.get_hit_300();
			if (t)
				std::cout << "300: " << *t << std::endl;
		}
		{
			auto t = r.get_hit_perfect();
			if (t)
				std::cout << "perfect: " << *t << std::endl;
		}
		{
			auto t = r.get_mania_keys();
			if (t)
			{
				const auto& a = *t;
				for (const auto& k : a)
					std::cout << k.second;
				std::cout << std::endl;
			}
		}
		{
			auto t = r.get_mod();
			if (t)
			{
#define __mod_test(mod) if (*t & mod_t::mod) std::cout << #mod << std::endl
				__mod_test(no_fail);
				__mod_test(easy);
				__mod_test(touch_device);
				__mod_test(hidden);
				__mod_test(hard_rock);
				__mod_test(sudden_death);
				__mod_test(double_time);
				__mod_test(relax);
				__mod_test(half_time);
				__mod_test(nightcore);
				__mod_test(flashlight);
				__mod_test(auto_play);
				__mod_test(spun_out);
				__mod_test(auto_pilot);
				__mod_test(perfect);
				__mod_test(key4);
				__mod_test(key5);
				__mod_test(key6);
				__mod_test(key7);
				__mod_test(key8);
				__mod_test(fade_in);
				__mod_test(random);
				__mod_test(cinema);
				__mod_test(target_practice);
				__mod_test(key9);
				__mod_test(co_op);
				__mod_test(key1);
				__mod_test(key3);
				__mod_test(key2);
				__mod_test(score_v2);
				__mod_test(mirror);
#undef __mod_test
			}
			{
				auto [t1, t2] = std::tuple{ r.get_status(), r.get_previous_status() };
				if (t1 && t2)
				{
#define __status_test(t, status) else if (*t == status_t::status) std::cout << #status << std::endl
#define __status_wrapper(t)									    \
					if (false) {}							    \
					__status_test(t, menu);					    \
					__status_test(t, edit);					    \
					__status_test(t, play);					    \
					__status_test(t, exit);					    \
					__status_test(t, select_edit);			    \
					__status_test(t, select_play);			    \
					__status_test(t, select_drawings);		    \
					__status_test(t, rank);					    \
					__status_test(t, update);				    \
					__status_test(t, busy);					    \
					__status_test(t, unknown);				    \
					__status_test(t, lobby);				    \
					__status_test(t, match_setup);			    \
					__status_test(t, select_multi);			    \
					__status_test(t, ranking_vs);			    \
					__status_test(t, online_selection);		    \
					__status_test(t, options_offset_wizard);    \
					__status_test(t, ranking_tag_coop);		    \
					__status_test(t, ranking_team);			    \
					__status_test(t, beatmap_import);		    \
					__status_test(t, package_updater);		    \
					__status_test(t, benchmark);			    \
					__status_test(t, tourney);				    \
					__status_test(t, charts)						 

					__status_wrapper(t1);
					__status_wrapper(t2);
#undef __status_wrapper
#undef __status_test
				}
			}
		}
		{
			auto t = r.get_is_autoplay();
			if (t)
			{
				std::cout << "is_autoplay: " << (*t ? "true" : "false") << std::endl;
			}
		}
	}
}