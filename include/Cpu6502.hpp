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
    inline uint8_t IMP(); inline uint8_t IMM();
    inline uint8_t ZP0(); inline uint8_t ZPX();
    inline uint8_t ZPY(); inline uint8_t REL();
    inline uint8_t ABS(); inline uint8_t ABX();
    inline uint8_t ABY(); inline uint8_t IND();
    inline uint8_t IZX(); inline uint8_t IZY();

    // Instructions
    inline uint8_t ADC(); inline uint8_t AND(); inline uint8_t ASL(); inline uint8_t BCC();
    inline uint8_t BCS(); inline uint8_t BEQ(); inline uint8_t BIT(); inline uint8_t BMI();
    inline uint8_t BNE(); inline uint8_t BPL(); inline uint8_t BRK(); inline uint8_t BVC();
    inline uint8_t BVS(); inline uint8_t CLC(); inline uint8_t CLD(); inline uint8_t CLI();
    inline uint8_t CLV(); inline uint8_t CMP(); inline uint8_t CPX(); inline uint8_t CPY();
    inline uint8_t DEC(); inline uint8_t DEX(); inline uint8_t DEY(); inline uint8_t EOR();
    inline uint8_t INC(); inline uint8_t INX(); inline uint8_t INY(); inline uint8_t JMP();
    inline uint8_t JSR(); inline uint8_t LDA(); inline uint8_t LDX(); inline uint8_t LDY();
    inline uint8_t LSR(); inline uint8_t NOP(); inline uint8_t ORA(); inline uint8_t PHA();
    inline uint8_t PHP(); inline uint8_t PLA(); inline uint8_t PLP(); inline uint8_t ROL(); 
    inline uint8_t ROR(); inline uint8_t RTI(); inline uint8_t RTS(); inline uint8_t SBC();
    inline uint8_t SEC(); inline uint8_t SED(); inline uint8_t SEI(); inline uint8_t STA(); 
    inline uint8_t STX(); inline uint8_t STY(); inline uint8_t TAX(); inline uint8_t TAY(); 
    inline uint8_t TSX(); inline uint8_t TXA(); inline uint8_t TXS(); inline uint8_t TYA();

    inline uint8_t AASL(); inline uint8_t ALSR(); inline uint8_t AROL(); inline uint8_t AROR();

    // Illegal Instructions
    inline uint8_t ALR(); inline uint8_t ANC(); inline uint8_t ARR(); inline uint8_t AXS();
    inline uint8_t LAX(); inline uint8_t SAX(); inline uint8_t DCP(); inline uint8_t ISC();
    inline uint8_t RLA(); inline uint8_t RRA(); inline uint8_t SLO(); inline uint8_t SRE();
    inline uint8_t SKB(); inline uint8_t IGN(); inline uint8_t STP(); inline uint8_t XAA();
    inline uint8_t SHX(); inline uint8_t SHY();
    inline uint8_t XXX();

    void clock();

    void reset();
    void irq();
    void nmi();

    inline bool completed() { return cycles == 0; }

    inline uint8_t fetch();
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
