/**
 * @file Bus.h
 * @author frayien (frayien@yahoo.fr)
 * @brief Bus emulation for the FNES Emulator
 * @version 0.1
 * @date 2020-10-31
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include <cstdint>
#include <array>
#include <memory>

#include "IBus.h"
#include "Cpu6502.h"
#include "Ppu2C02.h"
#include "Cartridge.h"
#include "Apu2A03.h"

union Controller
{
    uint8_t byte;
    struct
    {
        uint8_t A : 1;
        uint8_t B : 1;
        uint8_t select : 1;
        uint8_t start : 1;
        uint8_t up : 1;
        uint8_t down : 1;
        uint8_t left : 1;
        uint8_t right : 1;
    };
};

class Bus : public IBus
{
public:
    Bus();
    virtual ~Bus();

    // Devices on bus
    Cpu6502 cpu;
    Ppu2C02 ppu;
    Apu2A03 apu;

    std::array<uint8_t, 2048> cpu_ram = { 0 };

    std::shared_ptr<Cartridge> cart;

    Controller controller[2] = { { 0 }, { 0 } };
    
    // Bus read and write
    virtual uint8_t cpuRead(uint16_t addr, bool read_only = false) override;
    virtual void cpuWrite(uint16_t addr, uint8_t data) override;

    // System Interface
    void insertCartridge(std::shared_ptr<Cartridge> const& cartridge);
    void reset();
    void clock();

private:
    uint32_t system_clock_counter = 0;

    Controller controller_state[2] = { { 0 }, { 0 } };

    uint8_t dma_page = 0x00;
    uint8_t dma_addr = 0x00;
    uint8_t dma_data = 0x00;

    bool dma_transfer = false;
    bool dma_dummy = true;
};
