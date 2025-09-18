#include "Bus.hpp"

Bus::Bus()
{
    cpu.connectBus(this);
    apu.connectBus(this);
}

Bus::~Bus()
{

}

uint8_t Bus::cpuRead(uint16_t addr, bool read_only)
{
    uint8_t data = 0x00;
    if(cart->cpuRead(addr, data))
    {
        // cart handled
    }
    else if(addr >= 0x0000 && addr <= 0x1FFF)
    {
        data = cpu_ram[addr & 0x07FF];
    }
    else if(addr >= 0x2000 && addr <= 0x3FFF)
    {
        data = ppu.cpuRead(addr & 0x0007, read_only);
    }
    else if ((addr >= 0x4000 && addr <= 0x4013) || addr == 0x4015 || addr == 0x4017)
    {
        data = apu.cpuRead(addr);
    }
    else if (addr >= 0x4016 && addr <= 0x4017) // controllers
    {
        data = (controller_state[addr & 0x0001].byte & 0x01) > 0;
        controller_state[addr & 0x0001].byte >>= 1;
    }

    return data;
}

void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
    if(cart->cpuWrite(addr, data))
    {
       // cart handled
    }
    else if(addr >= 0x0000 && addr <= 0x1FFF)
    {
        cpu_ram[addr & 0x07FF] = data;
    }
    else if(addr >= 0x2000 && addr <= 0x3FFF)
    {
        ppu.cpuWrite(addr & 0x0007, data);
    }
    else if ((addr >= 0x4000 && addr <= 0x4013) || addr == 0x4015 || addr == 0x4017)
    {
        apu.cpuWrite(addr, data);
    }
    else if (addr == 0x4014) // Direct Memory Access
    {
        dma_page = data;
        dma_addr = 0x00;
        dma_transfer = true;
    }
    else if (addr >= 0x4016 && addr <= 0x4017) // controllers
    {
        controller_state[addr & 0x0001] = controller[addr & 0x0001];
    }
}

void Bus::insertCartridge(std::shared_ptr<Cartridge> const& cartridge)
{
    cart = cartridge;
    ppu.connectCartridge(cartridge);
}

void Bus::reset()
{
    cpu.reset();
    system_clock_counter = 0;
}

void Bus::clock()
{
    if(system_clock_counter % 3 == 0)
    {
        if(dma_transfer)
        {
            if(dma_dummy)
            {
                if(system_clock_counter % 2 == 1) // wait for sync
                {
                    dma_dummy = false;
                }
            }
            else
            {
                if(system_clock_counter % 2 == 0) // read from cpu data
                {
                    dma_data = cpuRead(dma_page << 8 | dma_addr);
                }
                else // write to ppu
                {
                    ppu.OAM_ptr[dma_addr] = dma_data;
                    dma_addr++;

                    if(dma_addr == 0x00)
                    {
                        dma_transfer = false;
                        dma_dummy = true;
                    }
                }
            }
        }
        else
        {
            cpu.clock();

            if(cpu.completed())
            {
                if(apu.irq || apu.dmc.irq)
                {
                    cpu.irq();
                }

                if(ppu.nmi)
                {
                    ppu.nmi = false;
                    cpu.nmi();
                }
            }
        }
    }

    ppu.clock();

    apu.clock();

    system_clock_counter++;
}