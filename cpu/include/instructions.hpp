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

    uint32_t imm, rs, rt;

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

    // Read helper (just for symmetry, though direct access works fine). Inline shoud help with the performance.
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
        bool link = (rt & 0x1E) == 0x10;    // BLTZAL, BGEZAL have bits 1000x
        bool ge = (rt & 0x01);              // BGEZ, BGEZAL have bit 00001

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

    // 0x08: ADDI rt, rs, imm (INCOMPLETE)
    static void addi(CPU& cpu) {
        rs = get_reg(cpu, OP_RS(cpu.instr));
        imm = sign_extend(OP_IMM16(cpu.instr));
        uint32_t res = rs + imm;
        // TODO: Signed overflow trap
        set_reg(cpu, OP_RT(cpu.instr), res);
    }

    // 0x09: ADDIU rt, rs, imm
    static void addiu(CPU& cpu) {
        rs = get_reg(cpu, OP_RS(cpu.instr));
        imm = sign_extend(OP_IMM16(cpu.instr));
        set_reg(cpu, OP_RT(cpu.instr), rs + imm);
    }

    // 0x0A: SLTI rt, rs, imm
    static void slti(CPU& cpu) {
        rs = get_reg(cpu, OP_RS(cpu.instr));
        imm = sign_extend(OP_IMM16(cpu.instr));
        set_reg(cpu, OP_RT(cpu.instr), static_cast<int32_t>(rs) < static_cast<int32_t>(imm) ? 1 : 0);
    }

    // 0x0B: SLTIU rt, rs, imm
    static void sltiu(CPU& cpu) {
        rs = get_reg(cpu, OP_RS(cpu.instr));
        imm = sign_extend(OP_IMM16(cpu.instr));
        set_reg(cpu, OP_RT(cpu.instr), (rs < imm) ? 1 : 0);
    }

    // 0x0C: ANDI rt, rs, imm
    static void andi(CPU& cpu) {
        rs = get_reg(cpu, OP_RS(cpu.instr));
        imm = OP_IMM16(cpu.instr); // Zero extended
        set_reg(cpu, OP_RT(cpu.instr), rs & imm);
    }

    // 0x0D: ORI rt, rs, imm
    static void ori(CPU& cpu) {
        rs = get_reg(cpu, OP_RS(cpu.instr));
        imm = OP_IMM16(cpu.instr); // Zero extended
        set_reg(cpu, OP_RT(cpu.instr), rs | imm);
    }

    // 0x0E: XORI rt, rs, imm
    static void xori(CPU& cpu) {
        rs = get_reg(cpu, OP_RS(cpu.instr));
        imm = OP_IMM16(cpu.instr); // Zero extended
        set_reg(cpu, OP_RT(cpu.instr), rs ^ imm);
    }

    // 0x0F: LUI rt, imm (Load Upper Immediate)
    static void lui(CPU& cpu) { // Do I need to implement the Load Delay Slot (LDS) here too?
        imm = OP_IMM16(cpu.instr);
        set_reg(cpu, OP_RT(cpu.instr), imm << 16);
    }

    // 0x20: LB rt, imm(rs)
    static void lb(CPU& cpu) {
        uint32_t addr = get_reg(cpu, OP_RS(cpu.instr)) + sign_extend(OP_IMM16(cpu.instr));
        int8_t value = static_cast<int8_t>(cpu.read(addr));
        cpu.scheduleLoad(OP_RT(cpu.instr), static_cast<uint32_t>(static_cast<int32_t>(value)));
    }

    // 0x24: LBU rt, imm(rs)
    static void lbu(CPU& cpu) {
        uint32_t addr = get_reg(cpu, OP_RS(cpu.instr)) + OP_IMM16(cpu.instr);
        uint8_t value = cpu.read(addr);
        cpu.scheduleLoad(OP_RT(cpu.instr), static_cast<uint32_t>(value));
    }

    // 0x21: LH rt, imm(rs)
    static void lh(CPU& cpu) {
        uint32_t addr = get_reg(cpu, OP_RS(cpu.instr)) + sign_extend(OP_IMM16(cpu.instr));
        int16_t value = static_cast<int16_t>(cpu.read16(addr));
        cpu.scheduleLoad(OP_RT(cpu.instr), static_cast<uint32_t>(static_cast<int32_t>(value)));
    }

    // 0x25: LHU rt, imm(rs)
    static void lhu(CPU& cpu) {
        uint32_t addr = get_reg(cpu, OP_RS(cpu.instr)) + OP_IMM16(cpu.instr);
        uint32_t value = cpu.read16(addr) & 0xFFFF;
        cpu.scheduleLoad(OP_RT(cpu.instr), value);
    }

    // 0x23: LW rt, imm(rs)
    static void lw(CPU& cpu) {
        uint32_t addr = get_reg(cpu, OP_RS(cpu.instr)) + sign_extend(OP_IMM16(cpu.instr));
        uint32_t value = cpu.read32(addr);
        cpu.scheduleLoad(OP_RT(cpu.instr), value);
    }

    // 0x28: SB rt, imm(rs)
    static void sb(CPU& cpu) {
        cpu.write(get_reg(cpu, OP_RS(cpu.instr)) + sign_extend(OP_IMM16(cpu.instr)), get_reg(cpu, OP_RT(cpu.instr)));
    }

    // 0x29: SH rt, imm(rs)
    static void sh(CPU& cpu) {
        cpu.write16(get_reg(cpu, OP_RS(cpu.instr)) + sign_extend(OP_IMM16(cpu.instr)), get_reg(cpu, OP_RT(cpu.instr)));
    }

    // 0x2B: SW rt, imm(rs)
    static void sw(CPU& cpu) {
        cpu.write32(get_reg(cpu, OP_RS(cpu.instr)) + sign_extend(OP_IMM16(cpu.instr)), get_reg(cpu, OP_RT(cpu.instr)));
    }

    // Corrected LWL (Little Endian)
    static void lwl(CPU& cpu) {
        uint32_t addr = get_reg(cpu, OP_RS(cpu.instr)) + sign_extend(OP_IMM16(cpu.instr));
        uint32_t shift = (addr & 3) * 8;
        uint32_t mem_word = cpu.read32(addr & ~3);
        uint32_t rt_val = get_reg(cpu, OP_RT(cpu.instr));
    
        // LWL at offset 0: (mem_word << 24). Mask reg bits 0..23.
        // LWL at offset 3: (mem_word << 0).  Mask reg bits (none).
        uint32_t value_to_load = mem_word << (24 - shift);
        uint32_t mask = 0x00FFFFFF >> shift; 
    
        cpu.scheduleLoad(OP_RT(cpu.instr), (rt_val & mask) | value_to_load);
    }
    
    // Corrected LWR (Little Endian)
    static void lwr(CPU& cpu) {
        uint32_t addr = get_reg(cpu, OP_RS(cpu.instr)) + sign_extend(OP_IMM16(cpu.instr));
        uint32_t shift = (addr & 3) * 8;
        uint32_t mem_word = cpu.read32(addr & ~3);
        uint32_t rt_val = get_reg(cpu, OP_RT(cpu.instr));
    
        // LWR at offset 0: (mem_word >> 0).  Mask reg bits (none).
        // LWR at offset 3: (mem_word >> 24). Mask reg bits 8..31.
        uint32_t value_to_load = mem_word >> shift;
        uint32_t mask = 0xFFFFFF00 << (24 - shift);
    
        cpu.scheduleLoad(OP_RT(cpu.instr), (rt_val & mask) | value_to_load);
    }

    // 0x2A: SWL rt, imm(rs)
    static void swl(CPU& cpu) {
        uint32_t addr = get_reg(cpu, OP_RS(cpu.instr)) + sign_extend(OP_IMM16(cpu.instr));
        uint32_t source_reg_val = get_reg(cpu, OP_RT(cpu.instr));

        uint32_t aligned_addr = addr & ~0x3;
        uint32_t offset = addr & 0x3; // 0, 1, 2, or 3

        uint32_t current_mem_word = cpu.read32(aligned_addr);
        uint32_t data_to_store_masked = 0;
        uint32_t mask_for_mem_write = 0;

        switch (offset) {
            case 0: // Aligned access, write all 4 bytes
                data_to_store_masked = source_reg_val;
                mask_for_mem_write = 0xFFFFFFFF;
                break;
            case 1: // Write bytes from reg[8-31] to mem[1-3]. Preserve mem[0].
                data_to_store_masked = (source_reg_val >> 8) << 8; // Take bytes 1,2,3 from source_reg_val, shift to mem positions 1,2,3
                mask_for_mem_write = 0xFFFFFF00; // Mask for bytes 1,2,3 in memory
                break;
            case 2: // Write bytes from reg[16-31] to mem[2-3]. Preserve mem[0-1].
                data_to_store_masked = (source_reg_val >> 16) << 16; // Take bytes 2,3 from source_reg_val, shift to mem positions 2,3
                mask_for_mem_write = 0xFFFF0000; // Mask for bytes 2,3 in memory
                break;
            case 3: // Write byte from reg[24-31] to mem[3]. Preserve mem[0-2].
                data_to_store_masked = (source_reg_val >> 24) << 24; // Take byte 3 from source_reg_val, shift to mem position 3
                mask_for_mem_write = 0xFF000000; // Mask for byte 3 in memory
                break;
        }

        uint32_t final_mem_value = (current_mem_word & ~mask_for_mem_write) | (data_to_store_masked & mask_for_mem_write);
        cpu.write32(aligned_addr, final_mem_value);
    }

    // 0x2E: SWR rt, imm(rs)
    static void swr(CPU& cpu) {
        uint32_t addr = get_reg(cpu, OP_RS(cpu.instr)) + sign_extend(OP_IMM16(cpu.instr));
        uint32_t source_reg_val = get_reg(cpu, OP_RT(cpu.instr));

        uint32_t aligned_addr = addr & ~0x3;
        uint32_t offset = addr & 0x3; // 0, 1, 2, or 3

        uint32_t current_mem_word = cpu.read32(aligned_addr);
        uint32_t data_to_store_masked = 0;
        uint32_t mask_for_mem_write = 0;

        // For SWR on Little Endian:
        // addr points to the least significant byte written.
        // Bytes are written from aligned_addr up to addr from the register's least significant part.
        // Bytes in memory after addr are preserved.
        switch (offset) {
            case 0: // addr = aligned_addr + 0. Store reg[0-7] into mem[0]. Preserve mem[1-3].
                data_to_store_masked = source_reg_val & 0x000000FF; // Take byte 0 from source_reg_val
                mask_for_mem_write = 0x000000FF; // Mask for byte 0 in memory
                break;
            case 1: // addr = aligned_addr + 1. Store reg[0-15] into mem[0-1]. Preserve mem[2-3].
                data_to_store_masked = source_reg_val & 0x0000FFFF; // Take bytes 0,1 from source_reg_val
                mask_for_mem_write = 0x0000FFFF; // Mask for bytes 0,1 in memory
                break;
            case 2: // addr = aligned_addr + 2. Store reg[0-23] into mem[0-2]. Preserve mem[3].
                data_to_store_masked = source_reg_val & 0x00FFFFFF; // Take bytes 0,1,2 from source_reg_val
                mask_for_mem_write = 0x00FFFFFF; // Mask for bytes 0,1,2 in memory
                break;
            case 3: // addr = aligned_addr + 3. Store reg[0-31] into mem[0-3]. Full word write.
                data_to_store_masked = source_reg_val;
                mask_for_mem_write = 0xFFFFFFFF;
                break;
        }

        uint32_t final_mem_value = (current_mem_word & ~mask_for_mem_write) | (data_to_store_masked & mask_for_mem_write);
        cpu.write32(aligned_addr, final_mem_value);
    }

    // 0x10: COP0
    static void cop0(CPU& cpu) {
        uint32_t rs = OP_RS(cpu.instr); // Operation type
        uint32_t rd = OP_RD(cpu.instr); // Destination COP0 register
        uint32_t rt = OP_RT(cpu.instr); // Source/Dest CPU register

        switch (rs) {
            case 0x00: // MFC0 (Move From Cop0): rt = cop0[rd]
                // We implement the critical registers for BIOS booting
                switch (rd) {
                    case 12: set_reg(cpu, rt, cpu.registers.sr); break;
                    case 13: set_reg(cpu, rt, cpu.registers.cause); break;
                    case 14: set_reg(cpu, rt, cpu.registers.epc); break;
                    default: 
                        // Fallback for unhandled registers
                        set_reg(cpu, rt, 0); 
                        break;
                }
                break;

            case 0x04: // MTC0 (Move To Cop0): cop0[rd] = rt
                {
                    uint32_t val = get_reg(cpu, rt);
                    switch (rd) {
                        case 12: cpu.registers.sr = val; break;
                        case 13: cpu.registers.cause = val; break;
                        // EPC is usually read-only for software, but writable by hardware
                        case 14: cpu.registers.epc = val; break; 
                        default: break;
                    }
                }
                break;

            default:
                std::cerr << "[CPU] Unhandled COP0 instruction: 0x" << std::hex << cpu.instr << std::endl;
                break;
        }
    }

    // 0x05: BNE
    static void bne(CPU& cpu) {
        uint32_t s = get_reg(cpu, OP_RS(cpu.instr));
        uint32_t t = get_reg(cpu, OP_RT(cpu.instr));
        uint32_t offset = sign_extend(OP_IMM16(cpu.instr)) << 2;

        if (s != t) {
            cpu.next_pc = cpu.registers.pc + 4 + offset;
        }
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