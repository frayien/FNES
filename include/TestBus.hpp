#pragma once

#include "IBus.hpp"
#include "Cpu6502.hpp"

#include <cstdint>
#include <array>

class TestBus : public IBus
{
public:
    TestBus();
    virtual ~TestBus();

    Cpu6502 cpu;

    std::array<uint8_t, 0x10000> cpu_ram = { 0 };

    // Bus read and write
    virtual uint8_t cpuRead(uint16_t addr, bool read_only = false) override;
    virtual void cpuWrite(uint16_t addr, uint8_t data) override;
};
