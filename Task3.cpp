#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>

// Constants
const int WIDTH = 800;
const int HEIGHT = 600;
const int SIZE = 20;
const float INITIAL_SPEED = 0.15f;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct SnakeSegment {
    int x, y;
    SnakeSegment(int x, int y) : x(x), y(y) {}
};

class Snake {
public:
    std::vector<SnakeSegment> body;
    Direction dir = RIGHT;
    bool grow = false;

    Snake() {
        body.emplace_back(5, 5);
        body.emplace_back(4, 5);
        body.emplace_back(3, 5);
    }

    void move() {
        SnakeSegment head = body.front();
        switch (dir) {
            case UP: head.y--; break;
            case DOWN: head.y++; break;
            case LEFT: head.x--; break;
            case RIGHT: head.x++; break;
        }
        body.insert(body.begin(), head);
        if (!grow) body.pop_back();
        grow = false;
    }

    void changeDir(Direction newDir) {
        if ((dir == UP && newDir != DOWN) || (dir == DOWN && newDir != UP) ||
            (dir == LEFT && newDir != RIGHT) || (dir == RIGHT && newDir != LEFT)) {
            dir = newDir;
        }
    }

    bool checkCollision() {
        SnakeSegment head = body.front();
        if (head.x < 0 || head.y < 0 || head.x >= WIDTH / SIZE || head.y >= HEIGHT / SIZE) return true;
        for (size_t i = 1; i < body.size(); ++i)
            if (body[i].x == head.x && body[i].y == head.y) return true;
        return false;
    }

    bool eatsFood(int foodX, int foodY) {
        SnakeSegment head = body.front();
        return head.x == foodX && head.y == foodY;
    }
};

int main() {
    srand(static_cast<unsigned>(time(0)));

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Snake Game - SFML");
    window.setFramerateLimit(60);

    sf::RectangleShape snakeBlock(sf::Vector2f(SIZE - 2, SIZE - 2));
    snakeBlock.setFillColor(sf::Color::Green);

    sf::RectangleShape foodBlock(sf::Vector2f(SIZE - 2, SIZE - 2));
    foodBlock.setFillColor(sf::Color::Red);

    sf::SoundBuffer eatBuffer;
    if (!eatBuffer.loadFromFile("eat.wav")) {
        return -1; // Ensure sound file exists
    }
    sf::Sound eatSound(eatBuffer);

    Snake snake;
    int foodX = rand() % (WIDTH / SIZE);
    int foodY = rand() % (HEIGHT / SIZE);
    float timer = 0, delay = INITIAL_SPEED;

    sf::Clock clock;

    while (window.isOpen()) {
        float time = clock.restart().asSeconds();
        timer += time;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Keyboard input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) snake.changeDir(UP);
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) snake.changeDir(DOWN);
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) snake.changeDir(LEFT);
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) snake.changeDir(RIGHT);

        if (timer > delay) {
            timer = 0;
            snake.move();

            // Check if food is eaten
            if (snake.eatsFood(foodX, foodY)) {
                snake.grow = true;
                eatSound.play();
                foodX = rand() % (WIDTH / SIZE);
                foodY = rand() % (HEIGHT / SIZE);
                delay *= 0.95f; // Increase speed
            }

            if (snake.checkCollision()) {
                window.close(); // Game over
            }
        }

        // Rendering
        window.clear(sf::Color::Black);

        // Draw snake
        for (auto &segment : snake.body) {
            snakeBlock.setPosition(segment.x * SIZE, segment.y * SIZE);
            window.draw(snakeBlock);
        }

        // Draw food
        foodBlock.setPosition(foodX * SIZE, foodY * SIZE);
        window.draw(foodBlock);

        window.display();
    }

    return 0;
}
