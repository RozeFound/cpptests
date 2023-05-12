#pragma once

#include <iostream>
#include <chrono>
#include <functional>
#include <optional>
#include <filesystem>

#include "extensions.hpp"

#ifdef THIRD_PARTY
#include <xxh3.h>
#include <crc32c/crc32c.h>
#endif

namespace RozeFoundUtils {

	auto print = [](const auto& ... Args) {
		((std::cout << Args << ' '), ...) << std::endl;
	};

	template<typename T> auto get_at_offset(auto& _class, std::ptrdiff_t offset) {
		auto offset_value = reinterpret_cast<std::ptrdiff_t>(&_class) + offset;
		if constexpr (std::is_same<T, char*>::value) return (char*)offset_value;
		else return std::ref(*reinterpret_cast<T*>(offset_value));
	}

	uint32_t get_process_id (std::string_view process_name);
	std::ptrdiff_t get_module_base (uint32_t process_id = 0, std::string_view module = "");

	template<typename T> concept suitable = requires (T container) { container.size(); };
	std::ptrdiff_t basic_sigscan(std::ptrdiff_t start, suitable auto signature) {

		std::size_t size = signature.size();
        auto hits = std::vector<bool>(size);
        auto _addr = (std::byte*)start;

        while (!hits.back()) {
            
			std::size_t count = 0;
            for (const auto byte : signature)
                if (*(_addr++) == byte) 
					hits.at(count++) = true;
                else break;

        }

        return std::ptrdiff_t(_addr - size);

    }

	constexpr auto to_bytes (std::integral auto&& ... Ts) noexcept {
		return std::array { std::byte(Ts) ... };
	}

	constexpr auto to_bytes (const std::string_view hex_values) noexcept {

		auto bytes = std::vector<std::byte>();

		for (const auto& hex_value : hex_values | ext::split)
			bytes.push_back(std::byte(std::stoi(hex_value.data(), 0, 16)));

		return bytes;

	}

	class Timer {

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

        void makeTimer(std::string_view name, std::function<void()> func);

        void write_to_file(std::string_view string,std::filesystem::path filepath);

        std::optional<std::string> read_from_file(std::filesystem::path filepath);

        void parallel_for(size_t start, size_t end, std::function<void(int)> function);

#ifdef THIRD_PARTY

	namespace hash {

		uint32_t crc32(std::filesystem::path path);

		XXH64_hash_t XXH3(std::filesystem::path path);
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

		std::string sharp_hex(std::byte *bytes, size_t length, casing Case);
		std::string sharp_hex(std::string_view bytes, casing Case);

		std::string hex(std::byte *bytes, size_t length, casing Case);
		std::string hex(std::string_view bytes, casing Case);

		std::string unhex(std::byte *bytes, size_t length);
		std::string unhex(std::string_view bytes);

        // templates

		template<class OutIt> OutIt sharp_hex_to(OutIt it, std::byte* bytes, size_t length, casing Case = casing::lower) {

			for (size_t i = 0; i < length; i++) {

				const unsigned int& value = (unsigned int)bytes[i];

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
