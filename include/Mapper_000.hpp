#pragma once

#include "Mapper.hpp"

class Mapper_000 : public Mapper
{
public:
    Mapper_000(const Cartridge & cartridge);
    virtual ~Mapper_000() = default;

    virtual bool cpuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
    virtual bool cpuMapWrite(uint16_t addr, uint8_t data, uint32_t& mapper_addr, MemoryKind& memory_kind) override;

    virtual bool ppuMapRead(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;
    virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapper_addr, MemoryKind& memory_kind) override;

    virtual MirrorMode mirror() const override;

private:
    MirrorMode mirror_mode;

    uint16_t cpu_prg_rom_mask;
    uint16_t cpu_prg_ram_mask;

    bool has_chr_rom;
};
