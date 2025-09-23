/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ParamButton
*/

#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class ResolutionMode {
    RES_800x600,
    RES_1280x720,
    RES_1920x1080
};

enum class DisplayMode {
    WINDOWED,
    FULLSCREEN
};

enum class GraphicsQuality {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA
};

enum class ColorBlindMode {
    NORMAL,
    PROTANOPIA,
    DEUTERANOPIA,
    TRITANOPIA,
    MONOCHROME
};

class Parameters {
public:
    Parameters(sf::Vector2u windowSize);
    ~Parameters() = default;

    bool loadResources();
    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update();
    void updateWindowSize(sf::Vector2u newSize);
    
    ResolutionMode getCurrentResolution() const { return currentResolution; }
    DisplayMode getCurrentDisplayMode() const { return currentDisplayMode; }
    GraphicsQuality getCurrentGraphicsQuality() const { return currentGraphicsQuality; }
    ColorBlindMode getCurrentColorBlindMode() const { return currentColorBlindMode; }

    void setResolution(ResolutionMode resolution);
    void setDisplayMode(DisplayMode mode);
    void setGraphicsQuality(GraphicsQuality quality);
    void setColorBlindMode(ColorBlindMode mode);
    sf::Vector2u getResolutionSize(ResolutionMode resolution) const;
    
    sf::Color applyColorBlindFilter(const sf::Color& originalColor) const;

private:
    sf::Text ParamsText;
    sf::Text VideoSectionTitle;
    sf::Text ResolutionLabel;
    sf::Text ResolutionValue;
    sf::Text DisplayModeLabel;
    sf::Text DisplayModeValue;
    sf::Text GraphicsQualityLabel;
    sf::Text GraphicsQualityValue;
    sf::Text AccessibilityTitle;
    sf::Text ColorBlindLabel;
    sf::Text ColorBlindValue;
    
    sf::Font FontParamsText;
    sf::Vector2u windowSize;

    ResolutionMode currentResolution;
    DisplayMode currentDisplayMode;
    GraphicsQuality currentGraphicsQuality;
    ColorBlindMode currentColorBlindMode;
    
    void centerText();
    void setupVideoSettings();
    void updateVideoSettingsPositions();
    std::string getResolutionString(ResolutionMode resolution) const;
    std::string getDisplayModeString(DisplayMode mode) const;
    std::string getGraphicsQualityString(GraphicsQuality quality) const;
    std::string getColorBlindModeString(ColorBlindMode mode) const;
};

#endif /* !PARAMETERS_HPP__ */
