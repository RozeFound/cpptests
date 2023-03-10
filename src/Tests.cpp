#include <random>
#include <thread>
#include <cmath>
#include <atomic>
#include <ranges>

//import RozeFoundUtils;
//import Experiments;

#include "Utils.hpp"
#include "Experiments.hpp"

#include <iostream>
#include <functional>
#include <algorithm>

#ifdef THIRD_PARTY
#include <xxh3.h>
#endif
#include "sha1.hpp"
#include "sha512.hpp"
#include "murmurhash2.hpp"

namespace u = RozeFoundUtils;

bool is_prime(int n) {

	if (n == 2) return true;
	else if (n <= 1 || n % 2 == 0) return false;

	for (int i = 3; i < std::sqrt(n); i += 2)
		if (n % i == 0) return false;

	return true;
}

void MultiThreading() {

	constexpr size_t size = 10000000;
	std::atomic<int> count = 0;

	auto func = [&](size_t min, size_t max) {
		for (; min <= max; min++) {
			if (is_prime((int)min)) {
				count++;
			}
		}
	};

	unsigned int thread_count = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;

	u::Timer timer;

	size_t base = size / thread_count;
	size_t leftover = size % thread_count;

	for (size_t i = 0; i < thread_count; i++) {

		size_t min = base * i;
		size_t max = leftover + base * (i + 1);
		if (i == thread_count) max += leftover;

		threads.push_back(std::thread(func, min, max));
	}


	for (auto& thread : threads)
		thread.join();

	u::print("Count:", count);
}

void ArrayTest() {

	constexpr size_t size = 10;

	std::mt19937 generator(std::random_device{}());
	std::uniform_int_distribution distribution(1, (int)size);
	auto rand = std::bind(distribution, generator);

	auto arr = array(size);
	for (int& n : arr) n = rand();

	// std::ranges::sort(arr);
	std::sort(arr.begin(), arr.end());

	u::print("Array:", arr | ext::join);
}

void test_hashes() {
	
	if (auto file = u::read_from_file("../data/Sodium.jar")) {

#ifdef THIRD_PARTY
		u::makeTimer("xxhash",  [&]{
			auto hash = XXH3_64bits(file->data(), file->size());
		});
#endif

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

class A {
	
	int secret = 666;
	char hello[16] = "Hello World!\0";
	int second_secret = 1337;

	// still being deleted in release mode...
	void func(bool stop_compiler_from_deleting) {
		if(!stop_compiler_from_deleting)
			std::cout << "You got me!" << std::endl;
	}

	public:

	A ( ) { func(true); }

};

void test_access_private_members() {

	A a;

	auto signature = u::to_bytes("55 48 89 E5 48 83 EC 10 40 88 f0 48 89 7D F8 24 01");
	auto func_address = u::basic_sigscan(u::get_module_base(), signature);
	auto private_func = (void(__cdecl*)(void* _this))func_address;

	u::print("secret is:", u::get_at_offset<int>(a, 0));
	u::print("second secret is:", u::get_at_offset<int>(a, 20));
	u::print(u::get_at_offset<char*>(a, 4));
	private_func(&a);

}
