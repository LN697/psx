/*
    Description: Opcodes Implementation File
    Author: LN697
    Date: 13 December 2025
*/

#pragma once

#include "cpu.hpp"

using InstructionHandler = void (*)(CPU&, uint32_t);
InstructionHandler opcodes[64] = {};
