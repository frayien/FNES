#include "Cartridge.h"

Cartridge::Cartridge(std::string const& path)
{
    std::ifstream stream;
    stream.open(path,std::ifstream::binary);

    if(stream.is_open())
    {
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


        // load trainer
        if(has_trainer)
        {
            trainer = std::make_unique<uint8_t[]>(TRAINER_SIZE);
            stream.read((char*) trainer.get(), TRAINER_SIZE);
        }
        // load prg
        prg = std::make_unique<uint8_t[]>(PRG_SIZE*prg_count);
        stream.read((char*) prg.get(), PRG_SIZE*prg_count);
        // load chr
        chr = std::make_unique<uint8_t[]>(CHR_SIZE*chr_count);
        stream.read((char*) chr.get(), CHR_SIZE*chr_count);

        stream.close();

        std::cout << "INES ? " << is_iNES_format << std::endl;
        std::cout << "NES2.0 ? " << is_NES20_format << std::endl;
        std::cout << "Mapper " << mapper_number << std::endl;
        std::cout << "PRG " << (int) prg_count << std::endl;
        std::cout << "CHR " << (int) chr_count << std::endl;

        switch (mapper_number)
        {
        case 0: mapper = std::make_shared<Mapper_000>(prg_count, chr_count); break;
        
        default:
            break;
        }

        mirror = hardwired_nametable_mirroring_type_is_vertical ? VERTICAL : HORIZONTAL;
    }
    else
    {
        std::cout << "Failed to open file " << path << std::endl;
    }
}

Cartridge::~Cartridge()
{

}

bool Cartridge::cpuRead(uint16_t addr, uint8_t& data)
{
    uint32_t mapper_addr = 0;
    if(mapper->cpuMapRead(addr, mapper_addr))
    {
        data = prg[mapper_addr];
        return true;
    }
    return false;
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t& data)
{
    uint32_t mapper_addr = 0;
    if(mapper->cpuMapWrite(addr, mapper_addr))
    {
        prg[mapper_addr] = data;
        return true;
    }
    return false;
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t& data)
{
    uint32_t mapper_addr = 0;
    if(mapper->ppuMapRead(addr, mapper_addr))
    {
        data = chr[mapper_addr];
        return true;
    }
    return false;
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t& data)
{
    uint32_t mapper_addr = 0;
    if(mapper->ppuMapWrite(addr, mapper_addr))
    {
        chr[mapper_addr] = data;
        return true;
    }
    return false;
}