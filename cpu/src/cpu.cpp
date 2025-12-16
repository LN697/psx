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
    registers.pc = 0xbfc00000;

    registers.v0 = 0x0;
    registers.v1 = 0x0;
    registers.a0 = 0x0;
    registers.a1 = 0x0;
    registers.a2 = 0x0;
    registers.a3 = 0x0;
    registers.t0 = 0x0;
    registers.t1 = 0x0;
    registers.t2 = 0x0;
    registers.t3 = 0x0;
    registers.t4 = 0x0;
    registers.t5 = 0x0;
    registers.t6 = 0x0;
    registers.t7 = 0x0;
    registers.s0 = 0x0;
    registers.s1 = 0x0;
    registers.s2 = 0x0;
    registers.s3 = 0x0;
    registers.s4 = 0x0;
    registers.s5 = 0x0;
    registers.s6 = 0x0;
    registers.s7 = 0x0;
    registers.t8 = 0x0;
    registers.t9 = 0x0;
    registers.k0 = 0x0;
    registers.k1 = 0x0;
    registers.gp = 0x0;
    registers.sp = 0x0;
    registers.fp_s8 = 0x0;
    registers.ra = 0x0;
    registers.hi = 0x0;
    registers.lo = 0x0;
}

void CPU::step() {
    fetch();
    decode();
    execute();
}

void CPU::fetch() {
    instr = bus->read32(registers.pc);
    registers.pc += 4;
}

void CPU::decode() {
    pri_opcode = instr >> 26;
    sec_opcode = instr & 0x3f;

    std::cout << "[CPU::Decode]   Primary Opcode: 0x" << std::hex << std::setfill('0') << std::setw(2) << (int)pri_opcode << std::endl;
    std::cout << "[CPU::Decode] Secondary Opcode: 0x" << std::hex << std::setfill('0') << std::setw(2) << (int)sec_opcode << std::endl;
}

void CPU::execute() {

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
