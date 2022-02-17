#include "PixelCanvas.h"

TileSet PixelCanvas::font;

PixelCanvas::PixelCanvas(uint16_t _x, uint16_t _y) :
    size_x(_x), size_y(_y),
    vertex_array(sf::Points)
{
    render_texture = std::make_unique<sf::RenderTexture>();
    render_texture->create(size_x, size_y);

    vertex_array.resize(size_x*size_y);

    for(uint16_t y = 0; y<size_y; ++y)
    {
        for(uint16_t x = 0; x<size_x; ++x)
        {
            vertex_array[y*size_x+x].position = sf::Vector2f(x, size_y-y);
            vertex_array[y*size_x+x].color = sf::Color(0x000000FF);
        }
    }
}

PixelCanvas::~PixelCanvas()
{
}

void PixelCanvas::clear(uint32_t color)
{
    for(uint16_t y = 0; y<size_y; ++y)
    {
        for(uint16_t x = 0; x<size_x; ++x)
        {
            setPixel(x,y,color);
        }
    }
}

void PixelCanvas::drawSprite(uint16_t x, uint16_t y, Tile const& tile, uint32_t color)
{
    for(uint16_t oy = 0; oy<8; ++oy)
        for(uint16_t ox = 0; ox<8; ++ox)
            setPixel(x+ox, y+oy, tile[oy*8 + ox] & color);
}

void PixelCanvas::drawSprite(uint16_t x, uint16_t y, TileSet const& tile_set, uint16_t tile_x, uint16_t tile_y, uint32_t color)
{
    drawSprite(x, y, tile_set[tile_y*16+tile_x], color);
}

void PixelCanvas::drawTileSet(uint16_t x, uint16_t y, TileSet const& tile_set)
{
    for(uint16_t j = 0; j<16; ++j)
        for(uint16_t i = 0; i<16; ++i)
            drawSprite(x+i*8, y+j*8, tile_set, i, j);
}

void PixelCanvas::drawChar(uint16_t x, uint16_t y, char c, uint32_t color)
{
    uint16_t ccode =  (c - ' ');
    if(ccode > 0x7F - ' ')
        drawSprite(x, y, font, 15, 1, color);
    else
        drawSprite(x, y, font, ccode%16, ccode/16, color);
}

void PixelCanvas::drawString(uint16_t x, uint16_t y, std::string const& str, uint16_t mx, uint16_t my)
{
    uint32_t color = 0xFFFFFFFF;

    uint16_t off_x = 0;
    uint16_t off_y = 0;

    bool meta_flag = false;

    for(char c : str)
    {
        if(meta_flag)
        {
            meta_flag = false;
            switch(c)
            {
            case '\\':
                drawChar(x+off_x, y+off_y, c, color);
                off_x += 8 + mx;
                break;
            case 'r': color = 0xFF0000FF; break; // red
            case 'g': color = 0x00FF00FF; break; // green
            case 'b': color = 0x0000FFFF; break; // blue
            case 'c': color = 0x00FFFFFF; break; // cyan
            case 'm': color = 0xFF00FFFF; break; // magenta
            case 'y': color = 0xFFFF00FF; break; // yellow
            case 'w': color = 0xFFFFFFFF; break; // white
            case 'd': color = 0x000000FF; break; // dark (black)
            case 't': color = 0x00000000; break; // transparent
            }
        }
        else
        {
            if(c == '\\')
            {
                meta_flag = true;
            }
            else if(c == '\n')
            {
                off_x = 0;
                off_y += 8 + my;
            }
            else
            {
                drawChar(x+off_x, y+off_y, c, color);
                off_x += 8 + mx;
            }
        }
    }
}


void PixelCanvas::initFont()
{
    // init font
    std::string data;
    data += "?Q`0001oOch0o01o@F40o0<AGD4090LAGD<090@A7ch0?00O7Q`0600>00000000";
    data += "O000000nOT0063Qo4d8>?7a14Gno94AA4gno94AaOT0>o3`oO400o7QN00000400";
    data += "Of80001oOg<7O7moBGT7O7lABET024@aBEd714AiOdl717a_=TH013Q>00000000";
    data += "720D000V?V5oB3Q_HdUoE7a9@DdDE4A9@DmoE4A;Hg]oM4Aj8S4D84@`00000000";
    data += "OaPT1000Oa`^13P1@AI[?g`1@A=[OdAoHgljA4Ao?WlBA7l1710007l100000000";
    data += "ObM6000oOfMV?3QoBDD`O7a0BDDH@5A0BDD<@5A0BGeVO5ao@CQR?5Po00000000";
    data += "Oc``000?Ogij70PO2D]??0Ph2DUM@7i`2DTg@7lh2GUj?0TO0C1870T?00000000";
    data += "70<4001o?P<7?1QoHg43O;`h@GT0@:@LB@d0>:@hN@L0@?aoN@<0O7ao0000?000";
    data += "OcH0001SOglLA7mg24TnK7ln24US>0PL24U140PnOgl0>7QgOcH0K71S0000A000";
    data += "00H00000@Dm1S007@DUSg00?OdTnH7YhOfTL<7Yh@Cl0700?@Ah0300700000000";
    data += "<008001QL00ZA41a@6HnI<1i@FHLM81M@@0LG81?O`0nC?Y7?`0ZA7Y300080000";
    data += "O`082000Oh0827mo6>Hn?Wmo?6HnMb11MP08@C11H`08@FP0@@0004@000000000";
    data += "00P00001Oab00003OcKP0006@6=PMgl<@440MglH@000000`@000001P00000000";
    data += "Ob@8@@00Ob@8@Ga13R@8Mga172@8?PAo3R@827QoOb@820@0O`0007`0000007P0";
    data += "O`000P08Od400g`<3V=P0G`673IP0`@3>1`00P@6O`P00g`<O`000GP800000000";
    data += "?P9PL020O`<`N3R0@E4HC7b0@ET<ATB0@@l6C4B0O`H3N7b0?P01L3R000000020";

    int px = 0, py = 0;
    for (size_t b = 0; b < 1024; b += 4)
    {
        uint32_t sym1 = (uint32_t)data[b + 0] - 48;
        uint32_t sym2 = (uint32_t)data[b + 1] - 48;
        uint32_t sym3 = (uint32_t)data[b + 2] - 48;
        uint32_t sym4 = (uint32_t)data[b + 3] - 48;
        uint32_t r = sym1 << 18 | sym2 << 12 | sym3 << 6 | sym4;

        for (int i = 0; i < 24; i++)
        {
            uint32_t k = r & (1 << i) ? 0xFFFFFFFF : 0x00000000;
            font[(py/8)*16+(px/8)][(py%8)*8+(px%8)] = k;
            if (++py == 48) { px++; py = 0; }
        }
    }
}