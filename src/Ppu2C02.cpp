#include "Ppu2C02.hpp"
#include "Mapper.hpp"

Ppu2C02::Ppu2C02()
{
}

Ppu2C02::~Ppu2C02()
{
}

uint8_t Ppu2C02::cpuRead(uint16_t addr, bool read_only)
{
    uint8_t data = 0x00;
    switch(addr)
    {
    case 0x0000: // PPUCTRL
        break;
    case 0x0001: // PPUMASK
        break;
    case 0x0002: // PPUSTATUS
        data = (ppustatus.reg & 0xE0) | (ppu_data_buffer & 0x1F);
        ppustatus.vertical_blank = 0;
        address_latch = 0;
        break;
    case 0x0003: // OAMADDR
        break;
    case 0x0004: // OAMDATA
        data = OAM_ptr[oam_addr];
        break;
    case 0x0005: // PPUSCROLL
        break;
    case 0x0006: // PPUADDR
        break;
    case 0x0007: // PPUDATA
        data = ppu_data_buffer;
        ppu_data_buffer = ppuRead(vram_addr.reg);

        // if reading in palette range, no delay
        if(vram_addr.reg > 0x3F00) data = ppu_data_buffer;

        vram_addr.reg += (ppuctrl.increment_mode*31 + 1);
        break;
    }

    return data;
}

void Ppu2C02::cpuWrite(uint16_t addr, uint8_t data)
{
    switch(addr)
    {
    case 0x0000: // PPUCTRL
        ppuctrl.reg = data;
        tram_addr.nametable_x = ppuctrl.nametable_x;
        tram_addr.nametable_y = ppuctrl.nametable_y;
        break;
    case 0x0001: // PPUMASK
        ppumask.reg = data;
        break;
    case 0x0002: // PPUSTATUS
        // readonly
        break;
    case 0x0003: // OAMADDR
        oam_addr = data;
        break;
    case 0x0004: // OAMDATA
        OAM_ptr[oam_addr] = data;
        break;
    case 0x0005: // PPUSCROLL
        if(address_latch == 0)
        {
            fine_x = data & 0x07;
            tram_addr.coarse_x = data >> 3;
            address_latch = 1;
        }
        else
        {
            tram_addr.fine_y = data & 0x07;
            tram_addr.coarse_y = data >> 3;
            address_latch = 0;
        }
        break;
    case 0x0006: // PPUADDR
        if(address_latch == 0)
        {
            tram_addr.reg = (tram_addr.reg & 0x00FF) | ((data & 0x3F) << 8);
            address_latch = 1;
        }
        else
        {
            tram_addr.reg = (tram_addr.reg & 0xFF00) | data;
            vram_addr = tram_addr;
            address_latch = 0;
        }
        break;
    case 0x0007: // PPUDATA
        ppuWrite(vram_addr.reg, data);
        vram_addr.reg += (ppuctrl.increment_mode*31 + 1);
        break;
    }
}

uint8_t Ppu2C02::ppuRead(uint16_t addr, bool read_only)
{
    addr &= 0x3FFF;
    uint8_t data = 0x00;

    if(cart->ppuRead(addr, data))
    {
        // cart handled
    }
    else if(addr >= 0x0000 && addr <= 0x1FFF) // pattern table
    {
        data = ram_pattern_table[(addr & 0x1000) >> 12][addr & 0x0FFF];
    }
    else if(addr >= 0x2000 && addr <= 0x3EFF) // nametable
    {
        if(cart->mirror() == MirrorMode::HORIZONTAL)
        {
            if(addr >= 0x2000 && addr <= 0x23FF)
                data = ram_nametable[0][addr & 0x03FF];
            if(addr >= 0x2400 && addr <= 0x27FF)
                data = ram_nametable[0][addr & 0x03FF];
            if(addr >= 0x2800 && addr <= 0x2BFF)
                data = ram_nametable[1][addr & 0x03FF];
            if(addr >= 0x2C00 && addr <= 0x2FFF)
                data = ram_nametable[1][addr & 0x03FF];
        }
        else if(cart->mirror() == MirrorMode::VERTICAL)
        {
            if(addr >= 0x2000 && addr <= 0x23FF)
                data = ram_nametable[0][addr & 0x03FF];
            if(addr >= 0x2400 && addr <= 0x27FF)
                data = ram_nametable[1][addr & 0x03FF];
            if(addr >= 0x2800 && addr <= 0x2BFF)
                data = ram_nametable[0][addr & 0x03FF];
            if(addr >= 0x2C00 && addr <= 0x2FFF)
                data = ram_nametable[1][addr & 0x03FF];
        }
    }
    else if(addr >= 0x3F00 && addr <= 0x3FFF) // palette
    {
        addr &= 0x001F;
        if(addr == 0x0010) addr = 0x0000;
        if(addr == 0x0014) addr = 0x0004;
        if(addr == 0x0018) addr = 0x0008;
        if(addr == 0x001C) addr = 0x000C;
        data = ram_palette[addr];
    }

    return data;
}

void Ppu2C02::ppuWrite(uint16_t addr, uint8_t data)
{
    addr &= 0x3FFF;

    if(cart->ppuWrite(addr, data))
    {
        // cart handled
    }
    else if(addr >= 0x0000 && addr <= 0x1FFF) // pattern table
    {
        ram_pattern_table[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
    }
    else if(addr >= 0x2000 && addr <= 0x3EFF) // nametable
    {
        if(cart->mirror() == MirrorMode::HORIZONTAL)
        {
            if(addr >= 0x2000 && addr <= 0x23FF)
                ram_nametable[0][addr & 0x03FF] = data;
            if(addr >= 0x2400 && addr <= 0x27FF)
                ram_nametable[0][addr & 0x03FF] = data;
            if(addr >= 0x2800 && addr <= 0x2BFF)
                ram_nametable[1][addr & 0x03FF] = data;
            if(addr >= 0x2C00 && addr <= 0x2FFF)
                ram_nametable[1][addr & 0x03FF] = data;
        }
        else if(cart->mirror() == MirrorMode::VERTICAL)
        {
            if(addr >= 0x2000 && addr <= 0x23FF)
                ram_nametable[0][addr & 0x03FF] = data;
            if(addr >= 0x2400 && addr <= 0x27FF)
                ram_nametable[1][addr & 0x03FF] = data;
            if(addr >= 0x2800 && addr <= 0x2BFF)
                ram_nametable[0][addr & 0x03FF] = data;
            if(addr >= 0x2C00 && addr <= 0x2FFF)
                ram_nametable[1][addr & 0x03FF] = data;
        }
    }
    else if(addr >= 0x3F00 && addr <= 0x3FFF) // palette
    {
        addr &= 0x001F;
        if(addr == 0x0010) addr = 0x0000;
        if(addr == 0x0014) addr = 0x0004;
        if(addr == 0x0018) addr = 0x0008;
        if(addr == 0x001C) addr = 0x000C;
        ram_palette[addr] = data;
    }
}

void Ppu2C02::connectCartridge(std::shared_ptr<Cartridge> const& cartridge)
{
    cart = cartridge;
}

void Ppu2C02::clock()
{
    if(scanline >= -1 && scanline < 240)
    {
        if(scanline == 0 && cycle == 0)
        {
            cycle = 1;
        }

        if(scanline == -1 && cycle == 1)
        {
            ppustatus.vertical_blank = 0;
            ppustatus.sprite_zero_hit = 0;
            ppustatus.sprite_overflow = 0;

            for(int i = 0; i < 8; ++i)
            {
                sprite_shifter_pattern_lo[i] = 0;
                sprite_shifter_pattern_hi[i] = 0;
            }
        }

        if((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338))
        {
            updateBgShifters();

            switch ((cycle - 1) % 8)
            {
            case 0:
                loadBgShifters();
                bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
                break;
            case 2:
                bg_next_tile_attrib = ppuRead(0x23C0 
                    | (vram_addr.nametable_y << 11)
                    | (vram_addr.nametable_x << 10)
                    | ((vram_addr.coarse_y >> 2) << 3)
                    | (vram_addr.coarse_x >> 2));
                if(vram_addr.coarse_y & 0x02) bg_next_tile_attrib >>= 4;
                if(vram_addr.coarse_x & 0x02) bg_next_tile_attrib >>= 2;
                bg_next_tile_attrib &= 0x03;
                break;
            case 4:
                bg_next_tile_lsb = ppuRead(
                    (ppuctrl.pattern_background << 12)
                    + ((uint16_t)bg_next_tile_id << 4)
                    + (vram_addr.fine_y) + 0);
                break;
            case 6:
                bg_next_tile_msb = ppuRead(
                    (ppuctrl.pattern_background << 12)
                    + ((uint16_t)bg_next_tile_id << 4)
                    + (vram_addr.fine_y) + 8);
                break;
            case 7:
                incScrollX();
                break;
            }
        }

        if(cycle == 256)
        {
            incScrollY();
        }

        if(cycle == 257)
        {
            loadBgShifters();
            tsfAddrX();
        }

        // Superfluous reads of tile id at end of scanline
        if (cycle == 338 || cycle == 340)
        {
            bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
        }

        if(scanline == -1 && cycle >= 280 && cycle < 305)
        {
            tsfAddrY();
        }

        // //////////////////////////////////// Foreground //////////////////////////////////// //
        if(cycle == 257 && scanline >= 0) // sprite evaluation phase
        {
            std::memset(spriteScanline, 0xFF, 8* sizeof(ObjectAttributeEntity));
            sprite_count = 0;

            uint8_t OAMEntry = 0;
            sprite_zero_hit_possible = false;
            while (OAMEntry < 64 && sprite_count < 9)
            {
                int16_t diff = ((int16_t)scanline - (int16_t) OAM[OAMEntry].y);
                if(diff >= 0 && diff < (ppuctrl.sprite_size*8 + 8))
                {
                    if(sprite_count < 8)
                    {
                        if(OAMEntry == 0) sprite_zero_hit_possible = true;

                        std::memcpy(&spriteScanline[sprite_count], &OAM[OAMEntry], sizeof(ObjectAttributeEntity));
                        sprite_count++;
                    }
                }
                OAMEntry++;
            }
            ppustatus.sprite_overflow = (sprite_count > 8);
        }

        if(cycle == 340)
        {
            for(uint8_t i = 0; i < sprite_count; ++i)
            {
                uint8_t sprite_pattern_bits_lo, sprite_pattern_bits_hi;
                uint16_t sprite_pattern_addr_lo, sprite_pattern_addr_hi;

                if(!ppuctrl.sprite_size)
                { // 8x8 Sprite mode
                    if(!(spriteScanline[i].attribute & 0x80))
                    { // Sprite NOT flipped
                        sprite_pattern_addr_lo =
                            (ppuctrl.pattern_sprite << 12)
                            | (spriteScanline[i].id << 4)
                            | (scanline - spriteScanline[i].y);
                    }
                    else
                    { // Sprite flipper vertically
                        sprite_pattern_addr_lo =
                            (ppuctrl.pattern_sprite << 12)
                            | (spriteScanline[i].id << 4)
                            | (7 - (scanline - spriteScanline[i].y));
                    }
                }
                else
                { // 8x16 Sprite mode
                    if(!(spriteScanline[i].attribute & 0x80))
                    { // Sprite NOT flipped
                        if(scanline - spriteScanline[i].y < 8)
                        { // top half
                            sprite_pattern_addr_lo = 
                                ((spriteScanline[i].id & 0x01) << 12)
                                | ((spriteScanline[i].id & 0xFE) << 4)
                                | ((scanline - spriteScanline[i].y) & 0x07);
                        }
                        else
                        { // bottom half
                            sprite_pattern_addr_lo = 
                                ((spriteScanline[i].id & 0x01) << 12)
                                | (((spriteScanline[i].id & 0xFE) + 1) << 4)
                                | ((scanline - spriteScanline[i].y) & 0x07);
                        }
                    }
                    else
                    { // Sprite flipper vertically
                        if(scanline - spriteScanline[i].y < 8)
                        { // top half
                            sprite_pattern_addr_lo = 
                                ((spriteScanline[i].id & 0x01) << 12)
                                | ((spriteScanline[i].id & 0xFE) << 4)
                                | ((7 - (scanline - spriteScanline[i].y)) & 0x07);
                        }
                        else
                        { // bottom half
                            sprite_pattern_addr_lo = 
                                ((spriteScanline[i].id & 0x01) << 12)
                                | (((spriteScanline[i].id & 0xFE) + 1) << 4)
                                | ((7 - (scanline - spriteScanline[i].y)) & 0x07);
                        }
                    }
                }

                sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;
                sprite_pattern_bits_lo = ppuRead(sprite_pattern_addr_lo);
                sprite_pattern_bits_hi = ppuRead(sprite_pattern_addr_hi);

                if(spriteScanline[i].attribute & 0x40)
                { // Flip pattern horizontally
                    sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
                    sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
                }

                sprite_shifter_pattern_lo[i] = sprite_pattern_bits_lo;
                sprite_shifter_pattern_hi[i] = sprite_pattern_bits_hi;
            }
        }

    }

    if(scanline == 240)
    {
        // nothing
    }

    if(scanline == 241 && cycle == 1)
    {
        ppustatus.vertical_blank = 1;
        if(ppuctrl.enable_nmi)
            nmi = true;
    }

    uint8_t bg_pixel = 0x00;
    uint8_t bg_palette = 0x00;

    if(ppumask.render_background)
    {
        uint16_t bit_mux = 0x8000 >> fine_x;

        uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
        uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;
        bg_pixel = (p1_pixel << 1) | p0_pixel;

        uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
        uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
        bg_palette = (bg_pal1 << 1) | bg_pal0;
    }

    uint8_t fg_pixel = 0x00;
    uint8_t fg_palette = 0x00;
    uint8_t fg_priority = 0x00;

    if(ppumask.render_sprites)
    {
        sprite_zero_being_rendered = false;

        for(uint8_t i = 0; i < sprite_count; ++i)
        {
            if(spriteScanline[i].x == 0)
            {
                uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
                uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
                fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;

                fg_palette = (spriteScanline[i].attribute & 0x03) + 0x04;
                fg_priority = (spriteScanline[i].attribute & 0x20) == 0;

                if(fg_pixel != 0)
                {
                    if(i == 0) sprite_zero_being_rendered = true;
                    break;
                }
            }
        }
    }

    uint8_t pixel = 0x00;
    uint8_t palette = 0x00;

    if(bg_pixel == 0 && fg_pixel == 0)
    { // bg and fg transparent, draw "bg" color
        pixel = 0x00;
        palette = 0x00;
    }
    else if (bg_pixel == 0 && fg_pixel > 0)
    { // bg transparent, fg visible, fg win
        pixel = fg_pixel;
        palette = fg_palette;
    }
    else if (bg_pixel > 0 && fg_pixel == 0)
    { // bg visible, fg transparent, bg win
        pixel = bg_pixel;
        palette = bg_palette;
    }
    else if (bg_pixel > 0 && fg_pixel > 0)
    {// bg visible, fg visible, fight !
        if(fg_priority)
        { // fg cheat to win
            pixel = fg_pixel;
            palette = fg_palette;
        }
        else
        { // bg win
            pixel = bg_pixel;
            palette = bg_palette;
        }

        if(sprite_zero_hit_possible && sprite_zero_being_rendered)
        {
            if(ppumask.render_background & ppumask.render_sprites)
            {
                if(!(ppumask.render_background_left | ppumask.render_sprites_left))
                {
                    if(cycle >= 9 && cycle < 258)
                    {
                        ppustatus.sprite_zero_hit = 1;
                    }
                }
                else
                {
                    if(cycle >= 1 && cycle < 258)
                    {
                        ppustatus.sprite_zero_hit = 1;
                    }
                }
            }
        }
    }

    screen.setPixel(cycle-1, scanline, getColorFromPaletteRam(palette, pixel));

    cycle++;
    if(cycle >= 341)
    {
        cycle = 0;
        scanline++;
        if(scanline >= 261)
        {
            scanline = -1;
            frame_complete = true;
        }
    }
}

uint32_t Ppu2C02::getColorFromPaletteRam(uint8_t palette, uint8_t pixel)
{
    return nes_palette[ppuRead(0x3F00 + (palette << 2) + pixel)];
}

PixelCanvas& Ppu2C02::getScreen()
{
    return screen;
}

PixelCanvas& Ppu2C02::getNametable(uint8_t i)
{
    return screen_nametable[i];
}

PixelCanvas& Ppu2C02::getPatternTable(uint8_t id, uint8_t palette)
{
    for(int y = 0; y<16; ++y)
    {
        for(int x = 0; x<16; ++x)
        {
            for(int j = 0; j<8; ++j)
            {
                uint8_t tile_lsb = ppuRead(id * 0x1000 + y*16*16 + x*16 + j + 0);
                uint8_t tile_msb = ppuRead(id * 0x1000 + y*16*16 + x*16 + j + 8);

                for(int i = 0; i<8; ++i)
                {
                    uint8_t pixel = (tile_lsb & 0b0000'0001) | ((tile_msb & 0b0000'0001) << 1);
                    tile_lsb >>= 1;
                    tile_msb >>= 1;

                    screen_pattern_table[id].setPixel(x*8+(7-i), y*8+j, getColorFromPaletteRam(palette, pixel));
                }
            }
        }
    }
    return screen_pattern_table[id];
}

void Ppu2C02::incScrollX()
{
    if(ppumask.render_background || ppumask.render_sprites)
    {
        if(vram_addr.coarse_x == 31)
        {
            vram_addr.coarse_x = 0;
            vram_addr.nametable_x = ~vram_addr.nametable_x;
        }
        else
        {
            vram_addr.coarse_x++;
        }
    }
}
void Ppu2C02::incScrollY()
{
    if(ppumask.render_background || ppumask.render_sprites)
    {
        if(vram_addr.fine_y < 7)
        {
            vram_addr.fine_y++;
        }
        else
        {
            vram_addr.fine_y = 0;

            if(vram_addr.coarse_y == 29)
            {
                vram_addr.coarse_y = 0;
                vram_addr.nametable_y = ~vram_addr.nametable_y;
            }
            else if(vram_addr.coarse_y == 31)
            {
                vram_addr.coarse_y = 0;
            }
            else
            {
                vram_addr.coarse_y++;
            }
        }
    }
}
void Ppu2C02::tsfAddrX()
{
    if(ppumask.render_background || ppumask.render_sprites)
    {
        vram_addr.nametable_x = tram_addr.nametable_x;
        vram_addr.coarse_x = tram_addr.coarse_x;
    }
}
void Ppu2C02::tsfAddrY()
{
    if(ppumask.render_background || ppumask.render_sprites)
    {
        vram_addr.fine_y = tram_addr.fine_y;
        vram_addr.nametable_y = tram_addr.nametable_y;
        vram_addr.coarse_y = tram_addr.coarse_y;
    }
}

void Ppu2C02::loadBgShifters()
{
    bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
    bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;

    bg_shifter_attrib_lo = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);
    bg_shifter_attrib_hi = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);
}

void Ppu2C02::updateBgShifters()
{
    if(ppumask.render_background)
    {
        bg_shifter_pattern_lo <<= 1;
        bg_shifter_pattern_hi <<= 1;
        bg_shifter_attrib_lo <<= 1;
        bg_shifter_attrib_hi <<= 1;
    }

    if(ppumask.render_sprites && cycle >= 1 && cycle < 258)
    {
        for(int i = 0; i < sprite_count; ++i)
        {
            if(spriteScanline[i].x > 0)
            {
                spriteScanline[i].x--;
            }
            else
            {
                sprite_shifter_pattern_lo[i] <<= 1;
                sprite_shifter_pattern_hi[i] <<= 1;
            }
        }
    }
}

uint8_t Ppu2C02::flipbyte(uint8_t b)
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}