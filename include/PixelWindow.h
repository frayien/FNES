/**
 * @file PixelWindow.h
 * @author frayien (frayien@yahoo.fr)
 * @brief A Window with Pixel level graphics
 * @version 0.1
 * @date 2020-11-01
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef FNES_PIXEL_WINDOW
#define FNES_PIXEL_WINDOW

#include <cstdint>
#include <array>
#include <set>
#include <string>
#include <atomic>
#include <SFML/Graphics.hpp>

#include "PixelCanvas.h"

class PixelWindow
{
public:
    PixelWindow(uint16_t x, uint16_t y, sf::String const& title);
    virtual ~PixelWindow();

    void renderThread();
    void run();

    void setBackgroundColor(sf::Color const& color) { background_color = color; }

    virtual void init() = 0;
    virtual void update() = 0;

    bool wasPressed(sf::Keyboard::Key key);

    void drawCanvas(PixelCanvas& canvas, uint16_t x, uint16_t y, float zoom);

private:
    uint16_t size_x;
    uint16_t size_y;

    sf::Color background_color;
    sf::Thread render_thread;

    std::set<sf::Keyboard::Key> pressed_keys;
    sf::Mutex mutex_pressed_keys;

    std::atomic_bool m_stop_interupt;

    sf::RenderWindow window;
};

#endif // !FNES_PIXEL_WINDOW