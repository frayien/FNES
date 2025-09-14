#include "Cpu6502.hpp"

#include <cstdint>
#include <stdexcept>

Cpu6502::Cpu6502()
{

}

Cpu6502::~Cpu6502()
{

}

inline uint8_t Cpu6502::read(uint16_t addr)
{
    return bus->cpuRead(addr);
}

void Cpu6502::write(uint16_t addr, uint8_t data)
{
    bus->cpuWrite(addr, data);
}

void Cpu6502::clock()
{
    if(cycles == 0)
    {
        opcode = read(reg_PC);
        reg_PC++;

        // unused flag is always set
        flag.U = 1;

    #define OP(op, ins, adr, cyc) \
            case 0x##op: cycles = cyc; \
            addr_page_boundary_crossed     = adr(); \
            addr_page_boundary_extra_cycle = ins(); \
            cycles += (addr_page_boundary_crossed & addr_page_boundary_extra_cycle); \
            break;

        switch(opcode)
        {
            OP(00, BRK, IMP, 7) OP(01, ORA, IZX, 6) OP(02, STP, IMP, 2) OP(03, SLO, IZX, 8) OP(04, IGN, ZP0, 3) OP(05, ORA, ZP0, 3) OP(06, ASL, ZP0, 5) OP(07, SLO, ZP0, 5) OP(08, PHP, IMP, 3) OP(09, ORA, IMM, 2) OP(0A, AASL, IMP, 2) OP(0B, ANC, IMM, 2) OP(0C, IGN, ABS, 4) OP(0D, ORA, ABS, 4) OP(0E, ASL, ABS, 6) OP(0F, SLO, ABS, 6)
            OP(10, BPL, REL, 2) OP(11, ORA, IZY, 5) OP(12, STP, IMP, 2) OP(13, SLO, IZY, 8) OP(14, IGN, ZPX, 4) OP(15, ORA, ZPX, 4) OP(16, ASL, ZPX, 6) OP(17, SLO, ZPX, 6) OP(18, CLC, IMP, 2) OP(19, ORA, ABY, 4) OP(1A,  NOP, IMP, 2) OP(1B, SLO, ABY, 7) OP(1C, IGN, ABX, 4) OP(1D, ORA, ABX, 4) OP(1E, ASL, ABX, 7) OP(1F, SLO, ABX, 7)
            OP(20, JSR, ABS, 6) OP(21, AND, IZX, 6) OP(22, STP, IMP, 2) OP(23, RLA, IZX, 8) OP(24, BIT, ZP0, 3) OP(25, AND, ZP0, 3) OP(26, ROL, ZP0, 5) OP(27, RLA, ZP0, 5) OP(28, PLP, IMP, 4) OP(29, AND, IMM, 2) OP(2A, AROL, IMP, 2) OP(2B, ANC, IMM, 2) OP(2C, BIT, ABS, 4) OP(2D, AND, ABS, 4) OP(2E, ROL, ABS, 6) OP(2F, RLA, ABS, 6)
            OP(30, BMI, REL, 2) OP(31, AND, IZY, 5) OP(32, STP, IMP, 2) OP(33, RLA, IZY, 8) OP(34, IGN, ZPX, 4) OP(35, AND, ZPX, 4) OP(36, ROL, ZPX, 6) OP(37, RLA, ZPX, 6) OP(38, SEC, IMP, 2) OP(39, AND, ABY, 4) OP(3A,  NOP, IMP, 2) OP(3B, RLA, ABY, 7) OP(3C, IGN, ABX, 4) OP(3D, AND, ABX, 4) OP(3E, ROL, ABX, 7) OP(3F, RLA, ABX, 7)
            OP(40, RTI, IMP, 6) OP(41, EOR, IZX, 6) OP(42, STP, IMP, 2) OP(43, SRE, IZX, 8) OP(44, IGN, ZP0, 3) OP(45, EOR, ZP0, 3) OP(46, LSR, ZP0, 5) OP(47, SRE, ZP0, 5) OP(48, PHA, IMP, 3) OP(49, EOR, IMM, 2) OP(4A, ALSR, IMP, 2) OP(4B, ALR, IMM, 2) OP(4C, JMP, ABS, 3) OP(4D, EOR, ABS, 4) OP(4E, LSR, ABS, 6) OP(4F, SRE, ABS, 6)
            OP(50, BVC, REL, 2) OP(51, EOR, IZY, 5) OP(52, STP, IMP, 2) OP(53, SRE, IZY, 8) OP(54, IGN, ZPX, 4) OP(55, EOR, ZPX, 4) OP(56, LSR, ZPX, 6) OP(57, SRE, ZPX, 6) OP(58, CLI, IMP, 2) OP(59, EOR, ABY, 4) OP(5A,  NOP, IMP, 2) OP(5B, SRE, ABY, 7) OP(5C, IGN, ABX, 4) OP(5D, EOR, ABX, 4) OP(5E, LSR, ABX, 7) OP(5F, SRE, ABX, 7)
            OP(60, RTS, IMP, 6) OP(61, ADC, IZX, 6) OP(62, STP, IMP, 2) OP(63, RRA, IZX, 8) OP(64, IGN, ZP0, 3) OP(65, ADC, ZP0, 3) OP(66, ROR, ZP0, 5) OP(67, RRA, ZP0, 5) OP(68, PLA, IMP, 4) OP(69, ADC, IMM, 2) OP(6A, AROR, IMP, 2) OP(6B, ARR, IMM, 2) OP(6C, JMP, IND, 5) OP(6D, ADC, ABS, 4) OP(6E, ROR, ABS, 6) OP(6F, RRA, ABS, 6)
            OP(70, BVS, REL, 2) OP(71, ADC, IZY, 5) OP(72, STP, IMP, 2) OP(73, RRA, IZY, 8) OP(74, IGN, ZPX, 4) OP(75, ADC, ZPX, 4) OP(76, ROR, ZPX, 6) OP(77, RRA, ZPX, 6) OP(78, SEI, IMP, 2) OP(79, ADC, ABY, 4) OP(7A,  NOP, IMP, 2) OP(7B, RRA, ABY, 7) OP(7C, IGN, ABX, 4) OP(7D, ADC, ABX, 4) OP(7E, ROR, ABX, 7) OP(7F, RRA, ABX, 7)
            OP(80, SKB, IMM, 2) OP(81, STA, IZX, 6) OP(82, SKB, IMM, 2) OP(83, SAX, IZX, 6) OP(84, STY, ZP0, 3) OP(85, STA, ZP0, 3) OP(86, STX, ZP0, 3) OP(87, SAX, ZP0, 3) OP(88, DEY, IMP, 2) OP(89, SKB, IMM, 2) OP(8A,  TXA, IMP, 2) OP(8B, XAA, IMM, 2) OP(8C, STY, ABS, 4) OP(8D, STA, ABS, 4) OP(8E, STX, ABS, 4) OP(8F, SAX, ABS, 4)
            OP(90, BCC, REL, 2) OP(91, STA, IZY, 6) OP(92, STP, IMP, 2) OP(93, AHX, IZY, 6) OP(94, STY, ZPX, 4) OP(95, STA, ZPX, 4) OP(96, STX, ZPY, 4) OP(97, SAX, ZPY, 4) OP(98, TYA, IMP, 2) OP(99, STA, ABY, 5) OP(9A,  TXS, IMP, 2) OP(9B, XXX, IMP, 5) OP(9C, SHY, ABX, 5) OP(9D, STA, ABX, 5) OP(9E, SHX, ABY, 5) OP(9F, AHX, ABY, 5)
            OP(A0, LDY, IMM, 2) OP(A1, LDA, IZX, 6) OP(A2, LDX, IMM, 2) OP(A3, LAX, IZX, 6) OP(A4, LDY, ZP0, 3) OP(A5, LDA, ZP0, 3) OP(A6, LDX, ZP0, 3) OP(A7, LAX, ZP0, 3) OP(A8, TAY, IMP, 2) OP(A9, LDA, IMM, 2) OP(AA,  TAX, IMP, 2) OP(AB, LAX, IMM, 2) OP(AC, LDY, ABS, 4) OP(AD, LDA, ABS, 4) OP(AE, LDX, ABS, 4) OP(AF, LAX, ABS, 4)
            OP(B0, BCS, REL, 2) OP(B1, LDA, IZY, 5) OP(B2, STP, IMP, 2) OP(B3, LAX, IZY, 5) OP(B4, LDY, ZPX, 4) OP(B5, LDA, ZPX, 4) OP(B6, LDX, ZPY, 4) OP(B7, LAX, ZPY, 4) OP(B8, CLV, IMP, 2) OP(B9, LDA, ABY, 4) OP(BA,  TSX, IMP, 2) OP(BB, XXX, IMP, 4) OP(BC, LDY, ABX, 4) OP(BD, LDA, ABX, 4) OP(BE, LDX, ABY, 4) OP(BF, LAX, ABY, 4)
            OP(C0, CPY, IMM, 2) OP(C1, CMP, IZX, 6) OP(C2, SKB, IMM, 2) OP(C3, DCP, IZX, 8) OP(C4, CPY, ZP0, 3) OP(C5, CMP, ZP0, 3) OP(C6, DEC, ZP0, 5) OP(C7, DCP, ZP0, 5) OP(C8, INY, IMP, 2) OP(C9, CMP, IMM, 2) OP(CA,  DEX, IMP, 2) OP(CB, AXS, IMM, 2) OP(CC, CPY, ABS, 4) OP(CD, CMP, ABS, 4) OP(CE, DEC, ABS, 6) OP(CF, DCP, ABS, 6)
            OP(D0, BNE, REL, 2) OP(D1, CMP, IZY, 5) OP(D2, STP, IMP, 2) OP(D3, DCP, IZY, 8) OP(D4, IGN, ZPX, 4) OP(D5, CMP, ZPX, 4) OP(D6, DEC, ZPX, 6) OP(D7, DCP, ZPX, 6) OP(D8, CLD, IMP, 2) OP(D9, CMP, ABY, 4) OP(DA,  NOP, IMP, 2) OP(DB, DCP, ABY, 7) OP(DC, IGN, ABX, 4) OP(DD, CMP, ABX, 4) OP(DE, DEC, ABX, 7) OP(DF, DCP, ABX, 7)
            OP(E0, CPX, IMM, 2) OP(E1, SBC, IZX, 6) OP(E2, SKB, IMM, 2) OP(E3, ISC, IZX, 8) OP(E4, CPX, ZP0, 3) OP(E5, SBC, ZP0, 3) OP(E6, INC, ZP0, 5) OP(E7, ISC, ZP0, 5) OP(E8, INX, IMP, 2) OP(E9, SBC, IMM, 2) OP(EA,  NOP, IMP, 2) OP(EB, SBC, IMM, 2) OP(EC, CPX, ABS, 4) OP(ED, SBC, ABS, 4) OP(EE, INC, ABS, 6) OP(EF, ISC, ABS, 6)
            OP(F0, BEQ, REL, 2) OP(F1, SBC, IZY, 5) OP(F2, STP, IMP, 2) OP(F3, ISC, IZY, 8) OP(F4, IGN, ZPX, 4) OP(F5, SBC, ZPX, 4) OP(F6, INC, ZPX, 6) OP(F7, ISC, ZPX, 6) OP(F8, SED, IMP, 2) OP(F9, SBC, ABY, 4) OP(FA,  NOP, IMP, 2) OP(FB, ISC, ABY, 7) OP(FC, IGN, ABX, 4) OP(FD, SBC, ABX, 4) OP(FE, INC, ABX, 7) OP(FF, ISC, ABX, 7)
        }
    #undef OP

        // unused flag is always set
        flag.U = 1;
    }

    cycles--;
}

void Cpu6502::reset()
{
    reg_A = 0;
    reg_X = 0;
    reg_Y = 0;
    reg_SP = 0xFD;
    reg_status = 0;
    flag.U = 1;

    addr_abs = 0xFFFC;

    reg_PC_byte.low = read(addr_abs);
    reg_PC_byte.high = read(addr_abs + 1);

    addr_rel = 0;
    addr_abs = 0;
    fetched = 0;

    cycles = 8;
}

void Cpu6502::irq()
{
    if(flag.I == 0)
    {
        // push PC
        write(0x0100 + reg_SP, reg_PC_byte.high);
        reg_SP--;
        write(0x0100 + reg_SP, reg_PC_byte.low);
        reg_SP--;

        // push status
        flag.B = 0;
        flag.U = 1;
        flag.I = 1;
        write(0x0100 + reg_SP, reg_status);
        reg_SP--;

        // goto irq vector
        addr_abs = 0xFFFE;
        reg_PC_byte.low = read(addr_abs);
        reg_PC_byte.high = read(addr_abs + 1);

        cycles = 7;
    }
}

void Cpu6502::nmi()
{
    // push PC
    write(0x0100 + reg_SP, reg_PC_byte.high);
    reg_SP--;
    write(0x0100 + reg_SP, reg_PC_byte.low);
    reg_SP--;

    // push status
    flag.B = 0;
    flag.U = 1;
    flag.I = 1;
    write(0x0100 + reg_SP, reg_status);
    reg_SP--;

    // goto nmi vector
    addr_abs = 0xFFFA;
    reg_PC_byte.low = read(addr_abs);
    reg_PC_byte.high = read(addr_abs + 1);

    cycles = 8;
}

// Addressing Modes 

uint8_t Cpu6502::IMP()
{
    return 0;
}

uint8_t Cpu6502::IMM()
{
    addr_abs = reg_PC;
    reg_PC++;
    return 0;
}

uint8_t Cpu6502::ZP0()
{
    addr_abs = read(reg_PC);
    reg_PC++;
    addr_abs &= 0x00FF;
    return 0;
}

uint8_t Cpu6502::ZPX()
{
    addr_abs = (read(reg_PC) + reg_X);
    reg_PC++;
    addr_abs &= 0x00FF;
    return 0;
}

uint8_t Cpu6502::ZPY()
{
    addr_abs = (read(reg_PC) + reg_Y);
    reg_PC++;
    addr_abs &= 0x00FF;
    return 0;
}

uint8_t Cpu6502::REL()
{
    addr_rel = read(reg_PC);
    reg_PC++;
    // the read offset is signed, so if the hi bit is set, we set all the hi bits for overflow to work
    if(addr_rel & 0x80)
        addr_rel |= 0xFF00;

    return 0;
}

uint8_t Cpu6502::ABS()
{
    addr_abs_byte.low = read(reg_PC);
    reg_PC++;
    addr_abs_byte.high = read(reg_PC);
    reg_PC++;
    return 0;
}

uint8_t Cpu6502::ABX()
{
    addr_abs_byte.low = read(reg_PC);
    reg_PC++;
    uint8_t hi = read(reg_PC);
    addr_abs_byte.high = hi;
    reg_PC++;

    addr_abs += reg_X;
    return addr_abs_byte.high != hi;
}

uint8_t Cpu6502::ABY()
{
    addr_abs_byte.low = read(reg_PC);
    reg_PC++;
    uint8_t hi = read(reg_PC);
    reg_PC++;
    addr_abs_byte.high = hi;

    addr_abs += reg_Y;
    return addr_abs_byte.high != hi;
}

uint8_t Cpu6502::IND()
{
    union
    {
        uint16_t ptr = 0x0000;
        struct
        {
            uint8_t low : 8;
            uint8_t high : 8;
        } ptr_byte;
        
    };

    ptr_byte.low = read(reg_PC);
    reg_PC++;
    ptr_byte.high = read(reg_PC);
    reg_PC++;

    addr_abs_byte.low = read(ptr);
    ptr_byte.low++; // bug JMP (a) // (#11FF read #11FF and #1100 instead of #1200)
    addr_abs_byte.high = read(ptr);

    return 0;
}

uint8_t Cpu6502::IZX()
{
    union
    {
        uint16_t ptr = 0x0000;
        struct
        {
            uint8_t low : 8;
            uint8_t high : 8;
        } ptr_byte;
        
    };

    ptr_byte.low = read(reg_PC);
    reg_PC++;

    ptr += reg_X;

    addr_abs_byte.low = read(ptr & 0x00FF);
    addr_abs_byte.high = read((ptr+1) & 0x00FF);

    return 0;
}

uint8_t Cpu6502::IZY()
{
    union
    {
        uint16_t ptr = 0x0000;
        struct
        {
            uint8_t low : 8;
            uint8_t high : 8;
        } ptr_byte;
        
    };

    ptr_byte.low = read(reg_PC);
    reg_PC++;

    addr_abs_byte.low = read(ptr & 0x00FF);
    uint8_t hi = read((ptr+1) & 0x00FF);
    addr_abs_byte.high = hi;

    addr_abs += reg_Y;

    return addr_abs_byte.high != hi;
}

// Instructions

uint8_t Cpu6502::fetch()
{
    fetched = read(addr_abs);
    return fetched;
}

uint8_t Cpu6502::ADC() 
{
    fetch();
    uint16_t tmp = reg_A + fetched + flag.C;

    flag.N = (tmp & 0x0080) > 0;
    ///  the sign of the result differs from the sign of both the input and the accumulator
    flag.V = ((tmp & 0x0080) != (reg_A & 0x80)) && ((tmp & 0x0080) != (fetched & 0x80));
    flag.Z = (tmp & 0x00FF) == 0;
    flag.C = (tmp & 0x0100) > 0;
    reg_A = tmp & 0x00FF;
    return 1;
}

uint8_t Cpu6502::AND()
{
    fetch();
    reg_A = reg_A & fetched;
    flag.Z = reg_A == 0;
    flag.N = (reg_A & 0x80) > 0;
    return 1;
}

uint8_t Cpu6502::ASL()
{
    fetch();
    flag.C = (fetched & 0b1000'0000) >> 7;
    fetched = fetched << 1;
    flag.N = (fetched & 0x80) > 0;
    flag.Z = fetched == 0;
    write(addr_abs, fetched);

    return 0;
} 

uint8_t Cpu6502::AASL()
{
    flag.C = (reg_A & 0b1000'0000) >> 7;
    reg_A = reg_A << 1;
    flag.N = (reg_A & 0x80) > 0;
    flag.Z = reg_A == 0;

    return 0;
} 

uint8_t Cpu6502::BCC()
{
    if(!flag.C)
    {
        cycles++;
        addr_abs = reg_PC + addr_rel;

        cycles += (addr_abs & 0xFF00) != (reg_PC & 0xFF00);

        reg_PC = addr_abs;
    }
    return 0;
}

uint8_t Cpu6502::BCS() 
{
    if(flag.C)
    {
        cycles++;
        addr_abs = reg_PC + addr_rel;

        cycles += (addr_abs & 0xFF00) != (reg_PC & 0xFF00);

        reg_PC = addr_abs;
    }
    return 0;
} 

uint8_t Cpu6502::BEQ()
{
    if(flag.Z)
    {
        cycles++;
        addr_abs = reg_PC + addr_rel;

        cycles += (addr_abs & 0xFF00) != (reg_PC & 0xFF00);

        reg_PC = addr_abs;
    }
    return 0;
} 

uint8_t Cpu6502::BIT()
{
    fetch();
    uint8_t tmp = reg_A & fetched;
    flag.N = (fetched & 0b1000'0000) > 0;
    flag.V = (fetched & 0b0100'0000) > 0;
    flag.Z = tmp == 0;
    return 0;
} 

uint8_t Cpu6502::BMI()
{
    if(flag.N)
    {
        cycles++;
        addr_abs = reg_PC + addr_rel;

        cycles += (addr_abs & 0xFF00) != (reg_PC & 0xFF00);

        reg_PC = addr_abs;
    }
    return 0;
}

uint8_t Cpu6502::BNE() 
{
    if(!flag.Z)
    {
        cycles++;
        addr_abs = reg_PC + addr_rel;

        cycles += (addr_abs & 0xFF00) != (reg_PC & 0xFF00);

        reg_PC = addr_abs;
    }
    return 0;
} 

uint8_t Cpu6502::BPL()
{
    if(!flag.N)
    {
        cycles++;
        addr_abs = reg_PC + addr_rel;

        cycles += (addr_abs & 0xFF00) != (reg_PC & 0xFF00);

        reg_PC = addr_abs;
    }
    return 0;
} 

uint8_t Cpu6502::BRK()
{
    reg_PC++;

    // push PC
    write(0x0100 + reg_SP, reg_PC_byte.high);
    reg_SP--;
    write(0x0100 + reg_SP, reg_PC_byte.low);
    reg_SP--;

    // push status
    flag.B = 1;
    write(0x0100 + reg_SP, reg_status);
    reg_SP--;
    flag.I = 1;
    flag.B = 0;

    // goto irq vector
    addr_abs = 0xFFFE;
    reg_PC_byte.low = read(addr_abs);
    reg_PC_byte.high = read(addr_abs + 1);

    return 0;
} 

uint8_t Cpu6502::BVC()
{
    if(!flag.V)
    {
        cycles++;
        addr_abs = reg_PC + addr_rel;

        cycles += (addr_abs & 0xFF00) != (reg_PC & 0xFF00);

        reg_PC = addr_abs;
    }
    return 0;
}

uint8_t Cpu6502::BVS() 
{
    if(flag.V)
    {
        cycles++;
        addr_abs = reg_PC + addr_rel;

        cycles += (addr_abs & 0xFF00) != (reg_PC & 0xFF00);

        reg_PC = addr_abs;
    }
    return 0;
} 

uint8_t Cpu6502::CLC()
{
    flag.C = 0;
    return 0;
} 

uint8_t Cpu6502::CLD()
{
    flag.D = 0;
    return 0;
} 

uint8_t Cpu6502::CLI()
{
    flag.I = 0;
    return 0;
}

uint8_t Cpu6502::CLV() 
{
    flag.V = 0;
    return 0;
} 

uint8_t Cpu6502::CMP()
{
    fetch();
    uint8_t tmp = reg_A - fetched;
    flag.N = (tmp & 0b1000'0000) > 0;
    flag.Z = reg_A == fetched;
    flag.C = reg_A >= fetched;
    return 0;
} 

uint8_t Cpu6502::CPX()
{
    fetch();
    uint8_t tmp = reg_X - fetched;
    flag.N = (tmp & 0b1000'0000) > 0;
    flag.Z = reg_X == fetched;
    flag.C = reg_X >= fetched;
    return 0;
} 

uint8_t Cpu6502::CPY()
{
    fetch();
    uint8_t tmp = reg_Y - fetched;
    flag.N = (tmp & 0b1000'0000) > 0;
    flag.Z = reg_Y == fetched;
    flag.C = reg_Y >= fetched;
    return 0;
}

uint8_t Cpu6502::DEC() 
{
    fetch();
    fetched--;
    write(addr_abs, fetched);
    flag.N = (fetched & 0x80) > 0;
    flag.Z = fetched == 0;
    return 0;
} 

uint8_t Cpu6502::DEX()
{
    reg_X--;
    flag.N = (reg_X & 0x80) > 0;
    flag.Z = reg_X == 0;
    return 0;
} 

uint8_t Cpu6502::DEY()
{
    reg_Y--;
    flag.N = (reg_Y & 0x80) > 0;
    flag.Z = reg_Y == 0;
    return 0;
} 

uint8_t Cpu6502::EOR()
{
    fetch();
    reg_A = fetched ^ reg_A;
    flag.N = (reg_A & 0x80) > 0;
    flag.Z = reg_A == 0;
    return 1;
}

uint8_t Cpu6502::INC() 
{
    fetch();
    fetched++;
    write(addr_abs, fetched);
    flag.N = (fetched & 0x80) > 0;
    flag.Z = fetched == 0;
    return 0;
} 

uint8_t Cpu6502::INX()
{
    reg_X++;
    flag.N = (reg_X & 0x80) > 0;
    flag.Z = reg_X == 0;
    return 0;
} 

uint8_t Cpu6502::INY()
{
    reg_Y++;
    flag.N = (reg_Y & 0x80) > 0;
    flag.Z = reg_Y == 0;
    return 0;
} 

uint8_t Cpu6502::JMP()
{
    reg_PC = addr_abs;
    return 0;
}

uint8_t Cpu6502::JSR() 
{
    reg_PC--;

    write(0x0100 + reg_SP, reg_PC_byte.high);
    reg_SP--;
    write(0x0100 + reg_SP, reg_PC_byte.low);
    reg_SP--;

    reg_PC = addr_abs;
    return 0;
} 

uint8_t Cpu6502::LDA()
{
    fetch();
    reg_A = fetched;
    flag.N = (reg_A & 0x80) > 0;
    flag.Z = reg_A == 0;
    return 1;
} 

uint8_t Cpu6502::LDX()
{
    fetch();
    reg_X = fetched;
    flag.N = (reg_X & 0x80) > 0;
    flag.Z = reg_X == 0;
    return 1;
} 

uint8_t Cpu6502::LDY()
{
    fetch();
    reg_Y = fetched;
    flag.N = (reg_Y & 0x80) > 0;
    flag.Z = reg_Y == 0;
    return 1;
}

uint8_t Cpu6502::LSR() 
{
    fetch();
    flag.C = fetched & 0b0000'0001;
    fetched = fetched >> 1;
    write(addr_abs, fetched);
    flag.N = 0;
    flag.Z = fetched == 0;
    return 0;
} 

uint8_t Cpu6502::ALSR() 
{
    flag.C = reg_A & 0b0000'0001;
    reg_A = reg_A >> 1;
    flag.N = 0;
    flag.Z = reg_A == 0;
    return 0;
} 

uint8_t Cpu6502::NOP()
{
	return 0;
} 

uint8_t Cpu6502::ORA()
{
    fetch();
    reg_A = fetched | reg_A;
    flag.N = (reg_A & 0x80) > 0;
    flag.Z = reg_A == 0;
    return 1;
} 

uint8_t Cpu6502::PHA()
{
    write(0x0100 + reg_SP, reg_A);
    reg_SP--;
    return 0;
}

uint8_t Cpu6502::PHP() 
{
    flag.B = 1;
    flag.U = 1;
    write(0x0100 + reg_SP, reg_status);
    reg_SP--;

    flag.B = 0;
    flag.U = 0;

    return 0;
} 

uint8_t Cpu6502::PLA()
{
    reg_SP++;
    reg_A = read(0x0100 + reg_SP);
    flag.Z = reg_A == 0;
    flag.N = (reg_A & 0x80) > 0;
    return 0;
} 

uint8_t Cpu6502::PLP()
{
    reg_SP++;
    reg_status = read(0x0100 + reg_SP);
    flag.U = 1;
    return 0;
} 

uint8_t Cpu6502::ROL()
{
    fetch();
    uint8_t tmp = flag.C;
    flag.C = (fetched & 0b1000'0000) > 0;
    fetched = fetched << 1;
    fetched |= tmp;
    flag.N = (fetched & 0x80) > 0;
    flag.Z = fetched == 0;
    write(addr_abs, fetched);

    return 0;
} 

uint8_t Cpu6502::AROL()
{
    uint8_t tmp = flag.C;
    flag.C = (reg_A & 0b1000'0000) > 0;
    reg_A = reg_A << 1;
    reg_A |= tmp;
    flag.N = (reg_A & 0x80) > 0;
    flag.Z = reg_A == 0;

    return 0;
} 

uint8_t Cpu6502::ROR() 
{
    fetch();
    uint8_t tmp = flag.C << 7;
    flag.C = fetched & 0b0000'0001;
    fetched = fetched >> 1;
    fetched |= tmp;
    flag.N = (fetched & 0x80) > 0;
    flag.Z = fetched == 0;
    write(addr_abs, fetched);
    
    return 0;
} 

uint8_t Cpu6502::AROR() 
{
    uint8_t tmp = flag.C << 7;
    flag.C = reg_A & 0b0000'0001;
    reg_A = reg_A >> 1;
    reg_A |= tmp;
    flag.N = (reg_A & 0x80) > 0;
    flag.Z = reg_A == 0;
    
    return 0;
} 

uint8_t Cpu6502::RTI()
{
    // pop SR
    reg_SP++;
    reg_status = read(0x0100 + reg_SP);
    flag.B = 0;
    flag.U = 0;

    // pop PC
    reg_SP++;
    reg_PC_byte.low = read(0x0100 + reg_SP);
    reg_SP++;
    reg_PC_byte.high = read(0x0100 + reg_SP);
    return 0;
} 

uint8_t Cpu6502::RTS()
{
    // pop PC
    reg_SP++;
    reg_PC_byte.low = read(0x0100 + reg_SP);
    reg_SP++;
    reg_PC_byte.high = read(0x0100 + reg_SP);

    reg_PC++;

    return 0;
} 

uint8_t Cpu6502::SBC()
{
    fetch();
    fetched = ~fetched;
    
    uint16_t tmp = reg_A + fetched + flag.C;

    flag.N = (tmp & 0x0080) > 0;
    ///  the sign of the result differs from the sign of both the input and the accumulator
    flag.V = ((tmp & 0x0080) != (reg_A & 0x80)) && ((tmp & 0x0080) != (fetched & 0x80));
    flag.Z = (tmp & 0x00FF) == 0;
    flag.C = (tmp & 0x0100) > 0;
    reg_A = tmp & 0x00FF;

    return 1;
}

uint8_t Cpu6502::SEC() 
{
    flag.C = 1;
    return 0;
} 

uint8_t Cpu6502::SED()
{
    flag.D = 1;
    return 0;
} 

uint8_t Cpu6502::SEI()
{
    flag.I = 1;
    return 0;
} 

uint8_t Cpu6502::STA()
{
    write(addr_abs, reg_A);
    return 0;
} 

uint8_t Cpu6502::STX() 
{
    write(addr_abs, reg_X);
    return 0;
} 

uint8_t Cpu6502::STY()
{
    write(addr_abs, reg_Y);
    return 0;
} 

uint8_t Cpu6502::TAX()
{
    reg_X = reg_A;
    flag.N = (reg_X & 0x80) > 0;
    flag.Z = reg_X == 0;
    return 0;
} 

uint8_t Cpu6502::TAY()
{
    reg_Y = reg_A;
    flag.N = (reg_Y & 0x80) > 0;
    flag.Z = reg_Y == 0;
    return 0;
} 

uint8_t Cpu6502::TSX() 
{
    reg_X = reg_SP;
    flag.N = (reg_X & 0x80) > 0;
    flag.Z = reg_X == 0;
    return 0;
} 

uint8_t Cpu6502::TXA()
{
    reg_A = reg_X;
    flag.N = (reg_A & 0x80) > 0;
    flag.Z = reg_A == 0;
    return 0;
} 

uint8_t Cpu6502::TXS()
{
    reg_SP = reg_X;
    return 0;
} 

uint8_t Cpu6502::TYA()
{
    reg_A = reg_Y;
    flag.N = (reg_A & 0x80) > 0;
    flag.Z = reg_A == 0;
    return 0;
}


// Illegal Instructions

uint8_t Cpu6502::ALR()
{
    fetch();
    // AND
    reg_A = reg_A & fetched;
    // ALSR
    flag.C = reg_A & 0b0000'0001;
    reg_A = reg_A >> 1;
    flag.N = 0;
    flag.Z = reg_A == 0;

    return 0;
}

uint8_t Cpu6502::ANC()
{
    fetch();
    // AND
    reg_A = reg_A & fetched;
    flag.Z = reg_A == 0;
    flag.N = (reg_A & 0x80) > 0;
    flag.C = flag.N;

    return 0;
}

uint8_t Cpu6502::ARR()
{
    fetch();
    // AND
    reg_A = reg_A & fetched;
    // AROR
    uint8_t tmp = flag.C << 7;
    reg_A = reg_A >> 1;
    reg_A |= tmp;
    flag.N = (reg_A & 0x80) > 0;
    flag.Z = reg_A == 0;
    flag.C = (reg_A & 0x40) > 0;
    flag.V = flag.C ^ ((reg_A & 0x20) > 0);

    return 0;
}

uint8_t Cpu6502::AXS()
{
    fetch();
    uint8_t tmp = reg_A & reg_X;
    reg_X = tmp - fetched;
    flag.N = (reg_X & 0b1000'0000) > 0;
    flag.Z = tmp == fetched;
    flag.C = tmp >= fetched;

    return 0;
}

uint8_t Cpu6502::LAX()
{
    fetch();
    // LDA
    reg_A = fetched;
    // TAX
    reg_X = reg_A;
    flag.N = (reg_X & 0x80) > 0;
    flag.Z = reg_X == 0;

    return 1;
}

uint8_t Cpu6502::SAX()
{
    write(addr_abs, reg_A & reg_X);
    return 0;
}

uint8_t Cpu6502::DCP()
{
    fetch();
    // DEC
    fetched--;
    write(addr_abs, fetched);
    // CMP
    uint8_t tmp = reg_A - fetched;
    flag.N = (tmp & 0b1000'0000) > 0;
    flag.Z = reg_A == fetched;
    flag.C = reg_A >= fetched;

    return 0;
}

uint8_t Cpu6502::ISC()
{
    fetch();
    // INC
    fetched++;
    write(addr_abs, fetched);
    // SBC
    fetched = ~fetched;
    
    uint16_t tmp = reg_A + fetched + flag.C;

    flag.N = (tmp & 0x0080) > 0;
    ///  the sign of the result differs from the sign of both the input and the accumulator
    flag.V = ((tmp & 0x0080) != (reg_A & 0x80)) && ((tmp & 0x0080) != (fetched & 0x80));
    flag.Z = (tmp & 0x00FF) == 0;
    flag.C = (tmp & 0x0100) > 0;
    reg_A = tmp & 0x00FF;

    return 0;
}

uint8_t Cpu6502::RLA()
{
    fetch();
    // ROL
    uint8_t tmp = flag.C;
    flag.C = (fetched & 0b1000'0000) > 0;
    fetched = fetched << 1;
    fetched |= tmp;
    write(addr_abs, fetched);
    // AND
    reg_A = reg_A & fetched;
    flag.Z = reg_A == 0;
    flag.N = (reg_A & 0x80) > 0;

    return 0;
}

uint8_t Cpu6502::RRA()
{
    fetch();
    // ROR
    uint16_t tmp = flag.C << 7;
    flag.C = fetched & 0b0000'0001;
    fetched = fetched >> 1;
    fetched |= (tmp & 0x00FF);
    write(addr_abs, fetched);
    // ADC
    tmp = reg_A + fetched + flag.C;
    flag.N = (tmp & 0x0080) > 0;
    ///  the sign of the result differs from the sign of both the input and the accumulator
    flag.V = ((tmp & 0x0080) != (reg_A & 0x80)) && ((tmp & 0x0080) != (fetched & 0x80));
    flag.Z = (tmp & 0x00FF) == 0;
    flag.C = (tmp & 0x0100) > 0;
    reg_A = tmp & 0x00FF;

    return 0;
}

uint8_t Cpu6502::SLO()
{
    fetch();
    // ASL
    flag.C = (fetched & 0b1000'0000) >> 7;
    fetched = fetched << 1;
    write(addr_abs, fetched);
    // ORA
    reg_A = fetched | reg_A;
    flag.N = (reg_A & 0x80) > 0;
    flag.Z = reg_A == 0;

    return 0;
}

uint8_t Cpu6502::SRE()
{
    fetch();
    // LSR
    flag.C = fetched & 0b0000'0001;
    fetched = fetched >> 1;
    write(addr_abs, fetched);
    // EOR
    reg_A = fetched ^ reg_A;
    flag.N = (reg_A & 0x80) > 0;
    flag.Z = reg_A == 0;

    return 0;
}

uint8_t Cpu6502::SKB()
{
    fetch();
    return 0;
}

uint8_t Cpu6502::IGN()
{
    fetch();
    return 1;
}

uint8_t Cpu6502::STP()
{
    throw std::runtime_error("STP instruction reached");
    return 0;
}

uint8_t Cpu6502::XAA()
{
    fetch();
    reg_A = reg_X & fetched;
    flag.Z = reg_A == 0;
    flag.N = (reg_A & 0x80) > 0;

    return 0;
}

uint8_t Cpu6502::SHX()
{
    uint8_t H = addr_abs_byte.high;

    if(addr_page_boundary_crossed)
    {   // Boundary crossed, SHX goes unstable
        // NOTE: H was incremented in ABY as part of normal operation
        addr_abs = (addr_abs & 0x00FF) | (addr_abs & 0xFF00 & (reg_X << 8));
    }
    else
    {
        ++H;
    }

    uint8_t tmp = reg_X & H;
    write(addr_abs, tmp);
    return 0;
}

uint8_t Cpu6502::SHY()
{
    uint8_t H = addr_abs_byte.high;

    if(addr_page_boundary_crossed)
    {   // Boundary crossed, SHY goes unstable
        // NOTE: H was incremented in ABX as part of normal operation
        addr_abs = (addr_abs & 0x00FF) | (addr_abs & 0xFF00 & (reg_Y << 8));
    }
    else
    {
        ++H;
    }

    uint8_t tmp = reg_Y & H;
    write(addr_abs, tmp);
    return 0;
}

uint8_t Cpu6502::AHX()
{
    uint8_t H = addr_abs_byte.high;

    if(addr_page_boundary_crossed)
    {   // Boundary crossed, SHX goes unstable
        // NOTE: H was incremented in ABX as part of normal operation
        addr_abs = (addr_abs & 0x00FF) | (addr_abs & 0xFF00 & (reg_X << 8));
    }
    else
    {
        ++H;
    }

    const uint8_t magic = 0xF5; // Magic constant, could be anything
    uint8_t tmp = reg_A & (reg_X | magic) & H;
    write(addr_abs, tmp);

    return 0;
}

uint8_t Cpu6502::XXX()
{
    return 0;
}