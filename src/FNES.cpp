#include "FNES.hpp"

#include <sstream>
#include <iostream>

FNES::FNES() :
    PixelWindow(256*3+128*2*2, 720, "FNES"),
    gui(400, 480)
{
    setBackgroundColor(sf::Color(0x45467fFF));
    PixelCanvas::initFont();

    cart = std::make_shared<Cartridge>("resources/holydiverbatman/testroms/M1_P128K.nes");
    // cart = std::make_shared<Cartridge>("resources/holydiverbatman/testroms/M1_P128K_C32K_S8K.nes");
    //cart = std::make_shared<Cartridge>("resources/instr_misc.nes");
    //cart = std::make_shared<Cartridge>("resources/instr_timing.nes");
    //cart = std::make_shared<Cartridge>("resources/apu_mixer/square.nes");
    //cart = std::make_shared<Cartridge>("resources/full_palette/full_palette_alt.nes");
    //cart = std::make_shared<Cartridge>("resources/instr_test-v5/official_only.nes");
    //cart = std::make_shared<Cartridge>("resources/nestest.nes");
    //cart = std::make_shared<Cartridge>("resources/color_test.nes");
    //cart = std::make_shared<Cartridge>("resources/AccuracyCoin.nes");
    //cart = std::make_shared<Cartridge>("resources/Super Mario Bros. (World).nes");
    //cart = std::make_shared<Cartridge>("resources/Donkey Kong (JU) [p1].nes");
    //cart = std::make_shared<Cartridge>("resources/Ice Climber (USA, Europe).nes");
    //cart = std::make_shared<Cartridge>("resources/Duck Tales (USA).nes");


    nes.insertCartridge(cart);

    decompiled = nes.decompile(0x0000, 0xFFFF);

    nes.cpu.reset();
}

FNES::~FNES()
{
}

void FNES::update()
{
    #ifdef FNES_DUMP
    int64_t clock_wait = clock.restart().asMicroseconds();
    #endif // FNES_DUMP

    if(wasPressed(sf::Keyboard::C) && !emulation_running)
    {
        do { nes.clock(); } while(!nes.cpu.completed());
        do { nes.clock(); } while(nes.cpu.completed());
    }

    if(wasPressed(sf::Keyboard::F) || emulation_running)
    {
        do { nes.clock(); } while(!nes.ppu.frame_complete);
        do { nes.clock(); } while(nes.cpu.completed());

        nes.ppu.frame_complete = false;
    }

    if(wasPressed(sf::Keyboard::Space))
        emulation_running = !emulation_running;

    if(wasPressed(sf::Keyboard::P))
        (++selected_palette) &= 0x07;

    if(wasPressed(sf::Keyboard::R))
        nes.cpu.reset();

    if(wasPressed(sf::Keyboard::I))
        nes.cpu.irq();

    if(wasPressed(sf::Keyboard::N))
        nes.cpu.nmi();

    // controller 1
    // keyboard
    nes.controller[0].A = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    nes.controller[0].B = sf::Keyboard::isKeyPressed(sf::Keyboard::B);
    nes.controller[0].select = sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4);
    nes.controller[0].start = sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5);
    nes.controller[0].up = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    nes.controller[0].down = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    nes.controller[0].left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    nes.controller[0].right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    // joystick
    if (sf::Joystick::isConnected(0))
    {
        nes.controller[0].A = sf::Joystick::isButtonPressed(0,JoystickButton::A) || sf::Joystick::isButtonPressed(0,JoystickButton::B);
        nes.controller[0].B = sf::Joystick::isButtonPressed(0,JoystickButton::X) || sf::Joystick::isButtonPressed(0,JoystickButton::Y);
        nes.controller[0].select = sf::Joystick::isButtonPressed(0,JoystickButton::Select);
        nes.controller[0].start = sf::Joystick::isButtonPressed(0,JoystickButton::Start);

        float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
        float y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);

        x += sf::Joystick::getAxisPosition(0, sf::Joystick::PovX);
        y += -sf::Joystick::getAxisPosition(0, sf::Joystick::PovY);

        nes.controller[0].up    = y < -50;
        nes.controller[0].down  = y >  50;
        nes.controller[0].left  = x < -50;
        nes.controller[0].right = x >  50;
    }

    #ifdef FNES_DUMP
    int64_t clock_clock = clock.restart().asMicroseconds();
    #endif // FNES_DUMP

    draw();

    #ifdef FNES_DUMP
    std::cout << "w " << clock_wait << " c " << clock_clock << " d " << clock.restart().asMicroseconds() << std::endl;
    #endif // FNES_DUMP
}

void FNES::draw()
{
    gui.clear(0x00000000);

    // Palette
    for(int pal = 0; pal<8; ++pal)
    {   for(int pix = 0; pix<4; ++pix)
        { gui.drawSprite(2 + pal*(5*8) + pix*8,2,full_tile, nes.ppu.getColorFromPaletteRam(pal,pix)); }}

    // Palette selection
    for(int pix = 0; pix<(8*4); ++pix)
    {   gui.setPixel(2+selected_palette*(8*5)+pix, 3+8, 0x0000FFFF); }

    // CPU state
    drawCpu(10, 4 + 8+2);
    // Sprite map
    // drawSprite(2       , 4+10*7 +8+2, 0 , 22);
    // drawSprite(4 + 16*8, 4+10*7 +8+2+12, 22, 43);
    // drawSprite(6 + 32*8, 4+10*7 +8+2+12, 43, 64);
    // Code at PC
    drawCode(10, 4+10*7 +8+2);
    gui.drawString(10, 362, "Space = Run\nC = Cpu Instruction\nF = Full Frame\nP = select palette\nR = RESET\nI = IRQ\nN = NMI", 0, 2);
    drawCanvas(gui, 256*3, 128*2, 1.3);

    // PatterTable 0 & 1
    drawCanvas(nes.ppu.getPatternTable(0,selected_palette), 256*3, 0, 2);
    drawCanvas(nes.ppu.getPatternTable(1,selected_palette), 256*4, 0, 2);
    // Game
    drawCanvas(nes.ppu.getScreen(), 0, 0, 3);
}

void FNES::drawRam(uint16_t x, uint16_t y, uint16_t page)
{
    std::stringstream str;
    for(uint16_t j = 0; j<16; ++j)
    {
        str << '$' << hex(page, 4) << ": ";
        for(uint16_t i = 0; i<16; ++i)
            str << hex(nes.cpuRead(page++), 2) << ' ';
        str << '\n';
    }
    gui.drawString(x, y, str.str(), 0, 2);
}

void FNES::drawCpu(uint16_t x, uint16_t y)
{
    gui.drawString(x, y, "STATUS: ");
    gui.drawString(x + 8 *8, y, std::string(nes.cpu.flag.N ? "\\g" : "\\r") + "N ");
    gui.drawString(x + 10*8, y, std::string(nes.cpu.flag.V ? "\\g" : "\\r") + "V ");
    gui.drawString(x + 12*8, y, std::string(nes.cpu.flag.U ? "\\g" : "\\r") + "U ");
    gui.drawString(x + 14*8, y, std::string(nes.cpu.flag.B ? "\\g" : "\\r") + "B ");
    gui.drawString(x + 16*8, y, std::string(nes.cpu.flag.D ? "\\g" : "\\r") + "D ");
    gui.drawString(x + 18*8, y, std::string(nes.cpu.flag.I ? "\\g" : "\\r") + "I ");
    gui.drawString(x + 20*8, y, std::string(nes.cpu.flag.Z ? "\\g" : "\\r") + "Z ");
    gui.drawString(x + 22*8, y, std::string(nes.cpu.flag.C ? "\\g" : "\\r") + "C" );

    std::stringstream str;
    str << "PC: $" << hex(nes.cpu.reg_PC, 4) << "\n";
    str << "A: $"  << hex(nes.cpu.reg_A , 2) << " [" << (int) nes.cpu.reg_A << "]\n";
    str << "X: $"  << hex(nes.cpu.reg_X , 2) << " [" << (int) nes.cpu.reg_X << "]\n";
    str << "Y: $"  << hex(nes.cpu.reg_Y , 2) << " [" << (int) nes.cpu.reg_Y << "]\n";
    str << "SP: $" << hex(nes.cpu.reg_SP, 4) << "\n";
    gui.drawString(x, y+10, str.str(), 0, 2);
}
void FNES::drawCode(uint16_t x, uint16_t y)
{
    uint16_t addr = nes.cpu.reg_PC;

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
void FNES::drawSprite(uint16_t x, uint16_t y, int s, int e)
{
    for(int i = s; i<e; ++i)
        gui.drawString(x, y + (i-s)*12, 
          std::string(nes.ppu.OAM[i].id > 0x00 && nes.ppu.OAM[i].y < 0xEF ? "\\g" : "\\r") + std::string(i<10 ? " " : "") + std::to_string(i) + "\\w "
        + "" + hex(nes.ppu.OAM[i].id, 2) + " " 
        + hex(nes.ppu.OAM[i].x, 2) + "," 
        + hex(nes.ppu.OAM[i].y, 2) 
        + " " + hex(nes.ppu.OAM[i].attribute & 0x03, 1)
        + std::string((nes.ppu.OAM[i].attribute & 0x20) > 0 ? "\\g" : "\\r") + "P\\w"
        + std::string((nes.ppu.OAM[i].attribute & 0x40) > 0 ? "\\g" : "\\r") + "H\\w"
        + std::string((nes.ppu.OAM[i].attribute & 0x80) > 0 ? "\\g" : "\\r") + "V\\w"
        );

}