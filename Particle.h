#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <vector>
#include <cmath>
#include <limits>

class Particle {

public:
	sf::Vector2f position;
	sf::Vector2f previous_position;
	sf::Vector2f acceleration;
    sf::Vector2f pinPosition;
    bool pinned;
    Particle(float x, float y,bool pin) : position({ x,y }), previous_position({ x,y }), acceleration({ 0,0 }), pinned(pin),pinPosition({x,y}) {};
    ~Particle() = default;

	void apply_force(sf::Vector2f& force) {
        if (!pinned) {
		acceleration += force;
        }

	}

	void update(float time_step) {
		//verlet integration
        if (!pinned) {
		sf::Vector2f velocity;
		velocity = position - previous_position;
		previous_position = position;
		position += velocity + acceleration * time_step * time_step;
		acceleration ={ 0,0 };//reset after updates
        }
        else {
            position = pinPosition;
            previous_position = pinPosition;
        }
	}

    void inBounds(float width, float height, float radius) {

        if (position.x < radius) {
            position.x = radius;
        }
        if (position.x > width - radius)
        {
            position.x = width - radius;
        }
        

        if(position.y < radius) {

            position.y = radius;
        }
        
        if (position.y > height - radius) {
            position.y = height - radius;
            
        }
    }
   
};


class Constraint {
public:
    Particle* p1;
    Particle* p2;
    float initial_length;

    Constraint(Particle* p1, Particle* p2) : p1(p1), p2(p2),
        initial_length(hypot(p2->position.x - p1->position.x, p2->position.y - p1->position.y)){}
    
    void satisfy() {
        sf::Vector2f delta = p2->position - p1->position;
        float current_length = hypot(delta.x, delta.y);
        float difference = (current_length - initial_length) / current_length;
        sf::Vector2f correction = delta * 0.5f * difference;
        
        if (p1->pinned && p2->pinned) return;    
        
        else if (p1->pinned && !p2->pinned)      
            p2->position -= 2.f * correction;   

        else if (!p1->pinned && p2->pinned)      
            p1->position += 2.f * correction;

        else {                  
            p1->position += correction;
            p2->position -= correction;

        }

    }
};