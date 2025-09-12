#include "DemoCpu6502.h"

#include <sstream>
#include <fstream>
#include <iostream>

DemoCpu6502::DemoCpu6502() : 
    PixelWindow(680*1.5, 480*1.5, "FNES"),
    gui(680,480)
{
    setBackgroundColor(sf::Color(0x45467fFF));
    PixelCanvas::initFont();
    /*
    std::stringstream prg;
    prg << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";

    uint16_t offset = 0x8000;
    while(!prg.eof())
    {
        std::string opcode;
        prg >> opcode;
        bus.cpuWrite(offset++, (uint8_t)std::stoul(opcode,nullptr,16));
    }

    bus.cpuWrite(0xFFFC, 0x00);
    bus.cpuWrite(0xFFFD, 0x80);
    */

    // load test prg
    std::ifstream stream;
    stream.open("resources/6502_functional_test.bin",std::ifstream::binary);
    if(stream.is_open())
    {
        uint8_t byte = 0x00;

        uint16_t offset = 0x000A;
        while(!stream.eof())
        {
            stream.read((char*) &byte, 1);
            bus.cpuWrite(offset++, byte);
        }
    }
    

    bus.cpu.reset();
    bus.cpu.reg_PC = 0x0400;

    decompiled = bus.decompile(0x0000, 0xFFFF);
}
 
DemoCpu6502::~DemoCpu6502()
{
}

void DemoCpu6502::step()
{
    do
    {
        bus.cpu.clock();
    } while (bus.cpu.cycles > 0);

    uint16_t addr = bus.cpu.reg_PC;

    if(addr == prev_PC)
        return;

    prev_PC = addr;

    auto it = decompiled.find(addr);
    if(it == decompiled.end()) return;
    
    buffer[buffer_pos] = it->second;
    buffer_pos = (buffer_pos + 1) % 10;
    inst_nb++;
}

void DemoCpu6502::update()
{
    if(wasPressed(sf::Keyboard::S) && !running)
    {
        step();
    }

    if(wasPressed(sf::Keyboard::Space))
        running = !running;

    if(running)
    {
        for(int i = 0; i<100000; i++) step();
    }

    if(buffer_PC == bus.cpu.reg_PC && running) 
    {
        for(int i = 0; i<10; ++i)
            std::cout << buffer[(buffer_pos + i) % 10] << std::endl;
        running = false;
    }
    buffer_PC = bus.cpu.reg_PC;
    
    if(wasPressed(sf::Keyboard::R))
        bus.cpu.reset();

    if(wasPressed(sf::Keyboard::I))
        bus.cpu.irq();

    if(wasPressed(sf::Keyboard::N))
        bus.cpu.nmi();

    draw();
}

void DemoCpu6502::draw()
{
    gui.clear(0x00000000);

    drawRam(2, 2, 0x0000);
    drawRam(2, 2+17*10, 0xFF00);

    drawCpu(4+16*3*8+7*8, 2);

    drawCode(4+16*3*8+7*8, 2+10*7);

    gui.drawString(12,362, "Space = Run   S = Step Instruction   R = RESET   I = IRQ   N = NMI");
    gui.drawString(12,382, std::to_string(inst_nb));

    drawCanvas(gui, 0,0, 1.5);
}

void DemoCpu6502::drawRam(uint16_t x, uint16_t y, uint16_t page)
{
    std::stringstream str;
    for(uint16_t j = 0; j<16; ++j)
    {
        str << '$' << hex(page,4) << ": ";
        for(uint16_t i = 0; i<16; ++i)
            str << hex(bus.cpuRead(page++),2) << ' ';
        str << '\n';
    }
    gui.drawString(x,y,str.str(), 0,2);
}

void DemoCpu6502::drawCpu(uint16_t x, uint16_t y)
{
    gui.drawString(x,y,"STATUS: ");
    gui.drawString(x + 8 *8, y, std::string(bus.cpu.flag.N ? "\\g" : "\\r") + "N ");
    gui.drawString(x + 10*8, y, std::string(bus.cpu.flag.V ? "\\g" : "\\r") + "V ");
    gui.drawString(x + 12*8, y, std::string(bus.cpu.flag.U ? "\\g" : "\\r") + "U ");
    gui.drawString(x + 14*8, y, std::string(bus.cpu.flag.B ? "\\g" : "\\r") + "B ");
    gui.drawString(x + 16*8, y, std::string(bus.cpu.flag.D ? "\\g" : "\\r") + "D ");
    gui.drawString(x + 18*8, y, std::string(bus.cpu.flag.I ? "\\g" : "\\r") + "I ");
    gui.drawString(x + 20*8, y, std::string(bus.cpu.flag.Z ? "\\g" : "\\r") + "Z ");
    gui.drawString(x + 22*8, y, std::string(bus.cpu.flag.C ? "\\g" : "\\r") + "C" );

    std::stringstream str;
    str << "PC: $" << hex(bus.cpu.reg_PC,4) << "\n";
    str << "A: $"  << hex(bus.cpu.reg_A ,2) << " [" << (int) bus.cpu.reg_A << "]\n";
    str << "X: $"  << hex(bus.cpu.reg_X ,2) << " [" << (int) bus.cpu.reg_X << "]\n";
    str << "Y: $"  << hex(bus.cpu.reg_Y ,2) << " [" << (int) bus.cpu.reg_Y << "]\n";
    str << "SP: $" << hex(bus.cpu.reg_SP,4) << "\n";
    gui.drawString(x,y+10,str.str(),0,2);
}
void DemoCpu6502::drawCode(uint16_t x, uint16_t y)
{
    uint16_t addr = bus.cpu.reg_PC;

    auto it = decompiled.find(addr);
    if(it == decompiled.end()) return;

    // draw before
    it--;
    for(int i = 11; i>=0 && it != decompiled.begin(); --i, --it)
        gui.drawString(x, y + i*10, it->second);

    // draw pc
    it = decompiled.find(addr);
    gui.drawString(x, y + 12*10, "\\c" + it->second);

    // draw after
    it++;
    for(int i = 13; i<26 && it != decompiled.end(); ++i, ++it)
        gui.drawString(x, y + i*10, it->second);
}