/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** EventManager
*/

#ifndef EVENTMANAGER_HPP_
#define EVENTMANAGER_HPP_

#include "Event.hpp"
#include <functional>
#include <list>

namespace Engine {
    class EventManager {
        public:
            void addListener(EventId event_id, std::function<void(Event &)> const &listener) {
                listeners[event_id].push_back(listener);
            }

            void sendEvent(Event &event) {
                uint32_t type = event.getType();

                for (auto const &listener : listeners[type]) {
                    listener(event);
                }
            }

            void sendEvent(EventId event_id) {
                Event event(event_id);

                for (auto const &listener : listeners[event_id]) {
                    listener(event);
                }
            }

        private:
            std::unordered_map<EventId, std::list<std::function<void(Event &)>>> listeners;
    };
};

#endif /* !EVENTMANAGER_HPP_ */
