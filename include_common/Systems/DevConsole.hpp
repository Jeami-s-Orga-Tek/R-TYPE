/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** DevConsole
*/

#ifndef DEVCONSOLE_HPP_
#define DEVCONSOLE_HPP_

#include <cstdint>
#include <memory>
#include <string>

#include "Event.hpp"
#include "Renderer.hpp"
#include "System.hpp"
#include "NetworkManager.hpp"
#include "Utils.hpp"

namespace Engine {
    namespace Systems {
        class DevConsole : public System {
            public:
                void init(std::shared_ptr<Mediator> mediator) {
                    mediator->addEventListener(static_cast<EventId>(EventsIds::DEVCONSOLE_KEY_PRESSED), [this](Event &) { this->is_open = !this->is_open; });
                    mediator->addEventListener(static_cast<EventId>(EventsIds::DEVCONSOLE_TEXT_ENTERED), [this](Event &event) { this->treatNewTextInput(event); });
                };

                void update(std::shared_ptr<NetworkManager> networkManager, std::shared_ptr<Renderer> renderer) {
                    if (!is_open)
                        return;

                    if (command_to_parse != "")
                        parseCommand(networkManager, renderer);

                    const Engine::Utils::Vec2 window_size = {static_cast<float>(renderer->getWindowWidth()), static_cast<float>(renderer->getWindowHeight())};
                    renderer->drawRectangle({0, 0, window_size.x, window_size.y}, 0x333333EE);
                    renderer->drawText("basic", old_std_out, 0, 0, 10);
                    renderer->drawText("basic", current_command, 0, window_size.y - (window_size.y / 10.0f), 15);
                };

                void treatNewTextInput(Event &event) {
                    uint32_t keycode = event.getParam<uint32_t>(0);
                    uint32_t unicode = event.getParam<uint32_t>(1);

                    // std::cout << keycode << " | " << unicode << std::endl;

                    if (unicode == '\b') {
                        if (!current_command.empty() && cursor_pos > 0) {
                            current_command.erase(cursor_pos - 1, 1);
                            cursor_pos--;
                        }
                    }
                    else if (unicode == 127) {
                        if (!current_command.empty() && cursor_pos < current_command.size()) {
                            current_command.erase(cursor_pos, 1);
                        }
                    }
                    else if (unicode == '\r' || unicode == '\n') {
                        command_to_parse = current_command;
                        old_std_out += current_command;
                        old_std_out += "\n";
                        current_command = "";
                        cursor_pos = 0;
                    }
                    else if (unicode < 128 && std::isprint(unicode)) {
                        current_command.insert(cursor_pos, 1, static_cast<char>(unicode));
                        cursor_pos++;
                    }

                    if (keycode == static_cast<uint32_t>(KeyCodes::LEFT) && cursor_pos > 0) {
                        cursor_pos--;
                    }
                    if (keycode == static_cast<uint32_t>(KeyCodes::RIGHT) && cursor_pos < current_command.size()) {
                        cursor_pos++;
                    }
                };
            private:
                void parseCommand(std::shared_ptr<NetworkManager> networkManager, std::shared_ptr<Renderer> renderer) {
                    if (command_to_parse == "list_entities") {
                        old_std_out += "\tEntities: [stub: implement entity listing]\n";
                    } else if (command_to_parse == "list_systems") {
                        old_std_out += "\tSystems: [stub: implement system listing]\n";
                    } else if (command_to_parse == "list_components") {
                        old_std_out += "\tComponents: [stub: implement component listing]\n";
                    } else if (command_to_parse.rfind("help", 0) == 0) {
                        old_std_out += "\tAvailable commands: list_entities, list_systems, list_components, help\n";
                    } else if (!command_to_parse.empty()) {
                        old_std_out += "\tUnknown command: " + command_to_parse + "\n";
                    }
                    command_to_parse = "";
                }

                bool is_open = false;
                std::string old_std_out = "";
                std::string current_command = "";
                std::string command_to_parse = "";
                uint16_t cursor_pos = 0;
        };
    };
};

#endif /* !DEVCONSOLE_HPP_ */
