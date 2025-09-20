/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Parameters
*/

#include "Parameters.hpp"
#include <iostream>
#include <algorithm>

Parameters::Parameters(sf::Vector2u windowSize) : windowSize(windowSize),
    currentResolution(ResolutionMode::RES_800x600),
    currentDisplayMode(DisplayMode::WINDOWED),
    currentGraphicsQuality(GraphicsQuality::MEDIUM)
{

}

bool Parameters::loadResources()
{
    if (!FontParamsText.loadFromFile("/usr/share/fonts/google-carlito-fonts/Carlito-Regular.ttf")) {
        std::cerr << "Erreur" << std::endl;
        return false;
    }
    
    ParamsText.setFont(FontParamsText);
    ParamsText.setString("PARAMETRES");
    ParamsText.setCharacterSize(36);
    ParamsText.setFillColor(sf::Color::White);
    
    setupVideoSettings();
    centerText();

    return true;
}

void Parameters::centerText()
{
    sf::FloatRect TextBounds = ParamsText.getLocalBounds();

    float centerX = (windowSize.x - TextBounds.width) / 2.0f;
    float centerY = 50.0f;

    ParamsText.setPosition(centerX, centerY);
}

void Parameters::draw(sf::RenderWindow& window)
{
    window.draw(ParamsText);
    window.draw(VideoSectionTitle);
    window.draw(ResolutionLabel);
    window.draw(ResolutionValue);
    window.draw(DisplayModeLabel);
    window.draw(DisplayModeValue);
    window.draw(GraphicsQualityLabel);
    window.draw(GraphicsQualityValue);
}

void Parameters::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            window.close();
        }
    }

    if (event.type == sf::Event::Resized) {
        windowSize.x = event.size.width;
        windowSize.y = event.size.height;
        centerText();
    }
}

void Parameters::update()
{
}

void Parameters::updateWindowSize(sf::Vector2u newSize)
{
    windowSize = newSize;
    centerText();
    updateVideoSettingsPositions();
}

void Parameters::updateVideoSettingsPositions()
{
    float leftMargin = std::max(30.0f, windowSize.x * 0.05f);
    float labelWidth = std::min(200.0f, windowSize.x * 0.3f);
    float rightOffset = leftMargin + labelWidth + 20;
    
    if (windowSize.x < 900) {
        rightOffset = std::min(rightOffset, windowSize.x * 0.5f);
    }
    
    VideoSectionTitle.setPosition(leftMargin, 150);
    ResolutionLabel.setPosition(leftMargin, 200);
    ResolutionValue.setPosition(rightOffset, 200);
    DisplayModeLabel.setPosition(leftMargin, 250);
    DisplayModeValue.setPosition(rightOffset, 250);
    GraphicsQualityLabel.setPosition(leftMargin, 300);
    GraphicsQualityValue.setPosition(rightOffset, 300);
}

void Parameters::setupVideoSettings()
{
    VideoSectionTitle.setFont(FontParamsText);
    VideoSectionTitle.setString("VIDEO");
    VideoSectionTitle.setCharacterSize(24);
    VideoSectionTitle.setFillColor(sf::Color::Cyan);
    
    ResolutionLabel.setFont(FontParamsText);
    ResolutionLabel.setString("Resolution:");
    ResolutionLabel.setCharacterSize(18);
    ResolutionLabel.setFillColor(sf::Color::White);
    
    ResolutionValue.setFont(FontParamsText);
    ResolutionValue.setString(getResolutionString(currentResolution));
    ResolutionValue.setCharacterSize(18);
    ResolutionValue.setFillColor(sf::Color::Yellow);
    
    DisplayModeLabel.setFont(FontParamsText);
    DisplayModeLabel.setString("Mode d'affichage:");
    DisplayModeLabel.setCharacterSize(18);
    DisplayModeLabel.setFillColor(sf::Color::White);
    
    DisplayModeValue.setFont(FontParamsText);
    DisplayModeValue.setString(getDisplayModeString(currentDisplayMode));
    DisplayModeValue.setCharacterSize(18);
    DisplayModeValue.setFillColor(sf::Color::Yellow);
    
    GraphicsQualityLabel.setFont(FontParamsText);
    GraphicsQualityLabel.setString("Qualite graphique:");
    GraphicsQualityLabel.setCharacterSize(18);
    GraphicsQualityLabel.setFillColor(sf::Color::White);
    
    GraphicsQualityValue.setFont(FontParamsText);
    GraphicsQualityValue.setString(getGraphicsQualityString(currentGraphicsQuality));
    GraphicsQualityValue.setCharacterSize(18);
    GraphicsQualityValue.setFillColor(sf::Color::Yellow);
    updateVideoSettingsPositions();
}

std::string Parameters::getResolutionString(ResolutionMode resolution) const
{
    switch (resolution) {
        case ResolutionMode::RES_800x600: return "800x600";
        case ResolutionMode::RES_1280x720: return "1280x720";
        case ResolutionMode::RES_1920x1080: return "1920x1080";
        default: return "800x600";
    }
}

std::string Parameters::getDisplayModeString(DisplayMode mode) const
{
    switch (mode) {
        case DisplayMode::WINDOWED: return "Fenetre";
        case DisplayMode::FULLSCREEN: return "Plein ecran";
        default: return "Fenetre";
    }
}

std::string Parameters::getGraphicsQualityString(GraphicsQuality quality) const
{
    switch (quality) {
        case GraphicsQuality::LOW: return "Faible";
        case GraphicsQuality::MEDIUM: return "Moyen";
        case GraphicsQuality::HIGH: return "Eleve";
        case GraphicsQuality::ULTRA: return "Ultra";
        default: return "Moyen";
    }
}

sf::Vector2u Parameters::getResolutionSize(ResolutionMode resolution) const
{
    switch (resolution) {
        case ResolutionMode::RES_800x600: return sf::Vector2u(800, 600);
        case ResolutionMode::RES_1280x720: return sf::Vector2u(1280, 720);
        case ResolutionMode::RES_1920x1080: return sf::Vector2u(1920, 1080);
        default: return sf::Vector2u(800, 600);
    }
}

void Parameters::setResolution(ResolutionMode resolution)
{
    currentResolution = resolution;
    ResolutionValue.setString(getResolutionString(resolution));
}

void Parameters::setDisplayMode(DisplayMode mode)
{
    currentDisplayMode = mode;
    DisplayModeValue.setString(getDisplayModeString(mode));
}

void Parameters::setGraphicsQuality(GraphicsQuality quality)
{
    currentGraphicsQuality = quality;
    GraphicsQualityValue.setString(getGraphicsQualityString(quality));
}
