#pragma once
#include <vector>
#include <cstdint>
#include <array>
#include <cstddef>

class Bus {
public:
    Bus();
    ~Bus();

    void init();

    uint8_t read(uint32_t address);
    void write(uint32_t address, uint8_t data);
    
    uint32_t read32(uint32_t address);
    void write32(uint32_t address, uint32_t data);

private:
    // Page Table constants
    static constexpr size_t PAGE_SIZE = 64 * 1024;  // 64KB pages
    static constexpr size_t PAGE_COUNT = 65536;     // 4GB / 64KB

    // Initialize to nullptr
    std::array<uint8_t*, PAGE_COUNT> memoryMap = {nullptr};

    // --- Mapped to CPU ---
    std::vector<uint8_t> mainRAM;
    std::vector<uint8_t> scratchpad;
    std::vector<uint8_t> expRegion1, expRegion2, expRegion3;
    std::vector<uint8_t> io_ports;
    std::vector<uint8_t> biosROM;
    
    void mapRegion(std::vector<uint8_t>& storage, uint32_t startAddr, size_t size);
};