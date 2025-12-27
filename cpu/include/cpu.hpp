/*
    Description: CPU Header File
    Author: LN697
    Date: 13 December 2025
*/

#pragma once

#include "registers.hpp"
#include "bus.hpp"
#include <array>
#include <functional>

class CPU;
using InstructionHandler = void (*)(CPU&);

class CPU {
    public:
        CPU(Bus* bus);
        ~CPU();

        void init();
        void step();

        uint8_t read(uint32_t address);
        void write(uint32_t address, uint8_t data);
        
        uint16_t read16(uint32_t address);
        void write16(uint32_t address, uint16_t data);

        uint32_t read32(uint32_t address);
        void write32(uint32_t address, uint32_t data);

        void scheduleLoad(uint32_t reg, uint32_t value);

#ifdef DEBUG
        void dumpRegisters();
#endif

    private:
        void fetch();
        void decode();
        void execute();

    public:
        Bus* bus = nullptr;
        Registers registers;

        uint32_t instr, next_pc;

        std::array<InstructionHandler, 64> pri_table, sec_table;
    
    private:
        int cycles;
        uint32_t pri_opcode, sec_opcode;

        // For Load Delay Slot (LDS)
        bool in_LDS = false;
        uint32_t delayed_load_tar_reg = 0;
        uint32_t delayed_load_val = 0;
        int lds_countdown = 0;
};
