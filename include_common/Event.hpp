/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Event
*/

#ifndef EVENT_HPP_
#define EVENT_HPP_

#include <cstdint>
#include <unordered_map>
#include <any>

namespace Engine {
    using EventId = std::uint32_t;
    using EventParamId = std::uint32_t;

    enum class EventsIds : EventId {
        PLAYER_INPUT,
        COLLISION,
        ENEMY_DESTROYED,
        PLAYER_HIT,
        DEVCONSOLE_KEY_PRESSED,
        DEVCONSOLE_TEXT_ENTERED,
    };

    enum class InputButtons {
        LEFT,
        RIGHT,
        UP,
        DOWN,
        SHOOT,
    };

    enum class KeyCodes {
        LEFT,
        RIGHT,
        UP,
        DOWN,
        DELETE,
    };

    class Event {
        public:
            Event(EventId type) : type(type) {}
            
            EventId getType() const { return (type); }
            template <typename T> void setParam(EventId id, T value);
            template <typename T> T getParam(EventId id);

        private:
            EventId type {};
            std::unordered_map<EventId, std::any> data {};
    };
};

template <typename T>
void Engine::Event::setParam(EventId id, T value)
{
    data[id] = value;
}

template <typename T>
T Engine::Event::getParam(EventId id)
{
    return (std::any_cast<T>(data[id]));
}

#endif /* !EVENT_HPP_ */
