#pragma once

#include "Mapper.h"

class Mapper_000 : public Mapper
{
public:
    Mapper_000(const Cartridge & cartridge);
    virtual ~Mapper_000() = default;

    virtual bool cpuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
    virtual bool cpuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;

    virtual bool ppuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
    virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;

private:
    uint16_t cpu_prg_rom_mask;
    uint16_t cpu_prg_ram_mask;
};
