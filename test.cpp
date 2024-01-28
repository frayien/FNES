#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow win(sf::VideoMode(640, 480), "Title");

    sf::RenderTexture rtex;
    rtex.create(50,50);

    sf::CircleShape shape(25);
    shape.setFillColor(sf::Color::Red);

    while(win.isOpen())
    {
        // render texture
        rtex.clear(sf::Color(0x00000000));
        rtex.draw(shape);
        rtex.display();

        // render window
        win.clear(sf::Color(0x1a5e21FF));

        sf::Texture tex = rtex.getTexture();
        sf::Sprite sprite(tex);
        win.draw(sprite);
        
        win.display();

        sf::Event event;
        while(win.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
            {
                win.close();
            }
        }
    }
}