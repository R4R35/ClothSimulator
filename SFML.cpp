#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <Windows.h>
#include <time.h>
#include "Particle.h"
using namespace std;


#define WIDTH 1200
#define HEIGHT 800
const float GRAVITY = 10.0f;
const float TIME_STEP = 0.1f;
const float PARTICLE_RADIUS = 10.0f;

const int rows = 10;
const int cols = 10;

int main()
{
    //Create window
    sf::RenderWindow window(sf::VideoMode({ WIDTH, HEIGHT}), "Physics?");
    window.setFramerateLimit(144);

    //Vector of particles
    vector<vector<Particle>> particlesGrid(rows, vector<Particle>());
    float spacing = 30.0f;
    float startX = WIDTH / 2 - 5 * spacing;
    float startY = 100; 

    //Initializing the grid
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            bool pinned = (i == 0 && j % 5 == 0);
            particlesGrid[i].emplace_back(startX + j * spacing, startY + i * spacing, pinned);

        }
    }

    vector <Constraint> constraints;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (j < cols - 1) {
                constraints.emplace_back(&particlesGrid[i][j], &particlesGrid[i][j + 1]);
            }
            if (i < rows - 1) {
                constraints.emplace_back(&particlesGrid[i][j], &particlesGrid[i + 1][j]);
            }
        }
    }

    sf::Vector2f force = { 0,GRAVITY};
    while (window.isOpen())
    {
        window.clear();
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::MouseButtonPressed>()) {
                auto mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                sf::Vector2f mousePos(mouseEvent->position.x, mouseEvent->position.y);

                for (int i = 0;i < rows;i++) {
                    for (int j = 0; j < cols;j++) {
                        float distance = hypot(
                            particlesGrid[i][j].position.x - mousePos.x,
                            particlesGrid[i][j].position.y - mousePos.y
                        );
                
                
                        if (distance <= PARTICLE_RADIUS) {
                            if (mouseEvent->button == sf::Mouse::Button::Left) {
                                particlesGrid[i][j].pinned = true;
                                particlesGrid[i][j].pinPosition = particlesGrid[i][j].position;
                            }
                            else if (mouseEvent->button == sf::Mouse::Button::Right)
                                particlesGrid[i][j].pinned = false;
                            break;
                        }
                    }
                }

            }
        }
        
        //satisfy the constraints
        for (auto& constraint : constraints) {
            constraint.satisfy();
        }



        //drawing the lines 
        for (auto& constraint : constraints) {
            sf::Vertex line[2]= {
                sf::Vertex(constraint.p1->position,sf::Color::White),
                sf::Vertex(constraint.p2->position,sf::Color::White),
            };
            window.draw(line , 2, sf::PrimitiveType::Lines);
        }

        //applying gravity 
        for (int i = 0;i < rows;i++) {
            for (int j = 0; j < cols;j++) {
                particlesGrid[i][j].apply_force(force);
                particlesGrid[i][j].update(TIME_STEP);
                particlesGrid[i][j].inBounds(WIDTH, HEIGHT, PARTICLE_RADIUS);
            }
        }
      

        //Drawing the particles
        for (int i = 0;i < rows;i++) {
            for (int j = 0; j < cols;j++) {
                sf::CircleShape circle(PARTICLE_RADIUS);
                particlesGrid[i][j].pinned ? circle.setFillColor(sf::Color::Red) : circle.setFillColor(sf::Color::White);
                circle.setOutlineColor(sf::Color::Blue);
                circle.setOrigin({ PARTICLE_RADIUS, PARTICLE_RADIUS });
                circle.setPosition(particlesGrid[i][j].position);
                window.draw(circle);
            }
        }
        
        window.display();
    }
}