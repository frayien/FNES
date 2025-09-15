#include "Mapper_001.hpp"
#include "Mapper.hpp"
#include "Cartridge.hpp"

#include <stdexcept>

Mapper_001::Mapper_001(Cartridge & cartridge)
{
    switch(cartridge.submapper_number)
    {
    case 0: variant = Variant::DEFAULT; break;
    case 5: variant = Variant::SEROM_SHROM_SH1ROM; break;
    case 6: variant = Variant::FCNS_2ME; break;
    case 7: variant = Variant::KS_7058; break;
    default: throw std::invalid_argument("Invalid Submapper");
    }

    // PRG-(NV)RAM
    uint32_t l_prg_ram_size = 0;
    if(!cartridge.is_NES20_format)
    { // Old NES format, prg_ram_is not specified in the header, fallback to 32KiB
        prg_ram_kind = MemoryKind::RAM;
        cartridge.prg_ram_size = 0x8000;
        l_prg_ram_size = 0x8000;
    }
    else if(cartridge.prg_nvram_size == 0)
    { // RAM
        prg_ram_kind = MemoryKind::RAM;
        l_prg_ram_size = cartridge.prg_ram_size;
    }
    else
    { // NVRAM
        if(cartridge.prg_ram_size != 0) { throw std::invalid_argument("Mapper 1 dont support PRG-RAM and PRG-NVRAM at the same time"); }

        prg_ram_kind = MemoryKind::NVRAM;
        l_prg_ram_size = cartridge.prg_nvram_size;
    }
    if(l_prg_ram_size % 0x2000 != 0)
    { throw std::invalid_argument("Invalid PRG-(NV)RAM size"); }
    prg_ram_bank_count = l_prg_ram_size / 0x2000;

    // PRG-ROM
    if(cartridge.prg_rom_size == 0 || cartridge.prg_rom_size % 0x4000 != 0)
    { throw std::invalid_argument("Invalid PRG-ROM size"); }
    prg_rom_bank_count = cartridge.prg_rom_size / 0x4000;

    // CHR-ROM/RAM
    uint32_t l_chr_size = 0;
    if(cartridge.chr_ram_size == 0)
    { // ROM
        chr_mem_kind = MemoryKind::ROM;
        l_chr_size = cartridge.chr_rom_size;
    }
    else
    { // RAM
        if(cartridge.chr_rom_size != 0) { throw std::invalid_argument("Mapper 1 dont support CHR-ROM and CHR-RAM at the same time"); }

        chr_mem_kind = MemoryKind::RAM;
        l_chr_size = cartridge.chr_ram_size;
    }
    if(l_chr_size % 0x1000 != 0)
    { throw std::invalid_argument("Invalid CHR-ROM/RAM size"); }
    chr_bank_count = l_chr_size / 0x1000;

    // Validation
    switch(variant)
    {
    case Variant::SEROM_SHROM_SH1ROM:
    { if(prg_rom_bank_count != 2) { throw std::invalid_argument("SEROM/SHROM/SH1ROM only support exactly 32KiB PRG-ROM"); } break; }
    case Variant::FCNS_2ME:
    { throw std::runtime_error("Not Implemented"); }
    case Variant::KS_7058:
    { throw std::runtime_error("Not Implemented"); }
    case Variant::DEFAULT: [[fallthrough]];
    default:
    {
        // if(prg_ram_bank_count == 1 && prg_nvram_bank_count == 1 && chr_bank_count >= 4)
        // { variant = Variant::SZROM; }
        // else if(chr_bank_count <= 2 && prg_rom_bank_count > 16)
        // { variant = Variant::SOROM_SUROM_SXROM; }
        // TODO SNROM
        break;
    }
    };

    // init
    prg_ram_bank_idx = 0;

    prg_rom_lo_bank_idx = 0;
    prg_rom_hi_bank_idx = prg_rom_bank_count-1;

    chr_lo_bank_idx = 0;
    chr_hi_bank_idx = chr_bank_count-1;

    reg_Shift = 0b1'0000;
    reg_Control = 0b0'1100;
    reg_CHR0 = 0;
    reg_CHR1 = 0;
    reg_PRG = 0;

    update_values_from_registers();
}

bool Mapper_001::cpuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x6000 && addr <= 0x7FFF && prg_ram_bank_count > 0) // prg (nv)ram
    {
        mapper_addr = 0x2000 * prg_ram_bank_idx + (addr & 0x1FFF);
        memory_kind = prg_ram_kind;
        return true;
    }
    else if(addr >= 0x8000 && addr <= 0xBFFF) // prg rom, switchable or fixed to the first bank
    {
        mapper_addr = 0x4000 * prg_rom_lo_bank_idx + (addr & 0x3FFF);
        memory_kind = MemoryKind::ROM;
        return true;
    }
    else if(addr >= 0xC000 && addr <= 0xFFFF) // prg rom, fixed to the last bank or switchable
    {
        mapper_addr = 0x4000 * prg_rom_hi_bank_idx + (addr & 0x3FFF);
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false;
}
bool Mapper_001::cpuMapWrite(uint16_t addr, uint8_t data, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    // Register
    if(addr >= 0x8000 && addr <= 0xFFFF)
    {
        bool shift_end_reached = (reg_Shift & 0x01) > 0;
        reg_Shift >>= 1;
        reg_Shift |= (data & 0x01) << 4;

        if(shift_end_reached)
        { // Write
            switch(addr & 0xE000)
            {
            case 0x8000: reg_Control = reg_Shift; break;
            case 0xA000: reg_CHR0    = reg_Shift; break;
            case 0xC000: reg_CHR1    = reg_Shift; break;
            case 0xE000: reg_PRG     = reg_Shift; break;
            default: throw std::runtime_error("Unreachable");
            }
            reg_Shift = 0b1'0000;

            update_values_from_registers();
        }
        if((data & 0x80) > 0)
        { // clear
            reg_Shift = 0b1'0000;
            reg_Control |= 0b0'1100;
        }
    }

    if(addr >= 0x6000 && addr <= 0x7FFF && prg_ram_bank_count > 0) // prg (nv)ram
    {
        mapper_addr = 0x2000 * prg_ram_bank_idx + (addr & 0x1FFF);
        memory_kind = prg_ram_kind;
        return true;
    }
    else if(addr >= 0x8000 && addr <= 0xBFFF) // prg rom, switchable or fixed to the first bank
    {
        mapper_addr = 0x4000 * prg_rom_lo_bank_idx + (addr & 0x3FFF);
        memory_kind = MemoryKind::ROM;
        return true;
    }
    else if(addr >= 0xC000 && addr <= 0xFFFF) // prg rom, fixed to the last bank or switchable
    {
        mapper_addr = 0x4000 * prg_rom_hi_bank_idx + (addr & 0x3FFF);
        memory_kind = MemoryKind::ROM;
        return true;
    }
    return false;
}

bool Mapper_001::ppuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x0000 && addr <= 0x0FFF && chr_bank_count > 0) // chr rom, low bank
    {
        mapper_addr = 0x1000 * chr_lo_bank_idx + (addr & 0x0FFF);
        memory_kind = chr_mem_kind;
        return true;
    }
    else if(addr >= 0x1000 && addr <= 0x1FFF && chr_bank_count > 0) // chr rom, high bank
    {
        mapper_addr = 0x1000 * chr_hi_bank_idx + (addr & 0x0FFF);
        memory_kind = chr_mem_kind;
        return true;
    }
    return false;
}
bool Mapper_001::ppuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind)
{
    if(addr >= 0x0000 && addr <= 0x0FFF && chr_bank_count > 0) // chr rom, low bank
    {
        mapper_addr = 0x1000 * chr_lo_bank_idx + (addr & 0x0FFF);
        memory_kind = chr_mem_kind;
        return true;
    }
    else if(addr >= 0x1000 && addr <= 0x1FFF && chr_bank_count > 0) // chr rom, high bank
    {
        mapper_addr = 0x1000 * chr_hi_bank_idx + (addr & 0x0FFF);
        memory_kind = chr_mem_kind;
        return true;
    }
    return false;
}

MirrorMode Mapper_001::mirror() const
{
    return mirror_mode;
}

void Mapper_001::update_values_from_registers()
{
    switch(reg_Control & 0b0'0011) // Nametable arrangement
    {
    case 0x00: mirror_mode = MirrorMode::ONESCREEN_LO; break;
    case 0x01: mirror_mode = MirrorMode::ONESCREEN_HI; break;
    case 0x02: mirror_mode = MirrorMode::VERTICAL;  break;
    case 0x03: mirror_mode = MirrorMode::HORIZONTAL; break;
    }

    switch((reg_Control & 0b0'1100) >> 2) // PRG-ROM bank mode
    {
    case 0x00: [[fallthrough]];
    case 0x01: // switch 32 KB at $8000, ignoring low bit of bank number
    {
        prg_rom_lo_bank_idx = (reg_PRG & 0x0E);
        prg_rom_hi_bank_idx = prg_rom_lo_bank_idx + 1;
        break;
    }
    case 0x02: // fix first bank at $8000 and switch 16 KB bank at $C000
    {
        prg_rom_lo_bank_idx = 0;
        prg_rom_hi_bank_idx = (reg_PRG & 0x0F);
        break;
    }
    case 0x03: //fix last bank at $C000 and switch 16 KB bank at $8000
    {
        prg_rom_lo_bank_idx = (reg_PRG & 0x0F);
        prg_rom_hi_bank_idx = prg_rom_bank_count-1;
        break;
    }
    }

    if((reg_Control & 0b1'0000) > 0) // CHR-ROM bank mode
    { // switch two separate 4 KB banks
        chr_lo_bank_idx = (reg_CHR0 & 0x1F);
        chr_hi_bank_idx = (reg_CHR1 & 0x1F);
    }
    else
    { // switch 8 KB at a time
        chr_lo_bank_idx = (reg_CHR0 & 0x1E);
        chr_hi_bank_idx = chr_lo_bank_idx + 1;
    }
}
