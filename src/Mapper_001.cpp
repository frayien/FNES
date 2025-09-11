#include "Mapper_001.h"
#include "Mapper.h"

Mapper_001::Mapper_001(uint8_t _prg_count, uint8_t _chr_count) :  Mapper(_prg_count,_chr_count)
{
    
}

Mapper_001::~Mapper_001()
{
}


bool Mapper_001::cpuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x6000 && addr <= 0x7FFF) // prg ram
    {
        memory_kind = MemoryKind::RAM;
        return true;
    }
    else if(addr >= 0x8000 && addr <= 0xBFFF) // prg rom, switchable or fixed to the first bank
    {
        mapper_addr = addr;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    else if(addr >= 0xC000 && addr <= 0xFFFF) // prg rom, fixed to the last bank or switchable
    {
        mapper_addr = addr;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false; 
}
bool Mapper_001::cpuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x6000 && addr <= 0x7FFF) // prg ram
    {
        memory_kind = MemoryKind::RAM;
        return true;
    }
    else if(addr >= 0x8000 && addr <= 0xBFFF) // prg rom, switchable or fixed to the first bank
    {
        mapper_addr = addr;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    else if(addr >= 0xC000 && addr <= 0xFFFF) // prg rom, fixed to the last bank or switchable
    {
        mapper_addr = addr;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false; 
}

bool Mapper_001::ppuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x0000 && addr <= 0x1FFF)
    {
        mapper_addr = addr;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false; 
}
bool Mapper_001::ppuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x0000 && addr <= 0x1FFF)
    {
        mapper_addr = addr;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false; 
}