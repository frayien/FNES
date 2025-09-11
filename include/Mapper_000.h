#ifndef FNES_MAPPER_000
#define FNES_MAPPER_000

#include "Mapper.h"

class Mapper_000 : public Mapper
{
public:
    Mapper_000(uint8_t _prg_count, uint8_t _chr_count);
    virtual ~Mapper_000();

    virtual bool cpuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
    virtual bool cpuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;

    virtual bool ppuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
    virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;

private:
    uint16_t cpu_prg_mode_mask;
};

#endif // FNES_MAPPER_000