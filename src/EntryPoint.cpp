import Experiments;
import RozeFoundUtils;
namespace u = RozeFoundUtils;

#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {

	std::string_view PATH = "D:\\Movies\\TOCONVERT\\Ignore\\Fullmetal Alchemist - Brotherhood\\Sound";

	auto xxhash_test = [&] {
		for (const auto& entry : std::filesystem::directory_iterator(PATH))
			auto hash = u::hash::XXH3(entry);
	};

	u::makeTimer("xxhash-XXH3", xxhash_test);
}