/*
    Description: Bus Header File
    Author: LN697
    Date: 13 December 2025
*/

#pragma once

#include <vector>
#include <cstdint>

class Bus {
    public:
        Bus();
        ~Bus();

        void init();

        uint8_t read(uint32_t address);
        uint16_t read16(uint32_t address);
        uint32_t read32(uint32_t address);
        void write(uint32_t address, uint8_t data);

    private:
        // --- Mapped to CPU ---
        std::vector<uint8_t> mainRAM;
        std::vector<uint8_t> scratchpad;
        std::vector<uint8_t> expRegion1, expRegion2, expRegion3;
        std::vector<uint8_t> io_ports;
        std::vector<uint8_t> intCPUCtrlRegs;
        std::vector<uint8_t> biosROM;

        // --- Not mapped to CPU ---
        std::vector<uint8_t> vRAM;
        std::vector<uint8_t> soundRAM;
        std::vector<uint8_t> cdromCtrlRAM;
        std::vector<uint8_t> cdromCtrlROM;
        std::vector<uint8_t> cdromBuffer;
        std::vector<uint8_t> eepROMs;
};
