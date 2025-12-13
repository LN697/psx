/*
    Description: CPU Header File
    Author: LN697
    Date: 13 December 2025
*/

#pragma once

#include "registers.hpp"
#include "bus.hpp"

class CPU {
    public:
        CPU(Bus* bus);
        ~CPU();

        void init();
        void step();

        uint8_t read(uint32_t address);

        void fetch();
        void decode();
        void execute();

#ifdef DEBUG
        void dumpRegisters();
#endif

    private:
        Bus* bus = nullptr;

        Registers registers;
        int cycles;

        uint32_t instr;
        uint32_t pri_opcode, sec_opcode;
};
