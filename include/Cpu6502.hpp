/**
 * @file Cpu6502.hpp
 * @author frayien (frayien@yahoo.fr)
 * @brief CPU 6502 emulation for the FNES Emulator
 * @version 0.1
 * @date 2020-10-31
 *
 * @copyright Copyright (c) 2020
 *
 * thx Klaus2m5 for the tests : https://github.com/Klaus2m5/6502_65C02_functional_tests
 *
 */

#pragma once

#include <cstdint>

#include "IBus.hpp"

class Cpu6502
{
public:
    Cpu6502();
    virtual ~Cpu6502();

    inline void connectBus(IBus *_bus) { bus = _bus; }

    // Registers
    union
    {
        uint8_t reg_status = 0x00;
        struct
        {
            uint8_t C : 1; // Carry
            uint8_t Z : 1; // Zero
            uint8_t I : 1; // Disable Interupts
            uint8_t D : 1; // Decimal Mode
            uint8_t B : 1; // Break
            uint8_t U : 1; // Unused
            uint8_t V : 1; // Overflow
            uint8_t N : 1; // Negative
        } flag;
    };
    uint8_t reg_A = 0x00; // Accumulator Registrer
    uint8_t reg_X = 0x00; // X Register
    uint8_t reg_Y = 0x00; // Y Register
    uint8_t reg_SP = 0x00; // Stack Pointer
    union
    {
        uint16_t reg_PC = 0x0000; // Program Counter
        struct
        {
            uint8_t low : 8;
            uint8_t high : 8;
        } reg_PC_byte;
    };

    // Adressing Modes
    uint8_t IMP(); uint8_t IMM();
    uint8_t ZP0(); uint8_t ZPX();
    uint8_t ZPY(); uint8_t REL();
    uint8_t ABS(); uint8_t ABX();
    uint8_t ABY(); uint8_t IND();
    uint8_t IZX(); uint8_t IZY();

    // Instructions
    uint8_t ADC(); uint8_t AND(); uint8_t ASL(); uint8_t BCC();
    uint8_t BCS(); uint8_t BEQ(); uint8_t BIT(); uint8_t BMI();
    uint8_t BNE(); uint8_t BPL(); uint8_t BRK(); uint8_t BVC();
    uint8_t BVS(); uint8_t CLC(); uint8_t CLD(); uint8_t CLI();
    uint8_t CLV(); uint8_t CMP(); uint8_t CPX(); uint8_t CPY();
    uint8_t DEC(); uint8_t DEX(); uint8_t DEY(); uint8_t EOR();
    uint8_t INC(); uint8_t INX(); uint8_t INY(); uint8_t JMP();
    uint8_t JSR(); uint8_t LDA(); uint8_t LDX(); uint8_t LDY();
    uint8_t LSR(); uint8_t NOP(); uint8_t ORA(); uint8_t PHA();
    uint8_t PHP(); uint8_t PLA(); uint8_t PLP(); uint8_t ROL();
    uint8_t ROR(); uint8_t RTI(); uint8_t RTS(); uint8_t SBC();
    uint8_t SEC(); uint8_t SED(); uint8_t SEI(); uint8_t STA();
    uint8_t STX(); uint8_t STY(); uint8_t TAX(); uint8_t TAY();
    uint8_t TSX(); uint8_t TXA(); uint8_t TXS(); uint8_t TYA();

    uint8_t AASL(); uint8_t ALSR(); uint8_t AROL(); uint8_t AROR();

    // Illegal Instructions
    uint8_t ALR(); uint8_t ANC(); uint8_t ARR(); uint8_t AXS();
    uint8_t LAX(); uint8_t SAX(); uint8_t DCP(); uint8_t ISC();
    uint8_t RLA(); uint8_t RRA(); uint8_t SLO(); uint8_t SRE();
    uint8_t SKB(); uint8_t IGN(); uint8_t STP(); uint8_t XAA();
    uint8_t SHX(); uint8_t SHY(); uint8_t AHX(); uint8_t TAS();
    uint8_t LAS();

    void clock();

    void reset();
    void irq();
    void nmi();

    inline bool completed() { return cycles == 0; }

    uint8_t fetch();
    uint8_t fetched = 0x00;

    union
    {
        uint16_t addr_abs = 0x0000;
        struct
        {
            uint8_t low : 8;
            uint8_t high : 8;
        } addr_abs_byte;
    };

    uint8_t addr_page_boundary_crossed = false;
    uint8_t addr_page_boundary_extra_cycle = false;

    uint16_t addr_rel = 0x0000;
    uint8_t opcode = 0x00;
    uint8_t cycles = 0;

private:
    IBus *bus = nullptr;

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
};
