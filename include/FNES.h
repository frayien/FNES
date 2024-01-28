/**
 * @file FNES.h
 * @author frayien (frayien@yahoo.fr)
 * @brief FNES main class
 * @version 0.1
 * @date 2020-11-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef FNES_FNES
#define FNES_FNES

//#define FNES_DUMP

#include "PixelWindow.h"
#include "PixelCanvas.h"
#include "Bus.h"
#include "Cartridge.h"

#include <sstream>
#include <memory>

class FNES : public PixelWindow
{
public:
    FNES();
    ~FNES();
    
    void draw();

    void drawRam(uint16_t x, uint16_t y, uint16_t page);
    void drawCpu(uint16_t x, uint16_t y);
    void drawCode(uint16_t x, uint16_t y);
    void drawSprite(uint16_t x, uint16_t y, int s, int e);

    virtual void init() override;
    virtual void update() override;

    bool emulation_running = false;
    uint8_t selected_palette = 0x00;

    std::shared_ptr<Cartridge> cart;
    Bus nes;

    std::map<uint16_t, std::string> decompiled;

    PixelCanvas gui;

    #ifdef FNES_DUMP 
    sf::Clock clock;
    #endif // FNES_DUMP

    Tile full_tile 
    {
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
    };

    enum JoystickButton : uint8_t
    {
        A = 0,
        B = 1,
        X = 2,
        Y = 3,
        LB = 4,
        RB = 5,
        Select = 6,
        Start = 7,
        LStick = 8,
        RStick = 9
    };
};

#endif // FNES_FNES
