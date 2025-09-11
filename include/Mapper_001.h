#ifndef FNES_MAPPER_001
#define FNES_MAPPER_001

#include "Mapper.h"

class Mapper_001 : public Mapper
{
public:
    Mapper_001(uint8_t _prg_count, uint8_t _chr_count);
    virtual ~Mapper_001();

    virtual bool cpuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
    virtual bool cpuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;

    virtual bool ppuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
    virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
};

#endif // FNES_MAPPER_001