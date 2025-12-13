/*
    Description: Register Header File
    Author: LN697
    Date: 13 December 2025
*/

#pragma once

#include <cstdint>

struct Registers {
    const uint32_t zero = 0x0;                       // constant 0
    uint32_t at = 0x0;                               // assembler temporary
    uint32_t v0 = 0x0;                               // subroutine return values
    uint32_t v1 = 0x0;                               // subroutine return values
    uint32_t a0 = 0x0;                               // subroutine arguments
    uint32_t a1 = 0x0;                               // subroutine arguments
    uint32_t a2 = 0x0;                               // subroutine arguments
    uint32_t a3 = 0x0;                               // subroutine arguments
    uint32_t t0 = 0x0;                               // temporaries
    uint32_t t1 = 0x0;                               // temporaries
    uint32_t t2 = 0x0;                               // temporaries
    uint32_t t3 = 0x0;                               // temporaries
    uint32_t t4 = 0x0;                               // temporaries
    uint32_t t5 = 0x0;                               // temporaries
    uint32_t t6 = 0x0;                               // temporaries
    uint32_t t7 = 0x0;                               // temporaries
    uint32_t s0 = 0x0;                               // static variables
    uint32_t s1 = 0x0;                               // static variables
    uint32_t s2 = 0x0;                               // static variables
    uint32_t s3 = 0x0;                               // static variables
    uint32_t s4 = 0x0;                               // static variables
    uint32_t s5 = 0x0;                               // static variables
    uint32_t s6 = 0x0;                               // static variables
    uint32_t s7 = 0x0;                               // static variables
    uint32_t t8 = 0x0;                               // temporaries
    uint32_t t9 = 0x0;                               // temporaries
    uint32_t k0 = 0x0;                               // reserved for kernel
    uint32_t k1 = 0x0;                               // reserved for kernel
    uint32_t gp = 0x0;                               // global pointer
    uint32_t sp = 0x0;                               // stack pointer
    uint32_t fp_s8 = 0x0;                            // frame pointer, or 9th static variable
    uint32_t ra = 0x0;                               // return address
    uint32_t pc = 0x0;                               // program counter
    uint32_t hi = 0x0;                               // multiply/divide results
    uint32_t lo = 0x0;                               // multiply/divide results
};
