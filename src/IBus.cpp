#include "IBus.h"

#include <sstream>

std::string hex(uint32_t n, uint8_t ln)
{
    std::string s(ln, '0');
    for(int i = ln-1; i>=0; i--, n >>= 4) { s[i] = "0123456789ABCDEF"[n & 0xF]; }
    return s;
}

IBus::IBus()
{
}

IBus::~IBus()
{
}


std::map<uint16_t, std::string> IBus::decompile(uint16_t start, uint16_t end)
{
    std::map<uint16_t, std::string> ret;

    for(uint32_t ptr = start; ptr<end; )
    {
        uint16_t addr = ptr;
        std::stringstream inst;
        inst << "$" << hex(ptr,4) << ": ";

        uint8_t opc = cpuRead(ptr++, true);

        OPCODE opcode = lookup[opc];
        inst << opcode.instruction << " ";

        if(opcode.addrmode == "IMP" || opcode.addrmode == "ACC")
        {
            // nothing
            inst << " ";
        }
        else if(opcode.addrmode == "IMM")
        {
            inst << "#$" << hex(cpuRead(ptr++, true),2) << " ";
        }
        else if(opcode.addrmode == "ABS")
        {
            uint8_t lo = cpuRead(ptr++, true);
            inst << "$" << hex(cpuRead(ptr++, true),2) << hex(lo,2) << " ";
        }
        else if(opcode.addrmode == "ZP0")
        {
            inst << "$" << hex(cpuRead(ptr++, true),2) << " ";
        }
        else if(opcode.addrmode == "REL")
        {
            uint8_t rel = cpuRead(ptr++, true);
            uint16_t ref = ptr + ((signed char)rel);
            inst << "$" << hex(rel,2) << " [$" << hex(ref,4) << "] ";
        }
        else if(opcode.addrmode == "IND")
        {
            uint8_t lo = cpuRead(ptr++, true);
            inst << "($" << hex(cpuRead(ptr++, true),2) << hex(lo,2) << ") ";
        }
        else if(opcode.addrmode == "ABX")
        {
            uint8_t lo = cpuRead(ptr++, true);
            inst << "$" << hex(cpuRead(ptr++, true),2) << hex(lo,2) << ",X ";
        }
        else if(opcode.addrmode == "ABY")
        {
            uint8_t lo = cpuRead(ptr++, true);
            inst << "$" << hex(cpuRead(ptr++, true),2) << hex(lo,2) << ",Y ";
        }
        else if(opcode.addrmode == "ZPX")
        {
            inst << "$" << hex(cpuRead(ptr++, true),2) << ",X ";
        }
        else if(opcode.addrmode == "ZPY")
        {
            inst << "$" << hex(cpuRead(ptr++, true),2) << ",Y ";
        }
        else if(opcode.addrmode == "IZX")
        {
            inst << "($" << hex(cpuRead(ptr++, true),2) << ",X) ";
        }
        else if(opcode.addrmode == "IZY")
        {
            inst << "($" << hex(cpuRead(ptr++, true),2) << "),Y ";
        }

        inst << "{" << opcode.addrmode << "}";

        ret[addr] = inst.str();
    }

    return ret;
}