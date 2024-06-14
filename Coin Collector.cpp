#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

class Player {
public:
    sf::RectangleShape shape;
    sf::Vector2f velocity;

    bool isJumping;
    float jumpHeight;

    Player(float x, float y) {
        shape.setSize(sf::Vector2f(50, 50));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Red);

        isJumping = false;
        jumpHeight = 200.0f;
    }

    void update() {
        shape.move(velocity);
    }

    void jump() {
        if (!isJumping) {
            velocity.y = -6.0f;
            isJumping = true;
        }
    }
};

class Ground {
public:
    sf::RectangleShape shape;

    Ground(float x, float y, float width, float height) {
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2f(width, height));
        shape.setFillColor(sf::Color::Green);
    }
};

class Platform {
public:
    sf::RectangleShape shape;

    Platform(float x, float y, float width, float height) {
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2f(width, height));
        shape.setFillColor(sf::Color::Cyan);
    }
};

class Coin {
public:
    sf::CircleShape shape;

    Coin(float x, float y) {
        shape.setRadius(15.0f);
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Yellow);
    }
};

class Enemy {
public:
    sf::RectangleShape shape;
    sf::Vector2f velocity;

    Enemy(float x, float y) {
        shape.setSize(sf::Vector2f(50, 50));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Blue);
        velocity.x = -6.0f; // Initial velocity, move left
    }

    void update() {
        shape.move(velocity);

        // Check for screen boundaries and reverse direction
        if (shape.getPosition().x < 0 || shape.getPosition().x + shape.getSize().x > 800) {
            velocity.x = -velocity.x;
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Super Mario");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("C:\\Users\\omerh\\OneDrive\\Desktop\\Glow Hockey\\neon.otf")) {
        std::cerr << "Error loading font file" << std::endl;
        return EXIT_FAILURE;
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    sf::Text winText;
    winText.setFont(font);
    winText.setCharacterSize(50);
    winText.setFillColor(sf::Color::Green);
    winText.setPosition(200, 250);

    sf::Text loseText;
    loseText.setFont(font);
    loseText.setCharacterSize(50);
    loseText.setFillColor(sf::Color::Red);
    loseText.setPosition(200, 250);

    std::srand(static_cast<unsigned int>(std::time(0))); // Seed for random number generation

    Player player(100, 100);
    Ground ground(0, 550, 800, 50);

    std::vector<Coin> coins;
    const int maxCoins = 10;

    Enemy enemy(700, 500);

    std::vector<Platform> platforms;
    platforms.push_back(Platform(150, 400, 150, 20)); // Platform 1
    platforms.push_back(Platform(500, 300, 150, 20)); // Platform 2

    int score = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    player.jump();
                }
            }
        }

        // Handle player input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.velocity.x = -5.0f;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.velocity.x = 5.0f;
        }
        else {
            player.velocity.x = 0.0f;
        }

        // Simulate gravity
        player.velocity.y += 0.1f;

        // Update player
        player.update();

        // Check for ground collision
        if (player.shape.getPosition().y + player.shape.getSize().y > ground.shape.getPosition().y) {
            player.velocity.y = 0.0f;
            player.shape.setPosition(player.shape.getPosition().x, ground.shape.getPosition().y - player.shape.getSize().y);
            player.isJumping = false;
        }

        // Check for platform collisions
        for (const auto& platform : platforms) {
            if (player.shape.getGlobalBounds().intersects(platform.shape.getGlobalBounds()) &&
                player.shape.getPosition().y + player.shape.getSize().y < platform.shape.getPosition().y + 10) {
                player.velocity.y = 0.0f;
                player.shape.setPosition(player.shape.getPosition().x, platform.shape.getPosition().y - player.shape.getSize().y);
                player.isJumping = false;
            }
        }

        // Check for coin collection
        for (auto it = coins.begin(); it != coins.end(); /* no increment here */) {
            if (player.shape.getGlobalBounds().intersects(it->shape.getGlobalBounds())) {
                score += 10;
                it = coins.erase(it);
            }
            else {
                ++it;
            }
        }

        // Generate new coins randomly if the limit hasn't been reached
        if (coins.size() < maxCoins && rand() % 100 < 5) {
            float x = rand() % 750 + 25; // Random x position between 25 and 775
            float y = rand() % 450 + 50; // Random y position between 50 and 500
            coins.push_back(Coin(x, y));
        }

        // Update enemy
        enemy.update();

        // Check for enemy collision
        if (player.shape.getGlobalBounds().intersects(enemy.shape.getGlobalBounds())) {
            loseText.setString("Game Over! You lose. Score: " + std::to_string(score));
            window.clear();
            window.draw(loseText);
            window.display();
            sf::sleep(sf::seconds(3)); // Display the message for 3 seconds
            window.close();
        }

        // Check for winning condition
        if (score >= 500) {
            winText.setString("Congratulations! You won the game!");
            window.clear();
            window.draw(winText);
            window.display();
            sf::sleep(sf::seconds(3)); // Display the message for 3 seconds
            window.close();
        }
        else {
            // Update score text
            scoreText.setString("Score: " + std::to_string(score));

            // Render
            window.clear();
            window.draw(ground.shape);
            window.draw(player.shape);
            for (const auto& coin : coins) {
                window.draw(coin.shape);
            }
            for (const auto& platform : platforms) {
                window.draw(platform.shape);
            }
            window.draw(enemy.shape);
            window.draw(scoreText); // Draw the score text
            window.display();
        }
    }

    return 0;
}