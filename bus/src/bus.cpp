#include "bus.hpp"
#include <iostream>
#include <cstring>

Bus::Bus() {}
Bus::~Bus() = default;

void Bus::mapRegion(std::vector<uint8_t>& storage, uint32_t startAddr, size_t size) {
    size_t numPages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    uint32_t startIndex = startAddr >> 16;

    for (size_t i = 0; i < numPages; i++) {
        if (startIndex + i < PAGE_COUNT) {
            memoryMap[startIndex + i] = &storage[i * PAGE_SIZE];
        }
    }
}

void Bus::init() {
    mainRAM.resize(2 * 1024 * 1024);      // 2MB
    expRegion1.resize(8 * 1024 * 1024);   // 8MB
    scratchpad.resize(1024);              // 1KB (Special handling needed typically)
    io_ports.resize(4 * 1024);            // 4KB
    biosROM.resize(512 * 1024);           // 512KB

    memoryMap.fill(nullptr);

    // Map Physical RAM (KUSEG: 0x00000000)
    mapRegion(mainRAM, 0x00000000, mainRAM.size());

    // KSEG0 (0x80000000): Cached Mirror of RAM
    mapRegion(mainRAM, 0x80000000, mainRAM.size());
    
    // KSEG1 (0xA0000000): Uncached Mirror of RAM
    mapRegion(mainRAM, 0xA0000000, mainRAM.size());

    // Map BIOS (0x1FC00000)
    mapRegion(biosROM, 0x1FC00000, biosROM.size());
    
    // Note: IO Ports (0x1F80xxxx) are usually NOT mapped this way 
    // because they require side-effects (hardware logic) on read/write.
    // We leave those as nullptr in the map to trigger the fallback logic.
}

uint8_t Bus::read(uint32_t address) {
    uint32_t page_index = address >> 16;
    uint32_t offset = address & 0xFFFF;
    
    if (uint8_t* page = memoryMap[page_index]) {
        return page[offset];
    }
    
    if (address >= 0x1F801000 && address < 0x1F802000) {
        return io_ports[address & 0xFFF]; 
    }

    // ... other MMIO checks ...

    return 0xFF; // Open bus behavior usually returns garbage or 0
}

uint32_t Bus::read32(uint32_t address) {
    uint32_t page_index = address >> 16;
    uint32_t offset = address & 0xFFFF;

    if (uint8_t* page = memoryMap[page_index]) {
        return *reinterpret_cast<uint32_t*>(&page[offset]);
    }

    return (uint32_t)read(address) | 
           ((uint32_t)read(address + 1) << 8) | 
           ((uint32_t)read(address + 2) << 16) | 
           ((uint32_t)read(address + 3) << 24);
}

void Bus::write(uint32_t address, uint8_t data) {
    uint32_t page_index = address >> 16;
    uint32_t offset = address & 0xFFFF;

    if (uint8_t* page = memoryMap[page_index]) {
        page[offset] = data;
        return;
    }

    if (address >= 0x1F801000 && address < 0x1F802000) {
         io_ports[address & 0xFFF] = data;
         // TODO: Trigger Hardware Side Effects here!
         return;
    }
}