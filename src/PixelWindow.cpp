#include "PixelWindow.hpp"

PixelWindow::PixelWindow(uint16_t x, uint16_t y, sf::String const& title) :
    size_x(x), size_y(y),
    render_thread(&PixelWindow::renderThread, this),
    m_stop_interupt(false),
    window(sf::VideoMode(size_x, size_y), title)
{
    window.setFramerateLimit(60);
}

PixelWindow::~PixelWindow()
{

}

void PixelWindow::renderThread()
{
    window.setActive(true);

    sf::Clock tps_clock;
    float tps = 0;

    float frame_count = 0;

    while(window.isOpen())
    {
        window.clear(background_color);

        // call user update
        mutex_pressed_keys.lock();
        update();
        pressed_keys.clear();
        mutex_pressed_keys.unlock();

        window.display();

        frame_count++;
        if(tps_clock.getElapsedTime().asSeconds() > 1.0f)
        {
            tps = tps_clock.restart().asSeconds();
            window.setTitle("FNES (" + std::to_string(1.0f / tps * frame_count) + " fps)");
            frame_count = 0;
        }

        if(m_stop_interupt.exchange(false))
        {
            window.close();
        }
    }
}

void PixelWindow::run()
{
    window.setActive(false);
    render_thread.launch();

    sf::Event event;
    while(window.isOpen())
    {
        while(window.waitEvent(event))
        {
            switch(event.type)
            {
            case sf::Event::KeyPressed:
                if(event.key.code != sf::Keyboard::Escape)
                {
                    mutex_pressed_keys.lock();
                    pressed_keys.insert(event.key.code);
                    mutex_pressed_keys.unlock();
                    break;
                }
                [[fallthrough]];
            case sf::Event::Closed:
                m_stop_interupt.store(true);
                goto label_break;
            default:
                break;
            }
        }
    }
    label_break:
    render_thread.wait();
}

bool PixelWindow::wasPressed(sf::Keyboard::Key key)
{
    sf::Lock l(mutex_pressed_keys);
    return pressed_keys.find(key) != pressed_keys.end();
}

void PixelWindow::drawCanvas(PixelCanvas& canvas, uint16_t x, uint16_t y, float zoom)
{
    const sf::Texture & tex = canvas.getTexture();
    sf::Sprite sprite(tex);
    sprite.setPosition(x,y);
    sprite.setScale(zoom,zoom);
    window.draw(sprite);
}