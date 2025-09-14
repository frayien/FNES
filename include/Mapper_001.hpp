#pragma once

#include "Mapper.hpp"

#include <cstdint>

class Mapper_001 : public Mapper
{
public:
    Mapper_001(const Cartridge & cartridge);
    virtual ~Mapper_001() = default;

    virtual bool cpuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
    virtual bool cpuMapWrite(uint16_t addr, uint8_t data, uint32_t& mapper_addr, MemoryKind& memory_kind) override;

    virtual bool ppuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
    virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;

    virtual MirrorMode mirror() const override;

private:
    void update_values_from_registers();

private:
    MirrorMode mirror_mode;

    MemoryKind prg_ram_kind;
    MemoryKind chr_mem_kind;

    uint32_t prg_ram_bank_count;
    uint32_t prg_rom_bank_count;
    uint32_t chr_bank_count;

    uint32_t prg_ram_bank_idx;

    uint32_t prg_rom_lo_bank_idx;
    uint32_t prg_rom_hi_bank_idx;

    uint32_t chr_lo_bank_idx;
    uint32_t chr_hi_bank_idx;


    enum class Variant
    {
        DEFAULT,
        // SxROM Variants
        SEROM_SHROM_SH1ROM, ///< do not connect PRG A14 to the MMC1; instead A14 is connected directly to the NES. As a result, register $E000 is completely without function, and the entire 32 KiB of PRG-ROM appear unbanked from CPU $8000-$FFFF. For compatibility with these, an emulator may switch to PRG bank 0 at power-on
        SNROM, ///< only supports 8 KiB of CHR-ROM/-RAM, uses the upper CHR bank select line coming out of the mapper (CHR A16; bit 4 of bank number) as an additional chip enable for the PRG-RAM.
        SOROM_SUROM_SXROM, ///< address only 8 KiB of CHR-ROM/-RAM ; repurpose the upper CHR lines to increase the PRG-RAM size from 8 to 16 KiB or 32 KiB ; and to increase the addressable PRG-ROM size from 256 to 512 KiB
        SZROM, ///< addresses only 64 KiB of CHR-ROM/-RAM and uses the remaining CHR line to increase the PRG-RAM size from 8 to 16 KiB.

        FCNS_2ME, ///< features 64 big-endian 16-bit words of EEPROM in addition to up to 32 KB of battery-backed PRG-RAM
        KS_7058
    };

    Variant variant;

private:
    uint8_t reg_Shift;
    uint8_t reg_Control;
    uint8_t reg_CHR0;
    uint8_t reg_CHR1;
    uint8_t reg_PRG;
};
