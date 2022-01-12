module;

#include <iostream>
#include <fstream>
#include <filesystem>
#include <functional>
#include <algorithm>
#include <optional>
#include <chrono>
#include <thread>
#include <array>

#define THIRD_PARTY

#ifdef THIRD_PARTY
#include <xxh3.h>
#include <boost/crc.hpp>
#endif

export module RozeFoundUtils;

namespace RozeFoundUtils {

	export auto print = [](const auto& ... Args) {
		((std::cout << Args << ' '), ...) << std::endl;
	};

	export class Timer {

	public:

		// Constructors

		Timer(): m_TimerName("Default Timer") { Start(); }
		Timer(std::string_view name) : m_TimerName(name) { Start(); }

		~Timer() { Stop(); }

	private:

		// Local methods

		void Start() {
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}
		void Stop() {

			std::chrono::time_point<std::chrono::high_resolution_clock> endTimepoint = std::chrono::high_resolution_clock::now();

			int64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			int64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			int64_t duration = end - start;
			double ms = duration * 0.001;

			std::cout << std::dec << m_TimerName << ": " << duration << "us (" << ms << "ms)" << std::endl;
		}

		// Local variables

		std::string_view m_TimerName;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
	};

	export void makeTimer(std::string_view name, std::function<void()> func) {
		Timer timer(name);
		func();
	}

	export void write_to_file(std::string_view string, std::filesystem::path filepath) {

		std::ofstream file;

		file.open(filepath, std::ios::binary);

		file.write(string.data(), string.size());

		file.close();
	}

	export std::optional<std::string> read_from_file(std::filesystem::path filepath) {

		std::ifstream file;
		file.open(filepath, std::ios::binary);

		if (file.is_open()) {
			std::stringstream ss;
			ss << file.rdbuf();
			return ss.str();
		}
		else {
			std::exception ex("File is not found");
			return std::nullopt;
		}
	}

	export void parallelFor(size_t start, size_t end, std::function<void(int)> function) {

		size_t thread_count = std::min((size_t)std::jthread::hardware_concurrency(), end);
		std::vector<std::jthread> threads;

		auto func = [&](size_t min, size_t max) {
			for (size_t i = min; i <= max; i++)
				function((int)i);
		};

		size_t base = end / thread_count;
		size_t leftover = end % thread_count;

		for (size_t i = 0; i < thread_count; i++) {

			size_t min = start + base * i;
			size_t max = start + base * (i + 1);
			if (i == thread_count) max += leftover;

			threads.push_back(std::jthread(func, min, max));
		}

		for (auto& thread : threads)
			thread.join();
	}

#ifdef THIRD_PARTY

	namespace hash {

		export uint32_t crc32(std::filesystem::path path) {

			std::ifstream file;
			file.open(path, std::ios::binary);

			boost::crc_32_type result;

			constexpr size_t buffSize = 1024;
			auto buffer = std::array<std::byte, buffSize>();

			do {
				file.read((char*)buffer.data(), buffSize);
				result.process_bytes((char*)buffer.data(), file.gcount());
			} while (file);

			file.close();

			return result.checksum();
		}

		export XXH64_hash_t XXH3(std::filesystem::path path) {

			std::ifstream file;
			file.open(path, std::ios::binary);

			auto state = XXH3_createState();

			constexpr size_t buffSize = 1024;
			auto buffer = std::array<std::byte, buffSize>();

			XXH3_64bits_reset(state);

			do {
				file.read((char*)buffer.data(), buffSize);
				XXH3_64bits_update(state, (xxh_u8*)buffer.data(), file.gcount());
			} while (file);

			file.close();

			auto hash = XXH3_64bits_digest(state);
			XXH3_freeState(state);

			return hash;
		}
	}

#endif

	namespace hex {

		enum class casing : unsigned int {
			upper, lower = 0x2020
		};

		namespace detail {

			constexpr char hex2int(const char& ch) {

				if (ch >= '0' && ch <= '9') return ch - '0';
				else if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
				else if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;

				return ch;
			}
		}

		std::string sharp_hex(std::byte* bytes, size_t length, casing Case) {

			std::string result; result.reserve(length);

			for (size_t i = 0; i < length; i++) {

				const unsigned int& value = (unsigned int)bytes[i];

				unsigned int difference = ((value & 0xf0) << 4) + (value & 0x0f) - 0x8989;
				unsigned int packedResult = ((((-(int)difference) & 0x7070U) >> 4) + difference + 0xB9B9U) | (unsigned int)Case;

				result.append(1, packedResult >> 8);
				result.append(1, packedResult & 0xFF);
			}

			return result;
		}

		std::string sharp_hex(std::string_view bytes, casing Case) {
			return std::move(sharp_hex((std::byte*)bytes.data(), bytes.size(), Case));
		}

		std::string hex(std::byte* bytes, size_t length, casing Case) {

			std::string_view hexDigits;

			switch (Case) {
				using enum casing;
			case upper: hexDigits = "0123456789ABCDEF"; break;
			case lower: hexDigits = "0123456789abcdef"; break;
			}

			std::string result; result.reserve(length);

			for (size_t i = 0; i < length; i++) {
				const int& ch = (int)bytes[i];
				result.append(1, hexDigits[(ch & 0xf0) >> 4]);
				result.append(1, hexDigits[ch & 0x0f]);
			}

			return result;
		}

		std::string hex(std::string_view bytes, casing Case) {
			return std::move(hex((std::byte*)bytes.data(), bytes.size(), Case));
		}

		std::string unhex(std::byte* bytes, size_t length) {

			std::string result; result.reserve(length);

			for (size_t i = 0; i < length; i += 2) {

				char fhch = detail::hex2int((int)bytes[i]) << 4;
				char shch = detail::hex2int((int)bytes[i + 1]);
				char result_char = fhch + shch;

				result.append(&result_char, 1);
			}

			return result;
		}

		std::string unhex(std::string_view bytes) {
			return std::move(unhex((std::byte*)bytes.data(), bytes.size()));
		}

		// templates

		template<class OutIt> OutIt sharp_hex_to(OutIt it, std::byte* bytes, size_t length, casing Case = casing::lower) {

			for (size_t i = 0; i < length; i++) {

				const unsigned int& value = bytes[i];

				unsigned int difference = ((value & 0xf0) << 4) + (value & 0x0f) - 0x8989;
				unsigned int packedResult = ((((-(int)difference) & 0x7070) >> 4) + difference + 0xb9b9) | (unsigned int)Case;

				*it++ = packedResult >> 8;
				*it++ = packedResult & 0xff;
			}
			*it++ = NULL;

			return it;
		}

		template<class OutIt> OutIt sharp_hex_to(OutIt it, std::string_view bytes, casing Case = casing::lower) {
			return std::move(sharp_hex_to(it, (std::byte*)bytes.data(), bytes.size(), Case));
		}

		template<class outIt> outIt hex_to(outIt it, std::byte* bytes, size_t length, casing Case = casing::lower) {

			std::string_view hexDigits;

			switch (Case) {
				using enum casing;
			case upper: hexDigits = "0123456789ABCDEF"; break;
			case lower: hexDigits = "0123456789abcdef"; break;
			}

			for (size_t i = 0; i < length; i++) {
				const int& ch = (int)bytes[i];
				*it++ = hexDigits[(ch & 0xf0) >> 4];
				*it++ = hexDigits[ch & 0x0f];
			}
			*it++ = NULL;

			return it;
		}

		template<class outIt> outIt hex_to(outIt it, std::string_view bytes, casing Case = casing::lower) {
			return std::move(hex_to(it, (std::byte*)bytes.data(), bytes.size(), Case));
		}

		template<class outIt> outIt unhex_to(outIt it, std::byte* bytes, size_t length) {

			for (size_t i = 0; i < length; i += 2) {

				char fhch = detail::hex2int((int)bytes[i]) << 4;
				char shch = detail::hex2int((int)bytes[i + 1]);
				*it++ = fhch + shch;
			}
			*it++ = NULL;

			return it;
		}

		template<class outIt> outIt unhex_to(outIt it, std::string_view bytes) {
			return std::move(unhex_to(it, (std::byte*)bytes.data(), bytes.size()));
		}
	}
}

export namespace ext {
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