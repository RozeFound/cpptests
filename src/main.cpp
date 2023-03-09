#include "Experiments.hpp"
#include "Utils.hpp"

#include <cstddef>
#include <fmt/core.h>
namespace u = RozeFoundUtils;

void test_access_private_members();

int main(int argc, char* argv[]) {
	
	auto args = std::vector(argv, argv + argc);

	std::ptrdiff_t process_module = 0;
	uint32_t process_id = 0;

	{
		auto timer = u::Timer("Find Module Base of Python");
		process_id = u::get_process_id("python");
		if (process_id == 0xDEADC0DE) return -1;
		process_module = u::get_module_base(process_id);
	}

	fmt::print("process_id of Python is: {}\n", process_id);	
	fmt::print("module_base of Python is: 0x{:x}\n", process_module);

	test_access_private_members();

	return 0;
}