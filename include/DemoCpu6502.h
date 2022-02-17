/**
 * @file DemoCpu6502.h
 * @author frayien (frayien@yahoo.fr)
 * @brief Test and Demo for the cpu 6502
 * @version 0.1
 * @date 2020-11-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef FNES_DEMOCPU6502
#define FNES_DEMOCPU6502

#include "PixelWindow.h"
#include "PixelCanvas.h"
#include "TestBus.h"

#include <string>
#include <array>
#include <sstream>
#include <fstream>
#include <iostream>

class DemoCpu6502 : public PixelWindow
{
public:
    DemoCpu6502();
    virtual ~DemoCpu6502();

    void draw();

    void drawRam(uint16_t x, uint16_t y, uint16_t page);
    void drawCpu(uint16_t x, uint16_t y);
    void drawCode(uint16_t x, uint16_t y);

    virtual void update() override;

    void step();

    TestBus bus;

    std::map<uint16_t, std::string> decompiled;

    PixelCanvas gui;

    bool running = false;
    uint16_t prev_PC = 0x0000;
    uint32_t inst_nb = 0;

    int buffer_pos = 0;
    uint16_t buffer_PC = 0;
    std::array<std::string, 10> buffer;
};

#endif // !FNES_DEMOCPU6502
