#ifndef FNES_PIXELCANVAS
#define FNES_PIXELCANVAS

#include <array>
#include <cstdint>
#include <memory>

#include <SFML/Graphics.hpp>

typedef std::array<uint32_t, 8*8> Tile;
typedef std::array<Tile, 16*16> TileSet;

class PixelCanvas
{
public:
    PixelCanvas(uint16_t x, uint16_t y);
    virtual ~PixelCanvas();

    // no copy
    PixelCanvas(PixelCanvas&) = delete;
    PixelCanvas& operator=(PixelCanvas&) = delete;

    // move is default
    PixelCanvas(PixelCanvas&&) = default;
    PixelCanvas& operator=(PixelCanvas&&) = default;

    inline void setPixel(uint16_t x, uint16_t y, uint32_t color)
    {
        if(x < size_x && y < size_y)
            image.setPixel(x, y, sf::Color(color));
    }

    void clear(uint32_t color = 0x000000FF);

    void drawSprite(uint16_t x, uint16_t y, Tile const& tile, uint32_t color = 0xFFFFFFFF);
    void drawSprite(uint16_t x, uint16_t y, TileSet const& tile_set, uint16_t tile_x, uint16_t tile_y, uint32_t color = 0xFFFFFFFF);
    void drawTileSet(uint16_t x, uint16_t y, TileSet const& tile_set);

    void drawChar(uint16_t x, uint16_t y, char c, uint32_t color = 0xFFFFFFFF);
    void drawString(uint16_t x, uint16_t y, std::string const& str, uint16_t mx = 0, uint16_t my = 0);

    inline uint16_t getSizeX() const { return size_x; }
    inline uint16_t getSizeY() const { return size_y; }

    inline const sf::Texture& getTexture() 
    { 
        texture.update(image);
        return texture;
    }

    static void initFont();
    static TileSet font;
private:
    uint16_t size_x;
    uint16_t size_y;

    sf::Image image;
    sf::Texture texture;
};

#endif // FNES_PIXELCANVAS
