#include "Experiments.hpp"
#include "Utils.hpp"

#include <xxh3.h>
#include "sha1.hpp"
#include "sha512.hpp"

namespace u = RozeFoundUtils;
uint32_t MurmurHash2 (const void * key, int len, uint32_t seed);

int main(int argc, char* argv[]) {

	if (auto file = u::read_from_file("/home/rozefound/Desktop/Sodium.jar")) {

		u::makeTimer("xxhash",  [&]{
			auto hash = XXH3_64bits(file->data(), file->size());
		});

		u::makeTimer("sha1", [&]{
			SHA1 checksum;
			checksum.update(*file);
			auto hash = checksum.final();
		});

		u::makeTimer("sha512", [&]{
			auto hash = sw::sha512::calculate(*file);
		});

		u::makeTimer("murmur2", [&]{
			auto data = *file;
			for (char c: {9, 10, 13, 32})
				data.erase(std::remove(data.begin(), data.end(), c), data.end());
			auto hash = MurmurHash2(data.data(), data.size(), 1);
		});
	};
		
}