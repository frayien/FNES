#ifndef FNES_MAPPER_001
#define FNES_MAPPER_001

#include "Mapper.h"

class Mapper_001 : public Mapper
{
public:
    Mapper_001(const Cartridge & cartridge);
    virtual ~Mapper_001() = default;

    virtual bool cpuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
    virtual bool cpuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;

    virtual bool ppuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
    virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
};

#endif // FNES_MAPPER_001