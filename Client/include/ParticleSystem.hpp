/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ParticleSystem
*/


#ifndef PARTICLESYSTEM_HPP_
#define PARTICLESYSTEM_HPP_

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

enum class ParticleType {
    STAR,
    DUST,
    NEBULA,
    SHOOTING_STAR
};

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float size;
    float life;
    float maxLife;
    ParticleType type;
    float rotation;
    float rotationSpeed;
    float alpha;
    float scintillation;
    
    Particle(sf::Vector2f pos, sf::Vector2f vel, sf::Color col, float sz, float lf, ParticleType t)
        : position(pos), velocity(vel), color(col), size(sz), life(lf), maxLife(lf), type(t),
          rotation(0.0f), rotationSpeed(0.0f), alpha(255.0f), scintillation(0.0f) {}
};

class ParticleSystem {
    private:
        std::vector<Particle> particles;
        sf::Vector2u windowSize;
        std::mt19937 generator;
        std::uniform_real_distribution<float> distribution;
        
        int maxParticles;
        float spawnRate;
        float spawnTimer;
        std::vector<sf::Color> starColors;
        std::vector<sf::Color> dustColors;
        std::vector<sf::Color> nebulaColors;
        
    public:
        ParticleSystem(sf::Vector2u winSize, int maxPart = 500);
        ~ParticleSystem() = default;
        
        void update(float deltaTime);
        void render(sf::RenderWindow& window);
        void updateWindowSize(sf::Vector2u newSize);
        
    private:
        void spawnParticle(ParticleType type);
        void updateParticle(Particle& particle, float deltaTime);
        void renderParticle(sf::RenderWindow& window, const Particle& particle);
        sf::Color getRandomColor(ParticleType type);
        sf::Vector2f getRandomPosition();
        sf::Vector2f getRandomVelocity(ParticleType type);
        float getRandomSize(ParticleType type);
        float getRandomLife(ParticleType type);
};

#endif /* !PARTICLESYSTEM_HPP_ */