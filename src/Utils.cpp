#include "Utils.hpp"

#include <filesystem>
#include <fstream>
#include <thread>
#include <vector>

namespace RozeFoundUtils {

    uint32_t get_process_id (std::string_view process_name) {

        namespace fs = std::filesystem;

        for (const auto& entry : fs::directory_iterator("/proc/")) {

            if (!entry.is_directory()) continue;

            for (const auto& entry : fs::directory_iterator(entry.path())) {

                std::error_code ec; auto file_type = entry.status(ec).type();
                if (file_type != fs::file_type::regular || entry.path().filename() != "status") continue; 

                if (auto file = read_from_file(entry.path())) {
                    if (file->find(process_name) != std::string::npos) {
                        return std::stol(entry.path().parent_path().stem().string());
                    }
                }
            }
        }

        return 0xDEADC0DE;

    }

    std::ptrdiff_t get_module_base (uint32_t process_id, std::string_view module) {

        auto maps_path = "/proc/" + std::to_string(process_id) + "/maps";
        if (process_id == 0) maps_path = "/proc/self/maps";

        auto maps = read_from_file(maps_path);

        std::size_t start;

        if (module.empty()) start = maps->find("r-xp");
        else start = maps->find(module);

        std::size_t end = start;

        while (maps->at(start) != '\n') start--;
        while (maps->at(end) != '-') end--;

        return std::stol(maps->substr(start, end - start), 0, 16);

    }

    void makeTimer(std::string_view name, std::function<void()> func) {
        Timer timer(name);
        func();
    }

    void write_to_file(std::string_view string, std::filesystem::path filepath) {

      std::ofstream file;

      file.open(filepath, std::ios::binary);

      file.write(string.data(), string.size());

      file.close();
    }

    std::optional<std::string> read_from_file(std::filesystem::path filepath) {

        std::ifstream file;
        file.open(filepath, std::ios::binary);

        if (file.is_open()) {
            std::stringstream ss;
            ss << file.rdbuf();
            return ss.str();
        } else {
            throw std::runtime_error("File is not found");
            return std::nullopt;
        }
    }

    void parallelFor(size_t start, size_t end, std::function<void(int)> function) {

        size_t thread_count = std::min((size_t)std::thread::hardware_concurrency(), end);
        std::vector<std::thread> threads;

        auto func = [&](size_t min, size_t max) {
            for (size_t i = min; i <= max; i++)
                function((int)i);
        };

        size_t base = end / thread_count;
        size_t leftover = end % thread_count;

        for (size_t i = 0; i < thread_count; i++) {

            size_t min = start + base * i;
            size_t max = start + base * (i + 1);
            if (i == thread_count)
                max += leftover;

            threads.push_back(std::thread(func, min, max));
        }

        for (auto &thread : threads)
            thread.join();
    }

#ifdef THIRD_PARTY

    namespace hash {

		uint32_t crc32(std::filesystem::path path) {

			std::ifstream file;
			file.open(path, std::ios::binary);

			uint32_t result = 0;

			constexpr size_t buffSize = 1024;
			auto buffer = std::array<std::byte, buffSize>();

			do {
				file.read((char*)buffer.data(), buffSize);
				result = crc32c::Extend(result, (uint8_t*)buffer.data(), file.gcount());
			} while (file);

			file.close();

			return result;
		}

		XXH64_hash_t XXH3(std::filesystem::path path) {

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

        std::string sharp_hex(std::byte *bytes, size_t length, casing Case) {

            std::string result;
            result.reserve(length);

            for (size_t i = 0; i < length; i++) {

                const unsigned int &value = (unsigned int)bytes[i];

                unsigned int difference = ((value & 0xf0) << 4) + (value & 0x0f) - 0x8989;
                unsigned int packedResult = ((((-(int)difference) & 0x7070U) >> 4) + difference + 0xB9B9U) | (unsigned int)Case;

                result.append(1, packedResult >> 8);
                result.append(1, packedResult & 0xFF);
            }

            return result;
        }

        std::string sharp_hex(std::string_view bytes, casing Case) {
            return std::move(sharp_hex((std::byte *)bytes.data(), bytes.size(), Case));
        }

        std::string hex(std::byte *bytes, size_t length, casing Case) {

            std::string_view hexDigits;

            switch (Case) {
                using enum casing;
                case upper: hexDigits = "0123456789ABCDEF"; break;
                case lower: hexDigits = "0123456789abcdef"; break;
            }

            std::string result;
            result.reserve(length);

            for (size_t i = 0; i < length; i++) {
                const int &ch = (int)bytes[i];
                result.append(1, hexDigits[(ch & 0xf0) >> 4]);
                result.append(1, hexDigits[ch & 0x0f]);
            }

            return result;
        }

        std::string hex(std::string_view bytes, casing Case) {
          return std::move(hex((std::byte *)bytes.data(), bytes.size(), Case));
        }

        std::string unhex(std::byte *bytes, size_t length) {

            std::string result;
            result.reserve(length);

            for (size_t i = 0; i < length; i += 2) {

                char fhch = detail::hex2int((int)bytes[i]) << 4;
                char shch = detail::hex2int((int)bytes[i + 1]);
                char result_char = fhch + shch;

                result.append(&result_char, 1);
            }

          return result;
        }
        std::string unhex(std::string_view bytes) {
            return std::move(unhex((std::byte *)bytes.data(), bytes.size()));
        }
    } // namespace hex
} // namespace RozeFoundUtils
