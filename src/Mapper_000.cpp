#include "Mapper_000.hpp"
#include "Mapper.hpp"
#include "Cartridge.hpp"

#include <stdexcept>

Mapper_000::Mapper_000(const Cartridge & cartridge)
{
    switch(cartridge.prg_rom_size)
    {
        case 0x4000: cpu_prg_rom_mask = 0x3FFF; break;
        case 0x8000: cpu_prg_rom_mask = 0x7FFF; break;
        default: throw std::invalid_argument("Invalid PRG-ROM size");
    }

    switch(cartridge.prg_ram_size)
    {
        case 0x000: cpu_prg_ram_mask = 0x000; break;
        case 0x400: cpu_prg_ram_mask = 0x3FF; break;
        case 0x800: cpu_prg_ram_mask = 0x7FF; break;
        default: throw std::invalid_argument("Invalid PRG-RAM size");
    }

    switch(cartridge.chr_rom_size)
    {
        case 0x0000: has_chr_rom = false; break;
        case 0x2000: has_chr_rom = true; break;
        default: throw std::invalid_argument("Invalid CHR-ROM size");
    }

    mirror_mode = cartridge.hardwired_nametable_mirroring_type_is_vertical ? MirrorMode::VERTICAL : MirrorMode::HORIZONTAL;
}

bool Mapper_000::cpuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x6000 && addr <= 0x7FFF && cpu_prg_ram_mask != 0)
    {
        mapper_addr = addr & cpu_prg_ram_mask;
        memory_kind = MemoryKind::RAM;
        return true;
    }
    else if(addr >= 0x8000 && addr <= 0xFFFF)
    {
        mapper_addr = addr & cpu_prg_rom_mask;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false;
}
bool Mapper_000::cpuMapWrite(uint16_t addr, uint8_t data, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x6000 && addr <= 0x7FFF && cpu_prg_ram_mask != 0)
    {
        mapper_addr = addr & cpu_prg_ram_mask;
        memory_kind = MemoryKind::RAM;
        return true;
    }
    else if(addr >= 0x8000 && addr <= 0xFFFF)
    {
        mapper_addr = addr & cpu_prg_rom_mask;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false;
}

bool Mapper_000::ppuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x0000 && addr <= 0x1FFF && has_chr_rom)
    {
        mapper_addr = addr;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false;
}
bool Mapper_000::ppuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x0000 && addr <= 0x1FFF && has_chr_rom)
    {
        mapper_addr = addr;
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false;
}

MirrorMode Mapper_000::mirror() const
{
    return mirror_mode;
}
