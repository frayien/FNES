#ifndef FNES_MAPPER
#define FNES_MAPPER

#include <cstdint>

// forward
class Cartridge;

enum class MemoryKind
{
    ROM,
    RAM,
    NVRAM
};

class Mapper
{
public:
    Mapper() = default;
    virtual ~Mapper() = default;

    virtual bool cpuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) = 0;
    virtual bool cpuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) = 0;

    virtual bool ppuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) = 0;
    virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) = 0;
};

#endif // FNES_MAPPER
