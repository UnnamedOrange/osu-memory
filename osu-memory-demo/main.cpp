#include <iostream>
#include <vector>
#include <numeric>
#include <array>
#include <chrono>

#include <osu_memory/osu_memory.h>

int main(int argn, char** argv)
{
	using namespace osu_memory;
	using namespace std::chrono;
	using namespace std::literals;

	while (true)
	{
		std::system("cls");

		{
			auto t = reader::get_miss(false);
			if (t)
				std::cout << "miss: " << *t << std::endl;
		}
		{
			auto t = reader::get_50(false);
			if (t)
				std::cout << "50: " << *t << std::endl;
		}
		{
			auto t = reader::get_100(false);
			if (t)
				std::cout << "100: " << *t << std::endl;
		}
		{
			auto t = reader::get_200(false);
			if (t)
				std::cout << "200: " << *t << std::endl;
		}
		{
			auto t = reader::get_300(false);
			if (t)
				std::cout << "300: " << *t << std::endl;
		}
		{
			auto t = reader::get_perfect(false);
			if (t)
				std::cout << "perfect: " << *t << std::endl;
		}
	}
}