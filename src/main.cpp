#include "Experiments.hpp"
#include "Utils.hpp"
#include "Tests.hpp"
#include <__functional/unwrap_ref.h>
#include <cstddef>
#include <cstring>

namespace u = RozeFoundUtils;

class A {
	int secret = 666;
	char hello[16] = "Hello World!\0";
	int second_secret = 1337;
public:
	int number = 42;
};

int main(int argc, char* argv[]) {

	A a;

	u::print("number is:", a.number);
	u::print("secret is:", u::get_at_offset<int>(a, 0));
	u::print("second secret is:", u::get_at_offset<int>(a, 20));
	u::print(u::get_at_offset<char*>(a, 4));

	return 0;
}