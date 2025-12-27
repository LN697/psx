/*
    Description: MIPS Opcode Mapping Initialization
    Author: LN697
    Date: 17 December 2025
*/

#pragma once

#include "cpu.hpp"
#include "instructions.hpp"

static void init_opcodes(CPU& cpu) {
    // 1. Initialize all to Illegal by default
    cpu.pri_table.fill(&Instructions::illegal);
    cpu.sec_table.fill(&Instructions::illegal);

    // 2. Primary Opcodes Mapping
    using namespace Instructions;

    // cpu.pri_table[0x00] = &special; // Dispatches to secondary
    // cpu.pri_table[0x01] = &bcondz;  // REGIMM (BLTZ, BGEZ, etc)
    // cpu.pri_table[0x02] = &j;
    // cpu.pri_table[0x03] = &jal;
    // cpu.pri_table[0x04] = &beq;
    // cpu.pri_table[0x05] = &bne;
    // cpu.pri_table[0x06] = &blez;
    // cpu.pri_table[0x07] = &bgtz;

    cpu.pri_table[0x08] = &addi;
    cpu.pri_table[0x09] = &addiu;
    cpu.pri_table[0x0A] = &slti;
    cpu.pri_table[0x0B] = &sltiu;
    cpu.pri_table[0x0C] = &andi;
    cpu.pri_table[0x0D] = &ori;
    cpu.pri_table[0x0E] = &xori;
    cpu.pri_table[0x0F] = &lui;

    // cpu.pri_table[0x10] = &cop0;
    // cpu.pri_table[0x11] = &cop1;
    // cpu.pri_table[0x12] = &cop2;
    // cpu.pri_table[0x13] = &cop3;
    
    cpu.pri_table[0x20] = &lb;
    cpu.pri_table[0x24] = &lbu;
    cpu.pri_table[0x21] = &lh;
    cpu.pri_table[0x22] = &lwl;
    cpu.pri_table[0x23] = &lw;
    cpu.pri_table[0x25] = &lhu;
    cpu.pri_table[0x26] = &lwr;

    cpu.pri_table[0x28] = &sb;
    cpu.pri_table[0x29] = &sh;
    cpu.pri_table[0x2A] = &swl;
    cpu.pri_table[0x2B] = &sw;
    cpu.pri_table[0x2E] = &swr;

    // 3. Secondary Opcodes (Function Field) Mapping
    
    // cpu.sec_table[0x00] = &sll;
    // cpu.sec_table[0x02] = &srl;
    // cpu.sec_table[0x03] = &sra;
    // cpu.sec_table[0x04] = &sllv;
    // cpu.sec_table[0x06] = &srlv;
    // cpu.sec_table[0x07] = &srav;

    // cpu.sec_table[0x08] = &jr;
    // cpu.sec_table[0x09] = &jalr;
    // cpu.sec_table[0x0C] = &syscall;
    // cpu.sec_table[0x0D] = &break;

    // cpu.sec_table[0x10] = &mfhi;
    // cpu.sec_table[0x11] = &mthi;
    // cpu.sec_table[0x12] = &mflo;
    // cpu.sec_table[0x13] = &mtlo;

    // cpu.sec_table[0x18] = &mult;
    // cpu.sec_table[0x19] = &multu;
    // cpu.sec_table[0x1A] = &div;
    // cpu.sec_table[0x1B] = &divu;

    // cpu.sec_table[0x20] = &add;
    // cpu.sec_table[0x21] = &addu;
    // cpu.sec_table[0x22] = &sub;
    // cpu.sec_table[0x23] = &subu;
    // cpu.sec_table[0x24] = &_and;
    // cpu.sec_table[0x25] = &_or;
    // cpu.sec_table[0x26] = &xor;
    // cpu.sec_table[0x27] = &nor;

    // cpu.sec_table[0x2A] = &slt;
    // cpu.sec_table[0x2B] = &sltu;
}