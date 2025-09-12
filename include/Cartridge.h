/**
 * @file Cartridge.h
 * @author frayien (frayien@yahoo.fr)
 * @brief Cartridge
 * @version 0.1
 * @date 2020-11-03
 *
 * @copyright Copyright (c) 2020
 *
 * cf http://wiki.nesdev.com/w/index.php/NES_2.0
 */

#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "Mapper.h"

class Cartridge
{
public:
    Cartridge(std::string const& path);
    ~Cartridge();

    bool cpuRead(uint16_t addr, uint8_t& data);
    bool cpuWrite(uint16_t addr, uint8_t& data);

    bool ppuRead(uint16_t addr, uint8_t& data);
    bool ppuWrite(uint16_t addr, uint8_t& data);


    std::shared_ptr<Mapper> mapper;

    std::unique_ptr<uint8_t[]> trainer;
    std::unique_ptr<uint8_t[]> prg_rom;
    std::unique_ptr<uint8_t[]> prg_ram;
    std::unique_ptr<uint8_t[]> prg_nvram;
    std::unique_ptr<uint8_t[]> chr_rom;
    std::unique_ptr<uint8_t[]> chr_ram;
    std::unique_ptr<uint8_t[]> chr_nvram;

    uint32_t prg_rom_size;
    uint32_t prg_ram_size;
    uint32_t prg_nvram_size;
    uint32_t chr_rom_size;
    uint32_t chr_ram_size;
    uint32_t chr_nvram_size;

    enum MIRROR
    {
        HORIZONTAL,
        VERTICAL,
        ONESCREEN_LO,
        ONESCREEN_HI
    } mirror = HORIZONTAL;

    // const //

    static constexpr uint16_t HEADER_SIZE = 0x0010;
    static constexpr uint16_t TRAINER_SIZE = 0x0200;
    static constexpr uint16_t PRG_SIZE = 0x4000;
    static constexpr uint16_t CHR_SIZE = 0x2000;

    static constexpr uint8_t INES_HEAD[4] = { 'N','E','S',0x1A };

    static constexpr uint8_t CONSOLE_TYPE_NES_FAMICOM = 0;
    /// cf : https://wiki.nesdev.com/w/index.php/Vs._System
    static constexpr uint8_t CONSOLE_TYPE_VS_SYSTEM = 1;
    /// cf : https://wiki.nesdev.com/w/index.php/PC10_ROM-Images
    static constexpr uint8_t CONSOLE_TYPE_PLAYCHOICE_10 = 2;
    /// cf : https://wiki.nesdev.com/w/index.php/NES_2.0#Extended_Console_Type
    static constexpr uint8_t CONSOLE_TYPE_EXTENDED = 3;

    /// cf : https://wiki.nesdev.com/w/index.php/NES_2.0#CPU.2FPPU_Timing
    static constexpr uint8_t CPU_PPU_TIMING_MODE_RP2C02 = 0; /// NTSC NES
    static constexpr uint8_t CPU_PPU_TIMING_MODE_RP2C07 = 1; /// Licensed PAL NES
    static constexpr uint8_t CPU_PPU_TIMING_MODE_MULTIPLE_REGION = 2;
    static constexpr uint8_t CPU_PPU_TIMING_MODE_UMC_6527P = 3; /// Dendy

    // header //

    /// HEADER : cf : https://wiki.nesdev.com/w/index.php/NES_2.0

    // format flag
    bool is_iNES_format = false;
    bool is_NES20_format = false;

    /// cf : https://wiki.nesdev.com/w/index.php/Mapper
    uint16_t mapper_number = 0x000;
    // flag 4
    uint8_t prg_count = 0;
    // flag 5
    uint8_t chr_count = 0;
    // flag 6
    /// false = Horizontal or mapper-controlled
    /// true = Vertical
    /// cf : https://wiki.nesdev.com/w/index.php/NES_2.0#Hard-Wired_Mirroring
    bool hardwired_nametable_mirroring_type_is_vertical = false;
    bool has_battery = false;
    /// cf : https://wiki.nesdev.com/w/index.php/NES_2.0#Trainer_Area
    bool has_trainer = false;
    ///cf : https://wiki.nesdev.com/w/index.php/NES_2.0#Hard-Wired_Mirroring
    bool hardwired_four_screen_mode = false;
    // flag 7
    uint8_t console_type = 0b00;
    uint8_t nes_20_identifier = 0b00;
    // flag 8
    /// cf : https://wiki.nesdev.com/w/index.php/NES_2.0_submappers
    uint8_t submapper_number = 0x0;
    // flag 9
    /// cf : https://wiki.nesdev.com/w/index.php/NES_2.0#PRG-ROM_Area
    uint8_t prg_rom_size_msb = 0x0;
    /// cf : https://wiki.nesdev.com/w/index.php/NES_2.0#CHR-ROM_Area
    uint8_t chr_rom_size_msb = 0x0;
    // flag 10
    /// cf : https://wiki.nesdev.com/w/index.php/NES_2.0#PRG-.28NV.29RAM.2FEEPROM
    uint8_t prg_ram_shift_count = 0x0;
    uint8_t prg_nvram_eeprom_shift_count = 0x0;
    /// If the shift count is zero, there is no PRG-(NV)RAM.
    /// If the shift count is non-zero, the actual size is
    /// "64 << shift count" bytes, i.e. 8192 bytes for a shift count of 7.
    // flag 11
    /// cf : https://wiki.nesdev.com/w/index.php/NES_2.0#CHR-.28NV.29RAM
    uint8_t chr_ram_shift_count = 0x0;
    uint8_t chr_nvram_shift_count = 0x0;
    /// If the shift count is zero, there is no CHR-(NV)RAM.
    /// If the shift count is non-zero, the actual size is
    /// "64 << shift count" bytes, i.e. 8192 bytes for a shift count of 7
    // flag 12
    /// cf : https://wiki.nesdev.com/w/index.php/NES_2.0#CPU.2FPPU_Timing
    uint8_t cpu_ppu_timing_mode = 0b00;
    // flag 13
    uint8_t vs_ppu_type = 0x0;
    uint8_t vs_hardware_type = 0x0;
    uint8_t extended_console_type = 0x0;
    // flag 14
    /// cf : https://wiki.nesdev.com/w/index.php/NES_2.0#Miscellaneous_ROM_Area
    uint8_t number_of_miscellaneous_roms_present = 0b00;
    // flag 15
    /// cf : https://wiki.nesdev.com/w/index.php/NES_2.0#Default_Expansion_Device
    uint8_t default_expansion_device = 0b00'0000;
};
