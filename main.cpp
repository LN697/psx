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

    std::cout << "[Main] Starting PS1 emulator..." << std::endl;

    Bus bus;
    CPU cpu(&bus);

    bus.init();
    
    if (!bus.loadBIOS(argv[1])) {
        return 1;
    }

    bus.dumpMemoryRegion(0xBFC00000, 0xff);

    cpu.init();

    cpu.step();

    std::cout << "[Main] Stopping PS1 emulator..." << std::endl;

#ifdef PROFILE
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - start;
    std::cout << "[Profiler] Execution time: " << duration.count() << " ns" << std::endl;
#endif

    return 0;
}
