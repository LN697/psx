/*
    Description: Register Header File (Refactored for Indexing)
    Author: LN697
    Date: 25 December 2025
*/

#pragma once

#include <cstdint>
#include <cstring> // For memset

struct Registers {
    // Union allows access via name (regs.sp) OR index (regs.r[29])
    // This is vital for decoding instructions like "ADD r1, r2, r3"
    union {
        uint32_t r[32]; 
        struct {
            uint32_t zero;      // r0: Hardwired to 0
            uint32_t at;        // r1: Assembler temporary
            uint32_t v0, v1;    // r2-r3: Return values
            uint32_t a0, a1, a2, a3; // r4-r7: Arguments
            uint32_t t0, t1, t2, t3, t4, t5, t6, t7; // r8-r15: Temporaries
            uint32_t s0, s1, s2, s3, s4, s5, s6, s7; // r16-r23: Saved
            uint32_t t8, t9;    // r24-r25: Temporaries
            uint32_t k0, k1;    // r26-r27: Kernel reserved
            uint32_t gp;        // r28: Global pointer
            uint32_t sp;        // r29: Stack pointer
            uint32_t fp;        // r30: Frame pointer
            uint32_t ra;        // r31: Return address
        };
    };

    uint32_t pc;        // Program Counter
    uint32_t next_pc;   // Next PC (For Branch Delay Slot)
    
    uint32_t hi;        // Multiply/Divide High
    uint32_t lo;        // Multiply/Divide Low

    // --- Coprocessor 0 Registers (System Control) ---
    // The BIOS relies heavily on Status(12), Cause(13), and EPC(14)
    uint32_t sr;        // Status Register (Reg 12)
    uint32_t cause;     // Cause Register (Reg 13)
    uint32_t epc;       // Exception PC (Reg 14)
    uint32_t badvaddr;  // Bad Virtual Address (Reg 8)
    
    Registers() {
        std::memset(r, 0, sizeof(r));
        pc = 0xbfc00000;
        next_pc = pc + 4;
        sr = 0;
        cause = 0;
        epc = 0;
        badvaddr = 0;
        hi = 0;
        lo = 0;
    }
};