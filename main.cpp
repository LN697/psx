/*
    Description: Main Entrypoint File
    Author: LN697
    Date: 13 December 2025
*/

#include <iostream>
#include <chrono>
#include "bus.hpp"
#include "cpu.hpp"

int main(int argc, char* argv[]) {
#ifdef PROFILE
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "[Profiler] Profiler initialized" << std::endl;
#endif

    if (argc < 2) {
        std::cerr << "Usage: "<< argv[0] << " <bios_file> <rom_file>" << std::endl;
        return 1;
    }

    if (argc == 2) {
        std::cout << "[Loader] No rom file supplied, running in BIOS mode" << std::endl;
    }

    std::cout << "[Main] Starting PSX emulator..." << std::endl;

    Bus bus;
    CPU cpu(&bus);

    bus.init();
    bus.write(0x0, 0xef);
    bus.write(0x1, 0xbe);
    bus.write(0x2, 0xad);
    bus.write(0x3, 0xde);

    cpu.fetch();
    cpu.decode();

    std::cout << "[Main] Stopping PSX emulator..." << std::endl;

#ifdef PROFILE
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - start;
    std::cout << "[Profiler] Execution time: " << duration.count() << " ns" << std::endl;
#endif

    return 0;
}
