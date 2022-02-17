#include "TestBus.h"


TestBus::TestBus()
{
    cpu.connectBus(this);
}

TestBus::~TestBus()
{
}

uint8_t TestBus::cpuRead(uint16_t addr, bool read_only)
{
    return cpu_ram[addr];
}

void TestBus::cpuWrite(uint16_t addr, uint8_t data)
{
    cpu_ram[addr] = data;
}