#pragma once

#include <cstddef>
#include <string>
#include <sstream>
#include <vector>

namespace ext {
	struct join_t {
		template<typename T>
		std::string operator()(const T& Data) const {

			std::stringstream ss;

			ss << '[';

			for (size_t i = 0; i < Data.size() - 1; i++)
				ss << Data[i] << ", ";

			ss << Data.back() << ']';

			return ss.str();
		}
	};

	const join_t join = {};

	template<typename T>
	std::string operator|(const T& Data, join_t f) {
		return f(Data);
	}

	struct split_t {

		auto operator() (std::string_view string) const {

			auto parts = std::vector<std::string_view>();

			for (std::size_t i = 0, word_length = 0; i <= string.size(); i++) {

				if (i == string.size() || string.at(i) == ' ') {
					auto word = string.substr(i - word_length, word_length);
					parts.push_back(word); word_length = 0;
				} else word_length++;

			}
			
			return parts;

		}

	};

	const split_t split = { };

    template<typename T>
	auto operator| (T string, split_t f) {
		return f(string);
	}

	struct widen_t {
		std::wstring operator()(std::string_view string) const {

			std::wstring result;

			wchar_t wch;

			for (char ch : string) {
				size_t len = std::mbtowc(&wch, &ch, 4);
				result.append(1, wch);
			}

			return result;
		}
	};

	const widen_t widen = {};

	template<typename T>
	std::wstring operator|(const T& Data, widen_t f) {
		return f(Data);
	}
}