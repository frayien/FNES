#ifndef FNES_MAPPER
#define FNES_MAPPER

#include <cstdint>

class Mapper
{
public:
    Mapper(uint8_t _prg_count, uint8_t _chr_count);
    virtual ~Mapper();

    virtual bool cpuMapRead(uint16_t addr, uint32_t& mapper_addr) = 0;
    virtual bool cpuMapWrite(uint16_t addr, uint32_t& mapper_addr) = 0;

    virtual bool ppuMapRead(uint16_t addr, uint32_t& mapper_addr) = 0;
    virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapper_addr) = 0;

protected:
    uint8_t prg_count;
    uint8_t chr_count;
};

#endif // FNES_MAPPER
