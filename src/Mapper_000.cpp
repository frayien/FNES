#include "Mapper_000.h"
#include "Mapper.h"

Mapper_000::Mapper_000(uint8_t _prg_count, uint8_t _chr_count) :  Mapper(_prg_count,_chr_count)
{
    cpu_prg_mode_mask = prg_count > 1 ? 0x7FFF : 0x3FFF;
}

Mapper_000::~Mapper_000()
{
}


bool Mapper_000::cpuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x8000 && addr <= 0xFFFF)
    {
        mapper_addr = addr & cpu_prg_mode_mask;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false; 
}
bool Mapper_000::cpuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x8000 && addr <= 0xFFFF)
    {
        mapper_addr = addr & cpu_prg_mode_mask;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false; 
}

bool Mapper_000::ppuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x0000 && addr <= 0x1FFF)
    {
        mapper_addr = addr;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false; 
}
bool Mapper_000::ppuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x0000 && addr <= 0x1FFF)
    {
        mapper_addr = addr;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false; 
}