
//
// main.cpp
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "chip8.h"

// Here is a small helper for you ! Have a look.
#include "ResourcePath.hpp"

int main(int argc, const char *argv[])
{
    bool flag = true;
    
    // Bringing over chip class varibles
    chip imp;
    
    //initialize variables
    imp.initialize();
    
    // Load game external flag from file into memory//
    flag = imp.loadGame();
    if(flag == false){
        printf("Could not open game!");
        return EXIT_FAILURE;
    }
    ///////ALL SFML INTEGRATION BELOW THIS///////
    ////////CURRENTLY AT A DEFAULT STATE/////////
    sf::RenderWindow window(sf::VideoMode(832, 416), "CHIP8");
    window.setFramerateLimit(60);
    
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.jpg")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    sf::Font font;
    if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
        return EXIT_FAILURE;
    }
    sf::Text text("CHIP8", font, 30);
    text.setColor(sf::Color::White);
    
    //define what pixel size::::since resolution is constant, variables won't be used
    sf::RectangleShape rectangle(sf::Vector2f(13,13));

    sf::Music music;
    if (!music.openFromFile(resourcePath() + "nice_music.ogg")) {
        return EXIT_FAILURE;
    }
    else music.play();

    // Start the game loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        window.clear();

        window.draw(text);
        
        // Draw Pixel
        window.draw(rectangle);

        window.display();
    }
    
    return EXIT_SUCCESS;
}
