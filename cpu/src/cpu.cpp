/*
    Description: CPU Implementation File
    Author: LN697
    Date: 13 December 2025
*/

#include "cpu.hpp"
#include <iostream>
#include <iomanip>

CPU::CPU(Bus* bus) : bus(bus) {
    cycles = 0;
}

CPU::~CPU() = default;

void CPU::init() {
    // for (int i = 0; i < 33; ++i) {
    //     (&registers.at)[i] = 0;
    // }
    // registers.pc = 0xbfc00000;
    next_pc = registers.pc + 4;
}

void CPU::step() {
    fetch();
    decode();

    // Handle Load Delay Slot countdown: decrement early so that a load's value becomes visible "two" cycles after the load instruction (i.e. after one intervening instruction).
    if (in_LDS) {
        lds_countdown--;
        if (lds_countdown <= 0) {
            if (delayed_load_tar_reg != 0) {
                uint32_t* regs = const_cast<uint32_t*>(&registers.zero);
                regs[delayed_load_tar_reg] = delayed_load_val;
            }
            in_LDS = false;
            delayed_load_tar_reg = 0;
            delayed_load_val = 0;
            lds_countdown = 0;
        }
    }

    execute();
}

void CPU::fetch() {
    instr = bus->read32(registers.pc);
    registers.pc = next_pc;
    next_pc += 4;
}

void CPU::decode() {
    pri_opcode = instr >> 26;
    sec_opcode = instr & 0x3f;
}

void CPU::execute() {
    pri_table[pri_opcode](*this);
}

uint8_t CPU::read(uint32_t address) {
    return bus->read(address);
}

void CPU::write(uint32_t address, uint8_t data) {
    bus->write(address, data);
}

uint16_t CPU::read16(uint32_t address) {
    return bus->read16(address);
}

void CPU::write16(uint32_t address, uint16_t data) {
    bus->write16(address, data);
}

uint32_t CPU::read32(uint32_t address) {
    return bus->read32(address);
}

void CPU::write32(uint32_t address, uint32_t data) {
    bus->write32(address, data);
}

void CPU::scheduleLoad(uint32_t reg, uint32_t value) {
    in_LDS = true;
    delayed_load_tar_reg = reg;
    delayed_load_val = value;
    lds_countdown = 2;
}



#ifdef DEBUG
void CPU::dumpRegisters() {
    std::cout << "[Register] zero value:\t 0x" << std::hex << registers.zero << std::endl;
    std::cout << "[Register] at value:\t 0x" << std::hex << registers.at << std::endl;
    std::cout << "[Register] v0 value:\t 0x" << std::hex << registers.v0 << std::endl;
    std::cout << "[Register] v1 value:\t 0x" << std::hex << registers.v1 << std::endl;
    std::cout << "[Register] a0 value:\t 0x" << std::hex << registers.a0 << std::endl;
    std::cout << "[Register] a1 value:\t 0x" << std::hex << registers.a1 << std::endl;
    std::cout << "[Register] a2 value:\t 0x" << std::hex << registers.a2 << std::endl;
    std::cout << "[Register] a3 value:\t 0x" << std::hex << registers.a3 << std::endl;
    std::cout << "[Register] t0 value:\t 0x" << std::hex << registers.t0 << std::endl;
    std::cout << "[Register] t1 value:\t 0x" << std::hex << registers.t1 << std::endl;
    std::cout << "[Register] t2 value:\t 0x" << std::hex << registers.t2 << std::endl;
    std::cout << "[Register] t3 value:\t 0x" << std::hex << registers.t3 << std::endl;
    std::cout << "[Register] t4 value:\t 0x" << std::hex << registers.t4 << std::endl;
    std::cout << "[Register] t5 value:\t 0x" << std::hex << registers.t5 << std::endl;
    std::cout << "[Register] t6 value:\t 0x" << std::hex << registers.t6 << std::endl;
    std::cout << "[Register] t7 value:\t 0x" << std::hex << registers.t7 << std::endl;
    std::cout << "[Register] s0 value:\t 0x" << std::hex << registers.s0 << std::endl;
    std::cout << "[Register] s1 value:\t 0x" << std::hex << registers.s1 << std::endl;
    std::cout << "[Register] s2 value:\t 0x" << std::hex << registers.s2 << std::endl;
    std::cout << "[Register] s3 value:\t 0x" << std::hex << registers.s3 << std::endl;
    std::cout << "[Register] s4 value:\t 0x" << std::hex << registers.s4 << std::endl;
    std::cout << "[Register] s5 value:\t 0x" << std::hex << registers.s5 << std::endl;
    std::cout << "[Register] s6 value:\t 0x" << std::hex << registers.s6 << std::endl;
    std::cout << "[Register] s7 value:\t 0x" << std::hex << registers.s7 << std::endl;
    std::cout << "[Register] t8 value:\t 0x" << std::hex << registers.t8 << std::endl;
    std::cout << "[Register] t9 value:\t 0x" << std::hex << registers.t9 << std::endl;
    std::cout << "[Register] k0 value:\t 0x" << std::hex << registers.k0 << std::endl;
    std::cout << "[Register] k1 value:\t 0x" << std::hex << registers.k1 << std::endl;
    std::cout << "[Register] gp value:\t 0x" << std::hex << registers.gp << std::endl;
    std::cout << "[Register] sp value:\t 0x" << std::hex << registers.sp << std::endl;
    std::cout << "[Register] fp_s8 value:\t 0x" << std::hex << registers.fp_s8 << std::endl;
    std::cout << "[Register] ra value:\t 0x" << std::hex << registers.ra << std::endl;
    std::cout << "[Register] pc value:\t 0x" << std::hex << registers.pc << std::endl;
    std::cout << "[Register] hi value:\t 0x" << std::hex << registers.hi << std::endl;
    std::cout << "[Register] lo value:\t 0x" << std::hex << registers.lo << std::endl;
}
#endif
