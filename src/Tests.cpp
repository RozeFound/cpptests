#include <random>
#include <thread>
#include <cmath>
#include <atomic>

//import RozeFoundUtils;
//import Experiments;

#include "Utils.hpp"
#include "Experiments.hpp"

#include <iostream>
#include <functional>
#include <algorithm>

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

	unsigned int thread_count = std::jthread::hardware_concurrency();
	std::vector<std::jthread> threads;

	u::Timer timer;

	size_t base = size / thread_count;
	size_t leftover = size % thread_count;

	for (size_t i = 0; i < thread_count; i++) {

		size_t min = base * i;
		size_t max = leftover + base * (i + 1);
		if (i == thread_count) max += leftover;

		threads.push_back(std::jthread(func, min, max));
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

	std::ranges::sort(arr);

	u::print("Array:", arr | ext::join);
}
