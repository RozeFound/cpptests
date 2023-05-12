#include "Experiments.hpp"
#include "Utils.hpp"

#include <ranges>
#include <algorithm>
#include <atomic>

#include <fmt/core.h>
namespace u = RozeFoundUtils;

constexpr bool is_prime (std::integral auto n) {

	if (n == 2) return true;
	else if (n <= 1 || ~n & 1)
		return false;

	for (auto i = 3; i < n / 2; i += 2)
		if (n % i == 0) return false;

	return true;
	
}

constexpr int count_primes (std::size_t range) {

	auto primes = std::vector<bool>(range); primes.flip();
	primes.at(0) = false; primes.at(1) = false;

	for (auto p = 2; p * p <= range; p++)
		if (primes[p]) for (auto i = p * p; i < range; i += p)
			primes.at(i) = false;

	return std::ranges::count(primes, true);

}

int main(int argc, char* argv[]) {
	
	auto args = std::vector(argv, argv + argc);

	constexpr size_t size = 1000000;
	auto numbers = std::vector<int>();
	for (size_t i = 0; i < size; i++) 
		numbers.push_back(i);

	u::makeTimer("Cound Primes", [&]{

		int count = 0;

		for (const auto& i : numbers)
			if (is_prime(i)) count++;

		fmt::print("Primes in {}: {}\n", size, count);

	});

	u::makeTimer("Cound Primes (parallel)", [&]{

		std::atomic<int> count = 0;
		
		u::parallel_for(0, size, [&] (int i) { if (is_prime(i)) count++; });

		fmt::print("Primes in {}: {}\n", size, count);

	});

	u::makeTimer("Cound Primes (sieve)", [&]{

		auto count = count_primes(size);
		fmt::print("Primes in {}: {}\n", size, count);

	});

	return 0;
}