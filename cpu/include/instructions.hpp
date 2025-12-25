/*
    Description: MIPS Instruction Implementations
    Author: LN697
    Date: 13 December 2025
*/

#pragma once

#include "cpu.hpp"
#include <iostream>
#include <cassert>

// --- Bit Manipulation Helpers ---
#define OP_RS(instr)        ((instr >> 21) & 0x1F)
#define OP_RT(instr)        ((instr >> 16) & 0x1F)
#define OP_RD(instr)        ((instr >> 11) & 0x1F)
#define OP_SHAMT(instr)     ((instr >> 6) & 0x1F)
#define OP_FUNCT(instr)     (instr & 0x3F)
#define OP_IMM16(instr)     (instr & 0xFFFF)
#define OP_TARGET(instr)    (instr & 0x3FFFFFF)

// Sign Extension Helper
inline uint32_t sign_extend(uint16_t val) {
    return static_cast<int32_t>(static_cast<int16_t>(val));
}

namespace Instructions {

    // --- Helper for Register Access ---
    // Handles the fact that register 0 is const/read-only.
    inline void set_reg(CPU& cpu, uint32_t index, uint32_t value) {
        if (index != 0) {
            // We cast away the const-ness of the 'zero' member's address.
            // This allows us to write to the other registers (1-31) which are mutable,
            // relying on the struct layout being contiguous.
            uint32_t* regs = const_cast<uint32_t*>(&cpu.registers.zero);
            regs[index] = value;
        }
    }

    // Read helper (just for symmetry, though direct access works fine)
    inline uint32_t get_reg(CPU& cpu, uint32_t index) {
        return (&cpu.registers.zero)[index];
    }

    // --- Core Logic ---

    static void illegal(CPU& cpu) {
        std::cerr << "Illegal Instruction: 0x" << std::hex << cpu.instr << " at PC: 0x" << cpu.registers.pc - 4 << std::endl;

        uint32_t pri = cpu.instr >> 26;
        uint32_t sec = cpu.instr & 0x3F;

        std::cerr << "Primary opcode: 0x" << std::hex << pri << ", Secondary opcode: 0x" << std::hex << sec << std::endl;

        if (cpu.bus) {
            cpu.bus->dumpMemoryRegion(cpu.registers.pc - 4, 0xff);
        }
        // TODO: Raise Exception Code 0x0A (Reserved Instruction)
        assert(false && "Illegal Instruction Encountered");
    }

    static void nop(CPU& cpu) {
        (void)cpu; 
    }

    // --- Primary Opcode Logic (0x00 - 0x3F) ---

    // 0x00: SPECIAL (Dispatches to Secondary Table)
    static void special(CPU& cpu) {
        uint32_t funct = OP_FUNCT(cpu.instr);
        cpu.sec_table[funct](cpu);
    }

    // 0x01: BcondZ (REGIMM)
    static void bcondz(CPU& cpu) {
        uint32_t rt = OP_RT(cpu.instr);
        uint32_t rs_val = get_reg(cpu, OP_RS(cpu.instr));
        bool link = (rt & 0x1E) == 0x10; // BLTZAL, BGEZAL have bits 1000x
        bool ge = (rt & 0x01);           // BGEZ, BGEZAL have bit 00001

        bool condition = ge ? ((int32_t)rs_val >= 0) : ((int32_t)rs_val < 0);

        if (link) {
            cpu.registers.ra = cpu.registers.pc + 4;
        }

        if (condition) {
            uint32_t offset = sign_extend(OP_IMM16(cpu.instr)) << 2;
            cpu.next_pc = cpu.registers.pc + offset;
        }
    }

    // 0x02: J (Jump)
    static void j(CPU& cpu) {
        uint32_t target = OP_TARGET(cpu.instr) << 2;
        cpu.next_pc = (cpu.registers.pc & 0xF0000000) | target;
    }

    // 0x03: JAL (Jump And Link)
    static void jal(CPU& cpu) {
        cpu.registers.ra = cpu.registers.pc + 4;
        uint32_t target = OP_TARGET(cpu.instr) << 2;
        cpu.next_pc = (cpu.registers.pc & 0xF0000000) | target;
    }

    // 0x08: ADDI
    static void addi(CPU& cpu) {
        uint32_t s = get_reg(cpu, OP_RS(cpu.instr));
        uint32_t i = sign_extend(OP_IMM16(cpu.instr));
        uint32_t res = s + i;
        // TODO: Overflow trap
        set_reg(cpu, OP_RT(cpu.instr), res);
    }

    // 0x09: ADDIU
    static void addiu(CPU& cpu) {
        uint32_t s = get_reg(cpu, OP_RS(cpu.instr));
        uint32_t i = sign_extend(OP_IMM16(cpu.instr));
        set_reg(cpu, OP_RT(cpu.instr), s + i);
    }

    // 0x0C: ANDI
    static void andi(CPU& cpu) {
        uint32_t s = get_reg(cpu, OP_RS(cpu.instr));
        uint32_t i = OP_IMM16(cpu.instr); // Zero extended
        set_reg(cpu, OP_RT(cpu.instr), s & i);
    }

    // 0x0D: ORI
    static void ori(CPU& cpu) {
        uint32_t s = get_reg(cpu, OP_RS(cpu.instr));
        uint32_t i = OP_IMM16(cpu.instr); // Zero extended
        set_reg(cpu, OP_RT(cpu.instr), s | i);
    }

    // 0x0F: LUI
    static void lui(CPU& cpu) {
        uint32_t i = OP_IMM16(cpu.instr);
        set_reg(cpu, OP_RT(cpu.instr), i << 16);
    }

    // 0x23: LW
    static void lw(CPU& cpu) {
        uint32_t base = get_reg(cpu, OP_RS(cpu.instr));
        uint32_t offset = sign_extend(OP_IMM16(cpu.instr));
        uint32_t addr = base + offset;
        uint32_t val = cpu.read32(addr);
        set_reg(cpu, OP_RT(cpu.instr), val);
    }

    // 0x2B: SW
    static void sw(CPU& cpu) {
        uint32_t base = get_reg(cpu, OP_RS(cpu.instr));
        uint32_t offset = sign_extend(OP_IMM16(cpu.instr));
        uint32_t addr = base + offset;
        uint32_t val = get_reg(cpu, OP_RT(cpu.instr));
        cpu.write32(addr, val);
    }

    // --- Secondary Function Logic ---

    // 0x00: SLL
    static void sll(CPU& cpu) {
        uint32_t rt = get_reg(cpu, OP_RT(cpu.instr));
        uint32_t sa = OP_SHAMT(cpu.instr);
        set_reg(cpu, OP_RD(cpu.instr), rt << sa);
    }

    // 0x20: ADD
    static void add(CPU& cpu) {
        uint32_t s = get_reg(cpu, OP_RS(cpu.instr));
        uint32_t t = get_reg(cpu, OP_RT(cpu.instr));
        uint32_t res = s + t;
        // TODO: Overflow trap
        set_reg(cpu, OP_RD(cpu.instr), res);
    }

    // 0x21: ADDU
    static void addu(CPU& cpu) {
        uint32_t s = get_reg(cpu, OP_RS(cpu.instr));
        uint32_t t = get_reg(cpu, OP_RT(cpu.instr));
        set_reg(cpu, OP_RD(cpu.instr), s + t);
    }
    
    // 0x24: AND
    static void _and(CPU& cpu) {
        uint32_t s = get_reg(cpu, OP_RS(cpu.instr));
        uint32_t t = get_reg(cpu, OP_RT(cpu.instr));
        set_reg(cpu, OP_RD(cpu.instr), s & t);
    }

    // 0x25: OR
    static void _or(CPU& cpu) {
        uint32_t s = get_reg(cpu, OP_RS(cpu.instr));
        uint32_t t = get_reg(cpu, OP_RT(cpu.instr));
        set_reg(cpu, OP_RD(cpu.instr), s | t);
    }

    // 0x08: JR
    static void jr(CPU& cpu) {
        uint32_t s = get_reg(cpu, OP_RS(cpu.instr));
        cpu.next_pc = s;
    }
}