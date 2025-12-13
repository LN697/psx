/*
    Description: CPU Header File
    Author: LN697
    Date: 13 December 2025
*/

#pragma once

#include "registers.hpp"

class CPU {
    public:
        CPU();
        ~CPU();

        void init();
        void step();

        void fetch();

#ifdef DEBUG
        void dumpRegisters();
#endif

    private:
        Registers registers;
        int cycles;
        uint32_t instr;
        uint32_t pri_opcode, sec_opcode;
};