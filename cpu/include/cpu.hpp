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

        uint32_t read32(uint32_t address);
        void write32(uint32_t address, uint32_t data);

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
};
