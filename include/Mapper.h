#ifndef FNES_MAPPER
#define FNES_MAPPER

#include <cstdint>

enum class MemoryKind
{
    ROM,
    RAM,
    NVRAM
};

class Mapper
{
public:
    Mapper(uint8_t prg_count_, uint8_t chr_count_);
    virtual ~Mapper();

    virtual bool cpuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) = 0;
    virtual bool cpuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) = 0;

    virtual bool ppuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) = 0;
    virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) = 0;

protected:
    uint8_t prg_count;
    uint8_t chr_count;
};

#endif // FNES_MAPPER
