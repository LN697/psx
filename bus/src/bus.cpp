/*
    Description: Bus Implementation File
    Author: LN697
    Date: 13 December 2025
*/

#include "bus.hpp"

Bus::Bus() {};

Bus::~Bus() = default;

void Bus::init() {
    // --- Reserve the memory ---
    mainRAM.reserve(2048 * 1024);
    expRegion1.reserve(8192 * 1024);
    scratchpad.reserve(1024);
    io_ports.reserve(4 * 1024);
    expRegion2.reserve(8 * 1024);
    expRegion3.reserve(2048);
    biosROM.reserve(512 * 1024);

    // --- Resize the vectors ---
    mainRAM.resize(2048 * 1024, 0x0);
    expRegion1.resize(8192 * 1024, 0x0);
    scratchpad.resize(1024, 0x0);
    io_ports.resize(4 * 1024, 0x0);
    expRegion2.resize(8 * 1024, 0x0);
    expRegion3.resize(2048, 0x0);
    biosROM.resize(512 * 1024, 0x0);
}

uint8_t Bus::read(uint32_t address) {
    // --- KUSEG ---
    if (address < 0x200000) {
        return mainRAM[address];
    } else if (address >= 0x1f000000 && address < 0x1f800000) {
        return expRegion1[address];
    } else if (address < 0x1f800400) {
        return scratchpad[address];
    } else if (address >= 0x1f801000 && address < 0x1f802000) {
        return io_ports[address];
    } else if (address < 0x1f804000) {
        return expRegion2[address];
    } else if (address >= 0x1fa00000 && address < 0x1fc00000) {
        return expRegion3[address];
    } else if (address < 0x1fc80000) {
        return biosROM[address];
    }

    return 0x0;
}

uint16_t Bus::read16(uint32_t address) {
    uint16_t byte0 = read(address);
    uint16_t byte1 = read(address + 1);

    return byte0 | (byte1 << 8);
}

uint32_t Bus::read32(uint32_t address) {
    uint32_t byte0 = read(address);
    uint32_t byte1 = read(address + 1);
    uint32_t byte2 = read(address + 2);
    uint32_t byte3 = read(address + 3);

    return byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24);
}

void Bus::write(uint32_t address, uint8_t data) {
    // --- KUSEG ---
    if (address < 0x200000) {
        mainRAM[address] = data;
    } else if (address >= 0x1f000000 && address < 0x1f800000) {
        expRegion1[address] = data;
    } else if (address < 0x1f800400) {
        scratchpad[address] = data;
    } else if (address >= 0x1f801000 && address < 0x1f802000) {
        io_ports[address] = data;
    } else if (address < 0x1f804000) {
        expRegion2[address] = data;
    } else if (address >= 0x1fa00000 && address < 0x1fc00000) {
        expRegion3[address] = data;
    } else if (address < 0x1fc80000) {
        biosROM[address] = data;
    }
}
