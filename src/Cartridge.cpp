#include "Cartridge.hpp"

#include "Mapper.hpp"
#include "Mapper_000.hpp"
#include "Mapper_001.hpp"

#include <cstdint>
#include <iostream>
#include <fstream>
#include <stdexcept>

Cartridge::Cartridge(std::string const& path)
{
    std::ifstream stream;
    stream.open(path,std::ifstream::binary);

    if(!stream.is_open())
    {
        throw std::runtime_error("Failed to open file " + path);
    }

    uint8_t header[HEADER_SIZE];
    stream.read((char*) header, HEADER_SIZE);

    // check format
    is_iNES_format = header[0] == INES_HEAD[0] && header[1] == INES_HEAD[1] && header[2] == INES_HEAD[2] && header[3] == INES_HEAD[3];
    is_NES20_format = is_iNES_format && (header[7]&0x0C)==0x08;
    // flag 4
    prg_count = header[4];
    // flag 5
    chr_count = header[5];
    // flag 6
    hardwired_nametable_mirroring_type_is_vertical = header[6] & 0b0000'0001;
    has_battery = (header[6] & 0b0000'0010) >> 1;
    has_trainer = (header[6] & 0b0000'0100) >> 2;
    hardwired_four_screen_mode = (header[6] & 0b0000'1000) >> 3;
    mapper_number = (header[6] & 0xF0) >> 4;
    // flag 7
    console_type = header[7] & 0b0000'0011;
    nes_20_identifier = (header[7] & 0b0000'1100) >> 2;
    mapper_number |= (header[7] & 0xF0);
    //flag 8
    mapper_number |= ((header[8] & 0x0F) << 8);
    submapper_number = (header[8] & 0xF0) >> 4;
    // flag 9
    prg_rom_size_msb = header[9] & 0x0F;
    chr_rom_size_msb = (header[9] & 0xF0) >> 4;
    // flag 10
    prg_ram_shift_count = header[10] & 0x0F;
    prg_nvram_eeprom_shift_count = (header[10] & 0xF0) >> 4;
    // flag 11
    chr_ram_shift_count = header[11] & 0x0F;
    chr_nvram_shift_count = (header[11] & 0xF0) >> 4;
    // flag 12
    cpu_ppu_timing_mode = header[12] & 0b0000'0011;
    // flag 13
    vs_ppu_type = header[13] & 0x0F;
    vs_hardware_type = (header[13] & 0xF0) >> 4;
    extended_console_type = vs_ppu_type;
    // flag 14
    number_of_miscellaneous_roms_present = header[14] & 0b0000'0011;
    // flag 15
    default_expansion_device = header[15] & 0b0011'1111;

    // init buffers
    if(prg_rom_size_msb == 0x0F)
    { // Exponent notation
        uint32_t exp = (prg_count & 0xFC) >> 2;
        uint32_t mult = (prg_count & 0x03)*2 + 1;
        prg_rom_size = (2 << exp) * mult;
    }
    else
    {
        uint32_t bc = prg_count | (prg_rom_size_msb << 8);
        prg_rom_size = PRG_SIZE*bc;
    }
    if(chr_rom_size_msb == 0x0F)
    { // Exponent notation
        uint32_t exp = (chr_count & 0xFC) >> 2;
        uint32_t mult = (chr_count & 0x03)*2 + 1;
        prg_rom_size = (2 << exp) * mult;
    }
    else
    {
        uint32_t bc = chr_count | (chr_rom_size_msb << 8);
        chr_rom_size = CHR_SIZE*bc;
    }
    prg_ram_size   = prg_ram_shift_count          == 0 ? 0 : 64 << prg_ram_shift_count;
    chr_ram_size   = chr_ram_shift_count          == 0 ? 0 : 64 << chr_ram_shift_count;
    prg_nvram_size = prg_nvram_eeprom_shift_count == 0 ? 0 : 64 << prg_nvram_eeprom_shift_count;
    chr_nvram_size = chr_nvram_shift_count        == 0 ? 0 : 64 << chr_nvram_shift_count;

    prg_rom = std::make_unique<uint8_t[]>(prg_rom_size);
    chr_rom = std::make_unique<uint8_t[]>(chr_rom_size);

    // load trainer
    if(has_trainer)
    {
        trainer = std::make_unique<uint8_t[]>(TRAINER_SIZE);
        stream.read((char*) trainer.get(), TRAINER_SIZE);
    }
    // load prg-rom
    stream.read((char*) prg_rom.get(), prg_rom_size);
    // load chr-rom
    stream.read((char*) chr_rom.get(), chr_rom_size);

    stream.close();

    std::cout << "INES ? " << is_iNES_format << std::endl;
    std::cout << "NES2.0 ? " << is_NES20_format << std::endl;
    std::cout << "Mapper " << mapper_number << std::endl;
    std::cout << "Submapper " << (int) submapper_number << std::endl;

    switch (mapper_number)
    {
    case 0: mapper = std::make_shared<Mapper_000>(*this); break;
    case 1: mapper = std::make_shared<Mapper_001>(*this); break;

    default: throw std::runtime_error("Unsupported Mapper");
    }

    std::cout << "PRG-ROM " << prg_rom_size << std::endl;
    std::cout << "CHR-ROM " << chr_rom_size << std::endl;
    std::cout << "PRG-RAM " << prg_ram_size << std::endl;
    std::cout << "CHR-RAM " << chr_ram_size << std::endl;
    std::cout << "PRG-NVRAM " << prg_nvram_size << std::endl;
    std::cout << "CHR-NVRAM " << chr_nvram_size << std::endl;

    prg_ram = std::make_unique<uint8_t[]>(prg_ram_size);
    chr_ram = std::make_unique<uint8_t[]>(chr_ram_size);
    prg_nvram = std::make_unique<uint8_t[]>(prg_nvram_size);
    chr_nvram = std::make_unique<uint8_t[]>(chr_nvram_size);
}

Cartridge::~Cartridge()
{

}

bool Cartridge::cpuRead(uint16_t addr, uint8_t& data)
{
    uint32_t mapper_addr = 0;
    MemoryKind memory_kind;
    if(mapper->cpuMapRead(addr, mapper_addr, memory_kind))
    {
        switch(memory_kind)
        {
        case MemoryKind::ROM:   data = prg_rom  [mapper_addr]; break;
        case MemoryKind::RAM:   data = prg_ram  [mapper_addr]; break;
        case MemoryKind::NVRAM: data = prg_nvram[mapper_addr]; break;
        }
        return true;
    }
    return false;
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t& data)
{
    uint32_t mapper_addr = 0;
    MemoryKind memory_kind;
    if(mapper->cpuMapWrite(addr, data, mapper_addr, memory_kind))
    {
        switch(memory_kind)
        {
        case MemoryKind::ROM:   /* READ-ONLY */                break;
        case MemoryKind::RAM:   prg_rom  [mapper_addr] = data; break;
        case MemoryKind::NVRAM: prg_nvram[mapper_addr] = data; break;
        }
        return true;
    }
    return false;
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t& data)
{
    uint32_t mapper_addr = 0;
    MemoryKind memory_kind;
    if(mapper->ppuMapRead(addr, mapper_addr, memory_kind))
    {
        switch(memory_kind)
        {
        case MemoryKind::ROM:   data = chr_rom  [mapper_addr]; break;
        case MemoryKind::RAM:   data = chr_ram  [mapper_addr]; break;
        case MemoryKind::NVRAM: data = chr_nvram[mapper_addr]; break;
        }
        return true;
    }
    return false;
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t& data)
{
    uint32_t mapper_addr = 0;
    MemoryKind memory_kind;
    if(mapper->ppuMapWrite(addr, mapper_addr, memory_kind))
    {
        switch(memory_kind)
        {
        case MemoryKind::ROM:   /* READ-ONLY */                break;
        case MemoryKind::RAM:   chr_ram  [mapper_addr] = data; break;
        case MemoryKind::NVRAM: chr_nvram[mapper_addr] = data; break;
        }
        return true;
    }
    return false;
}

MirrorMode Cartridge::mirror() const
{
    return mapper->mirror();
}
