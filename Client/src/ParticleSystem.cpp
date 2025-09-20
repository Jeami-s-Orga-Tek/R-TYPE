/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ParticleSystem
*/



#include "ParticleSystem.hpp"
#include <cmath>
#include <algorithm>

ParticleSystem::ParticleSystem(sf::Vector2u winSize, int maxPart)
    : windowSize(winSize), generator(std::random_device{}()), distribution(0.0f, 1.0f),
      maxParticles(maxPart), spawnRate(2.0f), spawnTimer(0.0f)
{
    particles.reserve(maxParticles);
    
    starColors = {
        sf::Color(255, 255, 255, 255), 
        sf::Color(255, 255, 200, 255),
        sf::Color(200, 200, 255, 255), 
        sf::Color(255, 200, 200, 255),
        sf::Color(200, 255, 255, 255)
    };
    
    dustColors = {
        sf::Color(100, 100, 150, 80),
        sf::Color(150, 100, 200, 60),
        sf::Color(80, 80, 120, 40),
        sf::Color(120, 80, 150, 50)
    };
    
    nebulaColors = {
        sf::Color(255, 100, 150, 30),
        sf::Color(100, 200, 255, 25),
        sf::Color(150, 255, 100, 35), 
        sf::Color(255, 200, 100, 40),
        sf::Color(200, 100, 255, 30)
    };

    for (int i = 0; i < maxParticles / 2; ++i) {
        if (distribution(generator) < 0.7f) {
            spawnParticle(ParticleType::STAR);
        } else if (distribution(generator) < 0.9f) {
            spawnParticle(ParticleType::DUST);
        } else {
            spawnParticle(ParticleType::NEBULA);
        }
    }
}

void ParticleSystem::update(float deltaTime)
{
    spawnTimer += deltaTime;
    if (spawnTimer >= 1.0f / spawnRate && particles.size() < static_cast<size_t>(maxParticles)) {
        spawnTimer = 0.0f;
        
        float rand = distribution(generator);
        if (rand < 0.6f) {
            spawnParticle(ParticleType::STAR);
        } else if (rand < 0.85f) {
            spawnParticle(ParticleType::DUST);
        } else if (rand < 0.98f) {
            spawnParticle(ParticleType::NEBULA);
        } else {
            spawnParticle(ParticleType::SHOOTING_STAR);
        }
    }
    
    for (auto it = particles.begin(); it != particles.end();) {
        updateParticle(*it, deltaTime);
        if (it->life <= 0.0f || 
            it->position.x < -50 || it->position.x > windowSize.x + 50 ||
            it->position.y < -50 || it->position.y > windowSize.y + 50) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleSystem::render(sf::RenderWindow& window)
{
    for (const auto& particle : particles) {
        renderParticle(window, particle);
    }
}

void ParticleSystem::updateWindowSize(sf::Vector2u newSize)
{
    sf::Vector2u oldSize = windowSize;
    windowSize = newSize;

    if (oldSize.x > 0 && oldSize.y > 0) {
        float scaleX = static_cast<float>(newSize.x) / static_cast<float>(oldSize.x);
        float scaleY = static_cast<float>(newSize.y) / static_cast<float>(oldSize.y);
        
        for (auto& particle : particles) {
            particle.position.x *= scaleX;
            particle.position.y *= scaleY;
            particle.position.x = std::max(0.0f, std::min(static_cast<float>(newSize.x), particle.position.x));
            particle.position.y = std::max(0.0f, std::min(static_cast<float>(newSize.y), particle.position.y));
        }
    }
}

void ParticleSystem::spawnParticle(ParticleType type)
{
    sf::Vector2f position = getRandomPosition();
    sf::Vector2f velocity = getRandomVelocity(type);
    sf::Color color = getRandomColor(type);
    float size = getRandomSize(type);
    float life = getRandomLife(type);
    
    Particle particle(position, velocity, color, size, life, type);
    
    switch (type) {
        case ParticleType::STAR:
            particle.scintillation = distribution(generator) * 6.28f; // 2π
            break;
        case ParticleType::SHOOTING_STAR:
            particle.velocity.x = -200.0f - distribution(generator) * 300.0f;
            particle.velocity.y = 50.0f + distribution(generator) * 100.0f;
            particle.position.x = windowSize.x + 50;
            particle.position.y = distribution(generator) * windowSize.y * 0.3f;
            break;
        case ParticleType::NEBULA:
            particle.rotationSpeed = (distribution(generator) - 0.5f) * 2.0f;
            break;
        case ParticleType::DUST:
            particle.alpha = 100.0f + distribution(generator) * 50.0f;
            break;
    }
    
    particles.push_back(particle);
}

void ParticleSystem::updateParticle(Particle& particle, float deltaTime)
{
    particle.position += particle.velocity * deltaTime;
    particle.life -= deltaTime;
    particle.rotation += particle.rotationSpeed * deltaTime;
    
    switch (particle.type) {
        case ParticleType::STAR:
            particle.scintillation += deltaTime * 3.0f;
            particle.alpha = 200.0f + 55.0f * std::sin(particle.scintillation);
            break;
            
        case ParticleType::DUST:
            particle.velocity.y += std::sin(particle.position.x * 0.01f) * 10.0f * deltaTime;
            particle.alpha = (particle.life / particle.maxLife) * 120.0f;
            break;
            
        case ParticleType::NEBULA:
            particle.size += deltaTime * 5.0f;
            particle.alpha = (particle.life / particle.maxLife) * 60.0f;
            break;      
        case ParticleType::SHOOTING_STAR:
            particle.alpha = (particle.life / particle.maxLife) * 255.0f;
            break;
    }
    particle.color.a = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, particle.alpha)));
}

void ParticleSystem::renderParticle(sf::RenderWindow& window, const Particle& particle)
{
    sf::CircleShape shape;
    
    switch (particle.type) {
        case ParticleType::STAR:
            shape.setRadius(particle.size);
            shape.setPointCount(5);
            break;
            
        case ParticleType::SHOOTING_STAR:
            shape.setRadius(particle.size);
            shape.setScale(3.0f, 0.5f);
            break;
            
        default:
            shape.setRadius(particle.size);
            shape.setPointCount(30);
            break;
    }
    
    shape.setFillColor(particle.color);
    shape.setPosition(particle.position.x - particle.size, particle.position.y - particle.size);
    shape.setRotation(particle.rotation);
    
    window.draw(shape);
    
    if (particle.type == ParticleType::STAR && particle.size > 2.0f) {
        sf::CircleShape halo;
        halo.setRadius(particle.size * 2.0f);
        halo.setPointCount(30);
        sf::Color haloColor = particle.color;
        haloColor.a = static_cast<sf::Uint8>(haloColor.a * 0.2f);
        halo.setFillColor(haloColor);
        halo.setPosition(particle.position.x - particle.size * 2.0f, 
                        particle.position.y - particle.size * 2.0f);
        window.draw(halo);
    }
}

sf::Color ParticleSystem::getRandomColor(ParticleType type)
{
    switch (type) {
        case ParticleType::STAR:
            return starColors[static_cast<int>(distribution(generator) * starColors.size())];
        case ParticleType::DUST:
            return dustColors[static_cast<int>(distribution(generator) * dustColors.size())];
        case ParticleType::NEBULA:
            return nebulaColors[static_cast<int>(distribution(generator) * nebulaColors.size())];
        case ParticleType::SHOOTING_STAR:
            return sf::Color(255, 255, 255, 255);
        default:
            return sf::Color::White;
    }
}

sf::Vector2f ParticleSystem::getRandomPosition()
{
    return sf::Vector2f(
        distribution(generator) * windowSize.x,
        distribution(generator) * windowSize.y
    );
}

sf::Vector2f ParticleSystem::getRandomVelocity(ParticleType type)
{
    switch (type) {
        case ParticleType::STAR:
            return sf::Vector2f(
                (distribution(generator) - 0.5f) * 20.0f,
                (distribution(generator) - 0.5f) * 20.0f
            );
        case ParticleType::DUST:
            return sf::Vector2f(
                -30.0f - distribution(generator) * 50.0f,
                (distribution(generator) - 0.5f) * 30.0f
            );
        case ParticleType::NEBULA:
            return sf::Vector2f(
                (distribution(generator) - 0.5f) * 40.0f,
                (distribution(generator) - 0.5f) * 40.0f
            );
        case ParticleType::SHOOTING_STAR:
            return sf::Vector2f(-200.0f, 50.0f);
        default:
            return sf::Vector2f(0.0f, 0.0f);
    }
}

float ParticleSystem::getRandomSize(ParticleType type)
{
    switch (type) {
        case ParticleType::STAR:
            return 1.0f + distribution(generator) * 3.0f;
        case ParticleType::DUST:
            return 0.5f + distribution(generator) * 1.5f;
        case ParticleType::NEBULA:
            return 8.0f + distribution(generator) * 15.0f;
        case ParticleType::SHOOTING_STAR:
            return 2.0f + distribution(generator) * 2.0f;
        default:
            return 1.0f;
    }
}

float ParticleSystem::getRandomLife(ParticleType type)
{
    switch (type) {
        case ParticleType::STAR:
            return 10.0f + distribution(generator) * 20.0f;
        case ParticleType::DUST:
            return 5.0f + distribution(generator) * 10.0f;
        case ParticleType::NEBULA:
            return 8.0f + distribution(generator) * 12.0f;
        case ParticleType::SHOOTING_STAR:
            return 1.0f + distribution(generator) * 2.0f;
        default:
            return 5.0f;
    }
}