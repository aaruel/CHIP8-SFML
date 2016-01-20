
//
// main.cpp
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "chip8.h"

// Here is a small helper for you ! Have a look.
#include "ResourcePath.hpp"

void drawDisplay(const unsigned char *graphics, sf::RenderWindow * window);
void updateKeyState(chip &key);

int main(int argc, const char *argv[])
{
    // Load game flag
    bool flag = true;

    // Bringing over chip class variables
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
    sf::RenderWindow * window = new sf::RenderWindow
    (sf::VideoMode(832, 416), "CHIP8");
    window->setFramerateLimit(60);
    
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.jpg")) {
        return EXIT_FAILURE;
    }
    window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    sf::Font font;
    if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
        return EXIT_FAILURE;
    }
    sf::Text text("CHIP8", font, 30);
    text.setColor(sf::Color::White);

    sf::Music music;
    if (!music.openFromFile(resourcePath() + "nice_music.ogg")) {
        return EXIT_FAILURE;
    }
    else music.play();
    
    
    static float refreshSpeed = 1.f/60.f;
    sf::Clock clock;
    const unsigned char *graphics = nullptr;
    // Start the game loop
    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window->close();
            }
            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window->close();
            }
        }
        
        

        window->draw(text);
        if(clock.getElapsedTime().asSeconds() >= refreshSpeed){
            updateKeyState(imp);
            imp.emulateCycle();
            if(imp.drawScreenF()){
                window->clear();
                graphics = imp.getGraphics();
                drawDisplay(graphics, window);
                window->display();
                imp.setScreenF(false);
            }
            clock.restart();
        }
    }
    graphics = nullptr;
    delete window;
    window = nullptr;
    return EXIT_SUCCESS;
}

void drawDisplay(const unsigned char *graphics, sf::RenderWindow * window){
    //define what pixel size::::since resolution is constant, variables won't be used
    sf::RectangleShape rectangle(sf::Vector2f(13,13));
    for(int y=0; y < 32; ++y){
        for(int x=0; x < 64; ++x){
            unsigned char gfx = graphics[x+(64*y)];
            if(gfx > 0){
                rectangle.setPosition((float)(x*13), (float)(y*13));
                window->draw(rectangle);
            }
        }
    }
}

void updateKeyState(chip &key){
    key.setKeyState(0x1, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1));
    key.setKeyState(0x2, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2));
    key.setKeyState(0x3, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3));
    key.setKeyState(0xC, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4));
    
    key.setKeyState(0x4, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q));
    key.setKeyState(0x5, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W));
    key.setKeyState(0x6, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E));
    key.setKeyState(0xD, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R));
    
    key.setKeyState(0x7, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A));
    key.setKeyState(0x8, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S));
    key.setKeyState(0x9, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D));
    key.setKeyState(0xE, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F));
    
    key.setKeyState(0xA, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z));
    key.setKeyState(0x0, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X));
    key.setKeyState(0xB, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C));
    key.setKeyState(0xF, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V));
}
