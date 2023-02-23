#include "Experiments.hpp"
#include "Utils.hpp"
#include "Tests.hpp"

#include <vector>

namespace u = RozeFoundUtils;

int main(int argc, char* argv[]) {
	
	auto args = std::vector(argv, argv + argc);

	auto process_id = u::get_process_id("spotify");
	auto process_module = u::get_module_base(process_id);

	std::cout << std::hex << process_module << std::endl;

	return 0;
}