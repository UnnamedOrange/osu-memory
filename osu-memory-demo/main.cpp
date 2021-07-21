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
	}
}