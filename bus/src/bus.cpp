#include "bus.hpp"
#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>

Bus::Bus() {}
Bus::~Bus() = default;

bool Bus::loadBIOS(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {
        std::cerr << "[Bus] Failed to open BIOS file: " << path << std::endl;
        return false;
    }

    std::streamsize size = file.tellg();
    if (size == -1) {
        std::cerr << "[Bus] Failed to determine file size." << std::endl;
        return false;
    }

    file.seekg(0, std::ios::beg);

    if (static_cast<size_t>(size) > biosROM.size()) {
        std::cerr << "[Bus] BIOS file too large! Expected 512KB, got " << size << " bytes." << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(biosROM.data()), size);

    return true;
}

void Bus::dumpMemoryRegion(uint32_t address, int range) {
    std::cout << "[Memory Dump] Address: 0x" << std::hex << address << " Range: " << std::dec << range << std::endl;
    for (int i = 0; i < range; ++i) {
        if (i % 0x10 == 0) {
            if (i != 0) std::cout << std::endl;
            std::cout << "0x" << std::hex << std::setfill('0') << std::setw(8) << (address + i) << ": ";
        }
        
        uint8_t value = read(address + i);
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)value << " ";
    }
    std::cout << std::endl;
}

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
    scratchpad.resize(1024);              // 1KB
    io_ports.resize(4 * 1024);            // 4KB
    biosROM.resize(512 * 1024);           // 512KB

    memoryMap.fill(nullptr);

    // Map Physical RAM (KUSEG: 0x00000000)
    mapRegion(mainRAM, 0x00000000, mainRAM.size());

    // KSEG0 (0x80000000): Cached Mirror of RAM
    mapRegion(mainRAM, 0x80000000, mainRAM.size());
    
    // KSEG1 (0xA0000000): Uncached Mirror of RAM
    mapRegion(mainRAM, 0xa0000000, mainRAM.size());

    // Map BIOS (0x1FC00000)
    mapRegion(biosROM, 0x1fc00000, biosROM.size());
    mapRegion(biosROM, 0x9fc00000, biosROM.size());
    mapRegion(biosROM, 0xbfc00000, biosROM.size());    
}

uint8_t Bus::read(uint32_t address) {
    page_index = address >> 16;
    offset = address & 0xFFFF;
    
    if (uint8_t* page = memoryMap[page_index]) {
        return page[offset];
    }
    
    if (address >= 0x1F801000 && address < 0x1F802000) {
        return io_ports[address & 0xFFF]; 
    }

    // Open bus behavior or garbage
    return 0x00; 
}

uint32_t Bus::read32(uint32_t address) {
    page_index = address >> 16;
    offset = address & 0xFFFF;

    // Fast path: aligned access within a valid page
    if (uint8_t* page = memoryMap[page_index]) {
        // Warning: This assumes host is Little Endian (like PSX)
        return *reinterpret_cast<uint32_t*>(&page[offset]);
    }

    // Fallback path
    return (uint32_t)read(address) | 
           ((uint32_t)read(address + 1) << 8) | 
           ((uint32_t)read(address + 2) << 16) | 
           ((uint32_t)read(address + 3) << 24);
}

void Bus::write(uint32_t address, uint8_t data) {
    page_index = address >> 16;
    offset = address & 0xFFFF;

    if (uint8_t* page = memoryMap[page_index]) {
        page[offset] = data;
        return;
    }

    if (address >= 0x1F801000 && address < 0x1F802000) {
         io_ports[address & 0xFFF] = data;
         return;
    }
}

void Bus::write32(uint32_t address, uint32_t data) {
    page_index = address >> 16;
    offset = address & 0xFFFF;

    if (uint8_t* page = memoryMap[page_index]) {
        *reinterpret_cast<uint32_t*>(&page[offset]) = data;
        return;
    }
    
    // Fallback: write byte by byte
    write(address, data & 0xFF);
    write(address + 1, (data >> 8) & 0xFF);
    write(address + 2, (data >> 16) & 0xFF);
    write(address + 3, (data >> 24) & 0xFF);
}