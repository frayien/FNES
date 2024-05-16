/**
 * @file Ppu2C02.h
 * @author frayien (frayien@yahoo.fr)
 * @brief PPU
 * @version 0.1
 * @date 2020-11-03
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef FNES_PPU2C02
#define FNES_PPU2C02

#include <cstdint>
#include <memory>
#include <cstring>


#include "PixelCanvas.h"

#include "Cartridge.h"

class Ppu2C02
{
public:
    Ppu2C02();
    virtual ~Ppu2C02();

    uint8_t cpuRead(uint16_t addr, bool read_only = false);
    void cpuWrite(uint16_t addr, uint8_t data);

    uint8_t ppuRead(uint16_t addr, bool read_only = false);
    void ppuWrite(uint16_t addr, uint8_t data);

    void connectCartridge(std::shared_ptr<Cartridge> const& cartridge);
    void clock();

    uint32_t getColorFromPaletteRam(uint8_t palette, uint8_t pixel);

    bool nmi = false;

    // debug
    PixelCanvas& getScreen();
    PixelCanvas& getNametable(uint8_t i);
    PixelCanvas& getPatternTable(uint8_t i, uint8_t palette);

    bool frame_complete = false;

    // registers
    union
    {
        struct
        {
            uint8_t nametable_x : 1;
            uint8_t nametable_y : 1;
            uint8_t increment_mode : 1;
            uint8_t pattern_sprite : 1;
            uint8_t pattern_background : 1;
            uint8_t sprite_size : 1;
            uint8_t slave_mode : 1;
            uint8_t enable_nmi : 1;
        };
        uint8_t reg = 0x00;
    } ppuctrl;

    union
    {
        struct
        {
            uint8_t greyscale : 1;
            uint8_t render_background_left : 1;
            uint8_t render_sprites_left : 1;
            uint8_t render_background : 1;
            uint8_t render_sprites : 1;
            uint8_t enhance_red : 1;
            uint8_t enhance_green : 1;
            uint8_t enhance_blue : 1;
        };
        uint8_t reg = 0x00;
    } ppumask;

    union
    {
        struct
        {
            uint8_t unused : 5;
            uint8_t sprite_overflow : 1;
            uint8_t sprite_zero_hit : 1;
            uint8_t vertical_blank : 1;
        };
        uint8_t reg = 0x00;
    } ppustatus;

    uint8_t address_latch = 0x00;
    uint8_t ppu_data_buffer = 0x00;
    //uint16_t ppu_address = 0x0000;


    // OAMADDR
    // OAMDATA
    union loopy_register
    {
        // Credit to Loopy
        struct
        {
            uint16_t coarse_x : 5;
            uint16_t coarse_y : 5;
            uint16_t nametable_x : 1;
            uint16_t nametable_y : 1;
            uint16_t fine_y : 3;
            uint16_t unused : 1;
        };
        uint16_t reg = 0x0000;
    };

    loopy_register vram_addr;
    loopy_register tram_addr;

    uint8_t fine_x = 0x00;

    uint8_t bg_next_tile_id = 0x00;
    uint8_t bg_next_tile_attrib = 0x00;
    uint8_t bg_next_tile_lsb = 0x00;
    uint8_t bg_next_tile_msb = 0x00;

    uint16_t bg_shifter_pattern_lo = 0x0000;
    uint16_t bg_shifter_pattern_hi = 0x0000;
    uint16_t bg_shifter_attrib_lo = 0x0000;
    uint16_t bg_shifter_attrib_hi = 0x0000;

    struct ObjectAttributeEntity
    {
        uint8_t y;
        uint8_t id;
        uint8_t attribute;
        uint8_t x;
    };
    ObjectAttributeEntity OAM[64];
    
    uint8_t* OAM_ptr = (uint8_t*) OAM;
    
private:
    std::shared_ptr<Cartridge> cart;

    uint8_t ram_pattern_table[2][4096];
    uint8_t ram_nametable[2][1024];
    uint8_t ram_palette[32];

    uint8_t oam_addr = 0x00;

    ObjectAttributeEntity spriteScanline[8];
    uint8_t sprite_count;
    uint8_t sprite_shifter_pattern_lo[8];
    uint8_t sprite_shifter_pattern_hi[8];

    bool sprite_zero_hit_possible = false;
    bool sprite_zero_being_rendered = false;

    // debug
    PixelCanvas screen{ 256,240 };
    PixelCanvas screen_nametable[2]{ PixelCanvas{256,240}, PixelCanvas{256,240} };
    PixelCanvas screen_pattern_table[2]{ PixelCanvas{128,128}, PixelCanvas{128,128} };

    int16_t scanline = 0;
    int16_t cycle = 0;

    // ppu cycle instructions
    void incScrollX();
    void incScrollY();
    void tsfAddrX();
    void tsfAddrY();

    void loadBgShifters();
    void updateBgShifters();

    // util
    uint8_t flipbyte(uint8_t b);

public:
    uint32_t nes_palette[0x40]
    {
        0x545454FF, 0x001E74FF, 0x081090FF, 0x300088FF, 0x440064FF, 0x5C0030FF, 0x540400FF, 0x3C1800FF, 0x202A00FF, 0x083A00FF, 0x004000FF, 0x003C00FF, 0x00323CFF, 0x000000FF, 0x000000FF, 0x000000FF,
        0x989698FF, 0x084CC4FF, 0x3032ECFF, 0x5C1EE4FF, 0x8814B0FF, 0xA01464FF, 0x982220FF, 0x783C00FF, 0x545A00FF, 0x287200FF, 0x087C00FF, 0x007628FF, 0x006678FF, 0x000000FF, 0x000000FF, 0x000000FF,
        0xECEEECFF, 0x4C9AECFF, 0x787CECFF, 0xB062ECFF, 0xE454ECFF, 0xEC58B4FF, 0xEC6A64FF, 0xD48820FF, 0xA0AA00FF, 0x74C400FF, 0x4CD020FF, 0x38CC6CFF, 0x38B4CCFF, 0x3C3C3CFF, 0x000000FF, 0x000000FF,
        0xECEEECFF, 0xA8CCECFF, 0xBCBCECFF, 0xD4B2ECFF, 0xECAEECFF, 0xECAED4FF, 0xECB4B0FF, 0xE4C490FF, 0xCCD278FF, 0xB4DE78FF, 0xA8E290FF, 0x98E2B4FF, 0xA0D6E4FF, 0xA0A2A0FF, 0x000000FF, 0x000000FF
    };
};

#endif // !FNES_PPU2C02