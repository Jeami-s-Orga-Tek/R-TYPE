/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** DevConsole
*/

#ifndef DEVCONSOLE_HPP_
#define DEVCONSOLE_HPP_

#include <boost/pfr/core.hpp>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>
#include <boost/pfr/core_name.hpp>
#include <boost/type_index.hpp>
#include <boost/lexical_cast.hpp>

#include "Components/EnemyInfo.hpp"
#include "Components/Gravity.hpp"
#include "Components/Hitbox.hpp"
#include "Components/PlayerInfo.hpp"
#include "Components/RigidBody.hpp"
#include "Components/ShootingCooldown.hpp"
#include "Components/Sound.hpp"
#include "Components/Sprite.hpp"
#include "Components/Transform.hpp"
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
                    renderer->drawText("dev", old_std_out, 0, 0, 20);
                    renderer->drawText("dev", current_command, 0, window_size.y - (window_size.y / 10.0f), 25);
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
                        old_std_out = "";
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
                    if (command_to_parse.rfind("set ", 0) == 0) {
                        size_t first_space = command_to_parse.find(' ');
                        size_t second_space = command_to_parse.find(' ', first_space + 1);
                        if (first_space != std::string::npos && second_space != std::string::npos) {
                            std::string var = command_to_parse.substr(first_space + 1, second_space - first_space - 1);
                            std::string value = command_to_parse.substr(second_space + 1);
                            console_vars[var] = value;
                            old_std_out += "\tSet var '" + var + "' to '" + value + "'\n";
                        } else {
                            old_std_out += "\tUsage: set <var> <value>\n";
                        }
                    } else if (command_to_parse.rfind("get ", 0) == 0) {
                        size_t first_space = command_to_parse.find(' ');
                        std::string var = command_to_parse.substr(first_space + 1);
                        auto it = console_vars.find(var);
                        if (it != console_vars.end()) {
                            old_std_out += "\t" + var + " = '" + it->second + "'\n";
                        } else {
                            old_std_out += "\tVar '" + var + "' not found\n";
                        }
                    } else if (command_to_parse.rfind("get_entity ", 0) == 0) {
                        size_t first_space = command_to_parse.find(' ');
                        Entity entity = std::stoi(command_to_parse.substr(first_space + 1));

                        //TEMP
                        reflectComponent<Components::EnemyInfo>(networkManager, entity);
                        reflectComponent<Components::Gravity>(networkManager, entity);
                        reflectComponent<Components::Hitbox>(networkManager, entity);
                        reflectComponent<Components::PlayerInfo>(networkManager, entity);
                        reflectComponent<Components::RigidBody>(networkManager, entity);
                        reflectComponent<Components::ShootingCooldown>(networkManager, entity);
                        reflectComponent<Components::Sound>(networkManager, entity);
                        reflectComponent<Components::Sprite>(networkManager, entity);
                        reflectComponent<Components::Transform>(networkManager, entity);
                    } else if (command_to_parse.rfind("set_entity ", 0) == 0) {
                        try {
                            size_t first_space = command_to_parse.find(' ');
                            Entity entity = std::stoi(command_to_parse.substr(first_space + 1));
                            std::string component_name = "";
                            size_t second_space = command_to_parse.find(' ', first_space + 1);
                            size_t third_space = command_to_parse.find(' ', second_space + 1);
                            if (second_space != std::string::npos && third_space != std::string::npos) {
                                component_name = command_to_parse.substr(second_space + 1, third_space - second_space - 1);
                                std::string field_name = command_to_parse.substr(third_space + 1, command_to_parse.find(' ', third_space + 1) - third_space - 1);
                                std::string value = command_to_parse.substr(command_to_parse.find(' ', third_space + 1) + 1);

                                //TEMP
                                old_std_out += "\tSet entity " + std::to_string(entity) + " component '" + component_name + "', \nfield '" + field_name + "' to '" + value + "\n";
                                setComponentValue<Components::EnemyInfo>(networkManager, entity, component_name, field_name, value);
                                setComponentValue<Components::Gravity>(networkManager, entity, component_name, field_name, value);
                                setComponentValue<Components::Hitbox>(networkManager, entity, component_name, field_name, value);
                                setComponentValue<Components::PlayerInfo>(networkManager, entity, component_name, field_name, value);
                                setComponentValue<Components::RigidBody>(networkManager, entity, component_name, field_name, value);
                                setComponentValue<Components::ShootingCooldown>(networkManager, entity, component_name, field_name, value);
                                setComponentValue<Components::Sound>(networkManager, entity, component_name, field_name, value);
                                setComponentValue<Components::Sprite>(networkManager, entity, component_name, field_name, value);
                                setComponentValue<Components::Transform>(networkManager, entity, component_name, field_name, value);
                            } else {
                                old_std_out += "\tUsage: set_entity <entity> <component> <field> <value>\n";
                            }
                        } catch (...) {}
                    } else if (command_to_parse == "list_vars") {
                        old_std_out += "\tConsole Vars:\n";
                        for (const auto &pair : console_vars) {
                            old_std_out += "\t  " + pair.first + " = '" + pair.second + "'\n";
                        }
                    } else if (command_to_parse == "list_entities") {
                        old_std_out += "\t" + std::to_string(networkManager->mediator->getEntityCount()) + "\n";
                    } else if (command_to_parse == "list_systems") {
                        const auto &systems = networkManager->mediator->getSystemNames();
                        for (const auto &system_name : systems) {
                            old_std_out += "\t" + system_name + "\n";
                        }
                    } else if (command_to_parse == "list_components") {
                        const auto &components = networkManager->mediator->getComponentsNames();
                        for (const auto &component_name : components) {
                            old_std_out += "\t" + component_name + "\n";
                        }
                    } else if (command_to_parse.rfind("help", 0) == 0) {
                        old_std_out += "\tAvailable commands: list_entities, list_systems, list_components, set <var> <value>, get <var>, list_vars, get_entity <entity>, help\n";
                    } else if (!command_to_parse.empty()) {
                        old_std_out += "\tUnknown command: " + command_to_parse + "\n";
                    }
                    command_to_parse = "";
                }

                template <typename T>
                void reflectComponent(std::shared_ptr<NetworkManager> networkManager, Entity entity) {
                    try {
                        const auto &comp = networkManager->mediator->getComponent<T>(entity);
                        const auto &field_names = boost::pfr::names_as_array<typeof(comp)>();
                        old_std_out += "\t" + boost::typeindex::type_id<T>().pretty_name() + " :\n";
                        uint8_t index = 0;
                        boost::pfr::for_each_field(comp, [this, field_names, &index](const auto &field) {
                            std::ostringstream oss;
                            oss << field_names[index] << " : " << field;
                            old_std_out += "\t\t" + oss.str() + "\n";
                            index++;
                        });
                    } catch (...) {}
                }

                template <typename T>
                void setComponentValue(std::shared_ptr<NetworkManager> networkManager, Entity entity, const std::string &component_name, const std::string &field_name, std::string value) {
                    try {
                        if (component_name != boost::typeindex::type_id<T>().pretty_name())
                            return;

                        auto &comp = networkManager->mediator->getComponent<T>(entity);
                        const auto &field_names = boost::pfr::names_as_array<typeof(comp)>();
                        uint8_t index = 0;
                        boost::pfr::for_each_field(comp, [this, &field_names, &index, &field_name, &value](auto &field) {
                            if (field_names[index] == field_name) {
                                // std::stringstream(value) >> value;
                                // field = std::any_cast<decltype(field)>(value);

                                //UGLY
                                if constexpr (std::is_arithmetic_v<std::remove_reference_t<decltype(field)>>) {
                                    field = boost::lexical_cast<std::remove_reference_t<decltype(field)>>(value);
                                } else {
                                    std::cout << "Custom type assignment not implemented for this field." << std::endl;
                                }
                                return;
                            }
                            index++;
                        });
                    } catch (const std::exception &e) {
                        std::cerr << e.what() << std::endl;
                    }
                }

                bool is_open = false;
                std::string old_std_out = "";
                std::string current_command = "";
                std::string command_to_parse = "";
                uint16_t cursor_pos = 0;
                std::unordered_map<std::string, std::string> console_vars;
        };
    };
};

#endif /* !DEVCONSOLE_HPP_ */
