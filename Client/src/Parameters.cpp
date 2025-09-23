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
    currentGraphicsQuality(GraphicsQuality::MEDIUM),
    currentColorBlindMode(ColorBlindMode::NORMAL)
{

}

bool Parameters::loadResources()
{
    if (!FontParamsText.loadFromFile("assets/r-type.otf")) {
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
    sf::Text tempParamsText = ParamsText;
    sf::Text tempVideoSectionTitle = VideoSectionTitle;
    sf::Text tempResolutionLabel = ResolutionLabel;
    sf::Text tempResolutionValue = ResolutionValue;
    sf::Text tempDisplayModeLabel = DisplayModeLabel;
    sf::Text tempDisplayModeValue = DisplayModeValue;
    sf::Text tempGraphicsQualityLabel = GraphicsQualityLabel;
    sf::Text tempGraphicsQualityValue = GraphicsQualityValue;
    sf::Text tempAccessibilityTitle = AccessibilityTitle;
    sf::Text tempColorBlindLabel = ColorBlindLabel;
    sf::Text tempColorBlindValue = ColorBlindValue;

    tempParamsText.setFillColor(applyColorBlindFilter(ParamsText.getFillColor()));
    tempVideoSectionTitle.setFillColor(applyColorBlindFilter(VideoSectionTitle.getFillColor()));
    tempResolutionLabel.setFillColor(applyColorBlindFilter(ResolutionLabel.getFillColor()));
    tempResolutionValue.setFillColor(applyColorBlindFilter(ResolutionValue.getFillColor()));
    tempDisplayModeLabel.setFillColor(applyColorBlindFilter(DisplayModeLabel.getFillColor()));
    tempDisplayModeValue.setFillColor(applyColorBlindFilter(DisplayModeValue.getFillColor()));
    tempGraphicsQualityLabel.setFillColor(applyColorBlindFilter(GraphicsQualityLabel.getFillColor()));
    tempGraphicsQualityValue.setFillColor(applyColorBlindFilter(GraphicsQualityValue.getFillColor()));
    tempAccessibilityTitle.setFillColor(applyColorBlindFilter(AccessibilityTitle.getFillColor()));
    tempColorBlindLabel.setFillColor(applyColorBlindFilter(ColorBlindLabel.getFillColor()));
    tempColorBlindValue.setFillColor(applyColorBlindFilter(ColorBlindValue.getFillColor()));
    window.draw(tempParamsText);
    window.draw(tempVideoSectionTitle);
    window.draw(tempResolutionLabel);
    window.draw(tempResolutionValue);
    window.draw(tempDisplayModeLabel);
    window.draw(tempDisplayModeValue);
    window.draw(tempGraphicsQualityLabel);
    window.draw(tempGraphicsQualityValue);
    window.draw(tempAccessibilityTitle);
    window.draw(tempColorBlindLabel);
    window.draw(tempColorBlindValue);
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
    ColorBlindLabel.setPosition(leftMargin, 350);
    ColorBlindValue.setPosition(rightOffset, 350);
    AccessibilityTitle.setPosition(leftMargin, 420);
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
    DisplayModeLabel.setString("Display Mode:");
    DisplayModeLabel.setCharacterSize(18);
    DisplayModeLabel.setFillColor(sf::Color::White);
    
    DisplayModeValue.setFont(FontParamsText);
    DisplayModeValue.setString(getDisplayModeString(currentDisplayMode));
    DisplayModeValue.setCharacterSize(18);
    DisplayModeValue.setFillColor(sf::Color::Yellow);
    
    GraphicsQualityLabel.setFont(FontParamsText);
    GraphicsQualityLabel.setString("Graphics Quality:");
    GraphicsQualityLabel.setCharacterSize(18);
    GraphicsQualityLabel.setFillColor(sf::Color::White);
    
    GraphicsQualityValue.setFont(FontParamsText);
    GraphicsQualityValue.setString(getGraphicsQualityString(currentGraphicsQuality));
    GraphicsQualityValue.setCharacterSize(18);
    GraphicsQualityValue.setFillColor(sf::Color::Yellow);
    ColorBlindLabel.setFont(FontParamsText);
    ColorBlindLabel.setString("Color Blind Mode:");
    ColorBlindLabel.setCharacterSize(18);
    ColorBlindLabel.setFillColor(sf::Color::White);
    ColorBlindValue.setFont(FontParamsText);
    ColorBlindValue.setString(getColorBlindModeString(currentColorBlindMode));
    ColorBlindValue.setCharacterSize(18);
    ColorBlindValue.setFillColor(sf::Color::Yellow);
    
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
        case DisplayMode::WINDOWED: return "Windowed";
        case DisplayMode::FULLSCREEN: return "Fullscreen";
        default: return "Windowed";
    }
}

std::string Parameters::getGraphicsQualityString(GraphicsQuality quality) const
{
    switch (quality) {
        case GraphicsQuality::LOW: return "Low";
        case GraphicsQuality::MEDIUM: return "Medium";
        case GraphicsQuality::HIGH: return "High";
        case GraphicsQuality::ULTRA: return "Ultra";
        default: return "Medium";
    }
}

std::string Parameters::getColorBlindModeString(ColorBlindMode mode) const
{
    switch (mode) {
        case ColorBlindMode::NORMAL: return "Normal";
        case ColorBlindMode::PROTANOPIA: return "Protanopie";
        case ColorBlindMode::DEUTERANOPIA: return "Deuteranopie";
        case ColorBlindMode::TRITANOPIA: return "Tritanopie";
        case ColorBlindMode::MONOCHROME: return "Monochrome";
        default: return "Normal";
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

void Parameters::setColorBlindMode(ColorBlindMode mode)
{
    currentColorBlindMode = mode;
    ColorBlindValue.setString(getColorBlindModeString(mode));
}

sf::Color Parameters::applyColorBlindFilter(const sf::Color& originalColor) const
{
    switch (currentColorBlindMode) {
        case ColorBlindMode::NORMAL:
            return originalColor;
            
        case ColorBlindMode::PROTANOPIA:
        {
            float r = originalColor.r / 255.0f;
            float g = originalColor.g / 255.0f;
            float b = originalColor.b / 255.0f;
            float newR = 0.567f * r + 0.433f * g + 0.0f * b;
            float newG = 0.558f * r + 0.442f * g + 0.0f * b;
            float newB = 0.0f * r + 0.242f * g + 0.758f * b;
            
            return sf::Color(
                (sf::Uint8)(std::min(255.0f, std::max(0.0f, newR * 255.0f))),
                (sf::Uint8)(std::min(255.0f, std::max(0.0f, newG * 255.0f))),
                (sf::Uint8)(std::min(255.0f, std::max(0.0f, newB * 255.0f))),
                originalColor.a
            );
        }
        
        case ColorBlindMode::DEUTERANOPIA:
        {
            float r = originalColor.r / 255.0f;
            float g = originalColor.g / 255.0f;
            float b = originalColor.b / 255.0f;
            float newR = 0.625f * r + 0.375f * g + 0.0f * b;
            float newG = 0.7f * r + 0.3f * g + 0.0f * b;
            float newB = 0.0f * r + 0.3f * g + 0.7f * b;
            
            return sf::Color(
                (sf::Uint8)(std::min(255.0f, std::max(0.0f, newR * 255.0f))),
                (sf::Uint8)(std::min(255.0f, std::max(0.0f, newG * 255.0f))),
                (sf::Uint8)(std::min(255.0f, std::max(0.0f, newB * 255.0f))),
                originalColor.a
            );
        }
        case ColorBlindMode::TRITANOPIA:
        {
            float r = originalColor.r / 255.0f;
            float g = originalColor.g / 255.0f;
            float b = originalColor.b / 255.0f;
            float newR = 0.95f * r + 0.05f * g + 0.0f * b;
            float newG = 0.0f * r + 0.433f * g + 0.567f * b;
            float newB = 0.0f * r + 0.475f * g + 0.525f * b;
            
            return sf::Color(
                (sf::Uint8)(std::min(255.0f, std::max(0.0f, newR * 255.0f))),
                (sf::Uint8)(std::min(255.0f, std::max(0.0f, newG * 255.0f))),
                (sf::Uint8)(std::min(255.0f, std::max(0.0f, newB * 255.0f))),
                originalColor.a
            );
        }
        case ColorBlindMode::MONOCHROME:
        {
            float luminance = 0.299f * originalColor.r + 0.587f * originalColor.g + 0.114f * originalColor.b;
            sf::Uint8 gray = (sf::Uint8)(std::min(255.0f, std::max(0.0f, luminance)));
            
            return sf::Color(gray, gray, gray, originalColor.a);
        }
        
        default:
            return originalColor;
    }
}
