/*
    Description: Main Entrypoint File
    Author: LN697
    Date: 13 December 2025
*/

#include <iostream>
#include <chrono>
#include "bus.hpp"
#include "cpu.hpp"

int main() {
#ifdef PROFILE
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "[Profiler] Profiler initialized" << std::endl;
#endif


    std::cout << "[Main] Starting PS1..." << std::endl;

    CPU cpu;
    Bus bus;

    bus.init();
    
    bus.read(rand());
    bus.write(rand(), rand());

#ifdef DEBUG
    cpu.dumpRegisters();
#endif

    std::cout << "[Main] Stopping PS1..." << std::endl;

#ifdef PROFILE
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - start;
    std::cout << "[Profiler] Execution time: " << duration.count() << " ns" << std::endl;
#endif

    return 0;
}