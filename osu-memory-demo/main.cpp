#include <iostream>
#include <vector>
#include <numeric>
#include <array>

#include <osu_memory/osu_memory.hpp>

int main(int argn, char** argv)
{
	using namespace osu_memory;
	osu_memory_collection reader;
	while (true)
	{
		std::system("cls");
		{
			auto version = reader.get_version_string();
			if (version)
			{
				std::cout << "version: " << *version << std::endl;
			}
		}
		{
			auto keys_4k = reader.get_mania_layout(4);
			if (keys_4k)
			{
				std::cout << "4k layout: ";
				for (char t : *keys_4k)
					std::cout << t;
				std::cout << std::endl;
			}
		}
		{
			auto keys_7k = reader.get_mania_layout(7);
			if (keys_7k)
			{
				std::cout << "7k layout: ";
				for (char t : *keys_7k)
					std::cout << t;
				std::cout << std::endl;
			}
		}
		{
			auto miss = reader.get_miss();
			if (miss)
			{
				std::cout << "miss: " << *miss << std::endl;
			}
			auto h50 = reader.get_50();
			if (h50)
			{
				std::cout << "50: " << *h50 << std::endl;
			}
			auto h100 = reader.get_100();
			if (h100)
			{
				std::cout << "100: " << *h100 << std::endl;
			}
			auto h200 = reader.get_200();
			if (h200)
			{
				std::cout << "200: " << *h200 << std::endl;
			}
			auto h300 = reader.get_300();
			if (h300)
			{
				std::cout << "300: " << *h300 << std::endl;
			}
			auto perfect = reader.get_perfect();
			if (perfect)
			{
				std::cout << "perfect: " << *perfect << std::endl;
			}
		}

		using namespace std::literals;
		std::this_thread::sleep_for(10ms);
	}
}