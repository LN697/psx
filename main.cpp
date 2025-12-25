/*
    Description: Main Entrypoint File
    Author: LN697
    Date: 13 December 2025
*/

#include <iostream>
#include <chrono>
#include <csignal>

#include "bus.hpp"
#include "cpu.hpp"
#include "opcodes.hpp"

volatile std::sig_atomic_t g_signal_received = 0;
void signal_handler(int signal) { g_signal_received = signal; }

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

    cpu.init();
    init_opcodes(cpu);

    while (g_signal_received == 0) {
        cpu.step();
    }
    
    cpu.bus->dumpMemoryRegion(cpu.registers.pc, 0x100);

    std::cout << "[Main] Stopping PS1 emulator..." << std::endl;

#ifdef PROFILE
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - start;
    std::cout << "[Profiler] Execution time: " << duration.count() << " ns" << std::endl;
#endif

    return 0;
}   
