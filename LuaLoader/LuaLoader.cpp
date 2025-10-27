/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** LuaLoader
*/

#include <iostream>
#include <stdexcept>

#include "LuaLoader.hpp"
#include "Mediator.hpp"
#include "Entity.hpp"
#include "NetworkManager.hpp"
#include "Utils.hpp"
#include "Event.hpp"
#include "Components/Transform.hpp"
#include "Components/Hitbox.hpp"
#include "Components/RigidBody.hpp"
#include "Components/Sprite.hpp"
#include "Components/PlayerInfo.hpp"
#include "Components/EnemyInfo.hpp"
#include "Components/ShootingCooldown.hpp"
#include "Components/Sound.hpp"
#include "Components/Gravity.hpp"

Engine::LuaLoader::LuaLoader()
{
    lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::math, sol::lib::io);
}

void Engine::LuaLoader::setMediator(std::shared_ptr<Engine::Mediator> mediator)
{
    this->mediator = mediator;
    bindECS();
    bindUtils();
    bindEvents();
}

void Engine::LuaLoader::setNetworkManager(std::shared_ptr<Engine::NetworkManager> networkManager)
{
    this->networkManager = networkManager;
    
    if (lua["ECS"].valid()) {
        sol::table ecs_table = lua["ECS"];

        ecs_table["sendEntity"] = [this](Engine::Entity entity, sol::table signatureComponents) {
            if (!this->networkManager) {
                std::cerr << "Error: NetworkManager not available for sendEntity" << std::endl;
                return;
            }

            Engine::Signature signature;
            for (auto &pair : signatureComponents) {
                if (pair.second.is<std::string>()) {
                    std::string typeName = pair.second.as<std::string>();
                    
                    if (typeName == "Transform") {
                        signature.set(mediator->getComponentType<Engine::Components::Transform>());
                    } else if (typeName == "Hitbox") {
                        signature.set(mediator->getComponentType<Engine::Components::Hitbox>());
                    } else if (typeName == "RigidBody") {
                        signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
                    } else if (typeName == "Sprite") {
                        signature.set(mediator->getComponentType<Engine::Components::Sprite>());
                    } else if (typeName == "PlayerInfo") {
                        signature.set(mediator->getComponentType<Engine::Components::PlayerInfo>());
                    } else if (typeName == "EnemyInfo") {
                        signature.set(mediator->getComponentType<Engine::Components::EnemyInfo>());
                    } else if (typeName == "ShootingCooldown") {
                        signature.set(mediator->getComponentType<Engine::Components::ShootingCooldown>());
                    } else if (typeName == "Sound") {
                        signature.set(mediator->getComponentType<Engine::Components::Sound>());
                    } else if (typeName == "Gravity") {
                        signature.set(mediator->getComponentType<Engine::Components::Gravity>());
                    }
                }
            }
            
            this->networkManager->sendEntity(entity, signature);
        };
        
        ecs_table["getAllEntities"] = [this]() -> std::vector<Engine::Entity> {
            std::vector<Engine::Entity> entities;
            // this is so bad :(((
            for (Engine::Entity i = 0; i < MAX_ENTITIES; ++i) {
                Engine::Signature signature = mediator->getSignature(i);
                if (!signature.none()) {
                    entities.push_back(i);
                }
            }
            return entities;
        };

        ecs_table["getConnectedPlayers"] = [this]() -> int {
            if (!this->networkManager) return 0;
            return this->networkManager->getConnectedPlayers();
        };
        
        std::cout << "NetworkManager functionality bound to Lua" << std::endl;
    }
}

void Engine::LuaLoader::bindECS()
{
    if (!mediator)
        throw std::runtime_error("Warning: Attempting to bind ECS without mediator set!");

    lua.new_usertype<Engine::Entity>("Entity");

    lua["ECS"] = lua.create_table();
    sol::table ecs_table = lua["ECS"];

    ecs_table["createEntity"] = [this]() -> Engine::Entity {
        return mediator->createEntity();
    };

    ecs_table["destroyEntity"] = [this](Engine::Entity entity) {
        mediator->destroyEntity(entity);
    };

    ecs_table["getEntityCount"] = [this]() -> size_t {
        return mediator->getEntityCount();
    };

    ecs_table["getComponent"] = [this](Engine::Entity entity, const std::string& componentType) -> sol::object {
        std::cerr << "Warning: Generic getComponent called for " << componentType << " - use specific get" << componentType << " function instead" << std::endl;
        return (sol::nil);
    };

    // TODO : Use the system signature system instead of this very ugly and inefficient bad thing :(
    ecs_table["getEntitiesWithComponents"] = [this](sol::table componentTypes) -> std::vector<Engine::Entity> {
        std::vector<Engine::Entity> result;
        Engine::Signature required_signature;

        sol::table ecs_table = lua["ECS"];
        if (!ecs_table["_componentTypes"].valid()) {
            ecs_table["_componentTypes"] = lua.create_table();
        }
        sol::table registeredTypes = ecs_table["_componentTypes"];
        
        for (auto& pair : componentTypes) {
            if (pair.second.is<std::string>()) {
                std::string typeName = pair.second.as<std::string>();

                if (registeredTypes[typeName].valid()) {
                    sol::function getComponentType = registeredTypes[typeName];
                    Engine::ComponentType componentType = getComponentType();
                    required_signature.set(componentType);
                } else {
                    std::cerr << "Warning: Component type '" << typeName << "' not dynamically registered, using fallback" << std::endl;
                    
                    if (typeName == "Transform") {
                        required_signature.set(mediator->getComponentType<Engine::Components::Transform>());
                    } else if (typeName == "Hitbox") {
                        required_signature.set(mediator->getComponentType<Engine::Components::Hitbox>());
                    } else if (typeName == "RigidBody") {
                        required_signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
                    } else if (typeName == "Sprite") {
                        required_signature.set(mediator->getComponentType<Engine::Components::Sprite>());
                    } else if (typeName == "PlayerInfo") {
                        required_signature.set(mediator->getComponentType<Engine::Components::PlayerInfo>());
                    } else if (typeName == "EnemyInfo") {
                        required_signature.set(mediator->getComponentType<Engine::Components::EnemyInfo>());
                    } else if (typeName == "ShootingCooldown") {
                        required_signature.set(mediator->getComponentType<Engine::Components::ShootingCooldown>());
                    } else if (typeName == "Sound") {
                        required_signature.set(mediator->getComponentType<Engine::Components::Sound>());
                    } else if (typeName == "Gravity") {
                        required_signature.set(mediator->getComponentType<Engine::Components::Gravity>());
                    } else {
                        std::cerr << "Error: Unknown component type in getEntitiesWithComponents: " << typeName << std::endl;
                    }
                }
            }
        }
        
        for (Engine::Entity entity = 0; entity < MAX_ENTITIES; entity++) {
            Engine::Signature entity_signature = mediator->getSignature(entity);

            if (entity_signature.none()) {
                continue;
            }

            if ((entity_signature & required_signature) == required_signature) {
                result.push_back(entity);
            }
        }
        
        return (result);
    };

    ecs_table["createSignature"] = [this]() -> sol::table {
        sol::table signature = lua.create_table();
        signature["componentTypes"] = lua.create_table();
        return (signature);
    };

    ecs_table["addComponentToSignature"] = [](sol::table signature, const std::string& componentType) {
        sol::table componentTypes = signature["componentTypes"];
        componentTypes[componentType] = true;
    };

    ecs_table["runSystem"] = [](sol::function luaSystem, sol::table componentTypes) {
        luaSystem(componentTypes);
    };

    std::cout << "ECS functionality bound to Lua" << std::endl;
}

void Engine::LuaLoader::bindEvents()
{
    std::cout << "Binding event system to Lua..." << std::endl;

    sol::usertype<Engine::Event> event_type = lua.new_usertype<Engine::Event>("Event");
    
    event_type[sol::call_constructor] = [](Engine::EventId id) {
        std::cout << "Creating Event with ID: " << id << std::endl;
        return Engine::Event(id);
    };
    
    event_type["getType"] = &Engine::Event::getType;
    
    event_type["setParamInt"] = [](Engine::Event& event, Engine::EventId id, int value) {
        event.setParam(id, value);
    };
    
    event_type["setParamFloat"] = [](Engine::Event& event, Engine::EventId id, float value) {
        event.setParam(id, value);
    };
    
    event_type["setParamEntity"] = [](Engine::Event& event, Engine::EventId id, Engine::Entity value) {
        event.setParam(id, value);
    };
    
    event_type["setParamHitboxLayer"] = [](Engine::Event& event, Engine::EventId id, int layerValue) {
        event.setParam(id, static_cast<HITBOX_LAYERS>(layerValue));
    };
    
    event_type["setParamIntById"] = [](Engine::Event& event, int id, int value) {
        event.setParam(static_cast<Engine::EventId>(id), value);
    };
    
    event_type["setParamEntityById"] = [](Engine::Event& event, int id, Engine::Entity value) {
        event.setParam(static_cast<Engine::EventId>(id), value);
    };
    
    event_type["setParamHitboxLayerById"] = [](Engine::Event& event, int id, int layerValue) {
        event.setParam(static_cast<Engine::EventId>(id), static_cast<HITBOX_LAYERS>(layerValue));
    };
    
    event_type["getParamInt"] = [](Engine::Event& event, Engine::EventId id) -> int {
        return event.getParam<int>(id);
    };
    
    event_type["getParamFloat"] = [](Engine::Event& event, Engine::EventId id) -> float {
        return event.getParam<float>(id);
    };
    
    event_type["getParamEntity"] = [](Engine::Event& event, Engine::EventId id) -> Engine::Entity {
        return event.getParam<Engine::Entity>(id);
    };
    
    event_type["getParamIntById"] = [](Engine::Event& event, int id) -> int {
        return event.getParam<int>(static_cast<Engine::EventId>(id));
    };
    
    event_type["getParamEntityById"] = [](Engine::Event& event, int id) -> Engine::Entity {
        return event.getParam<Engine::Entity>(static_cast<Engine::EventId>(id));
    };

    lua["EventsIds"] = lua.create_table();
    sol::table events_table = lua["EventsIds"];
    events_table["PLAYER_INPUT"] = static_cast<Engine::EventId>(Engine::EventsIds::PLAYER_INPUT);
    events_table["COLLISION"] = static_cast<Engine::EventId>(Engine::EventsIds::COLLISION);
    events_table["ENEMY_DESTROYED"] = static_cast<Engine::EventId>(Engine::EventsIds::ENEMY_DESTROYED);
    events_table["PLAYER_HIT"] = static_cast<Engine::EventId>(Engine::EventsIds::PLAYER_HIT);
    events_table["DEVCONSOLE_KEY_PRESSED"] = static_cast<Engine::EventId>(Engine::EventsIds::DEVCONSOLE_KEY_PRESSED);
    events_table["DEVCONSOLE_TEXT_ENTERED"] = static_cast<Engine::EventId>(Engine::EventsIds::DEVCONSOLE_TEXT_ENTERED);

    std::cout << "EventsIds bound. COLLISION ID = " << static_cast<Engine::EventId>(Engine::EventsIds::COLLISION) << std::endl;

    lua["HITBOX_LAYERS"] = lua.create_table();
    sol::table hitbox_layers_table = lua["HITBOX_LAYERS"];
    hitbox_layers_table["PLAYER"] = static_cast<int>(HITBOX_LAYERS::PLAYER);
    hitbox_layers_table["PLAYER_PROJECTILE"] = static_cast<int>(HITBOX_LAYERS::PLAYER_PROJECTILE);
    hitbox_layers_table["ENEMY"] = static_cast<int>(HITBOX_LAYERS::ENEMY);
    hitbox_layers_table["ENEMY_PROJECTILE"] = static_cast<int>(HITBOX_LAYERS::ENEMY_PROJECTILE);

    std::cout << "HITBOX_LAYERS bound. PLAYER=" << static_cast<int>(HITBOX_LAYERS::PLAYER) 
              << ", PLAYER_PROJECTILE=" << static_cast<int>(HITBOX_LAYERS::PLAYER_PROJECTILE)
              << ", ENEMY=" << static_cast<int>(HITBOX_LAYERS::ENEMY) 
              << ", ENEMY_PROJECTILE=" << static_cast<int>(HITBOX_LAYERS::ENEMY_PROJECTILE) << std::endl;

    if (lua["ECS"].valid()) {
        sol::table ecs_table = lua["ECS"];

        ecs_table["sendEvent"] = [this](Engine::Event& event) {
            if (!this->mediator) {
                std::cerr << "Error: Mediator not available for sendEvent" << std::endl;
                return;
            }
            std::cout << "Lua sending event with type: " << event.getType() << std::endl;
            this->mediator->sendEvent(event);
            std::cout << "Event sent through mediator successfully" << std::endl;
        };
        
        ecs_table["sendEventById"] = [this](Engine::EventId eventId) {
            if (!this->mediator) {
                std::cerr << "Error: Mediator not available for sendEventById" << std::endl;
                return;
            }
            this->mediator->sendEvent(eventId);
        };
        
        ecs_table["addEventListener"] = [this](Engine::EventId eventId, sol::function luaCallback) {
            if (!this->mediator) {
                std::cerr << "Error: Mediator not available for addEventListener" << std::endl;
                return;
            }
            
            auto callback = [luaCallback](Engine::Event& event) {
                try {
                    luaCallback(event);
                } catch (const sol::error& e) {
                    std::cerr << "Error in Lua event callback: " << e.what() << std::endl;
                }
            };
            
            this->mediator->addEventListener(eventId, callback);
        };
        
        std::cout << "Event system bound to ECS table" << std::endl;
    }

    std::cout << "Event system binding completed" << std::endl;
}

void Engine::LuaLoader::bindUtils()
{
    std::cout << "Binding utility classes to Lua..." << std::endl;

    if (lua["Vec2"].valid()) {
        std::cout << "Vec2 already exists in Lua state" << std::endl;
    } else {
        std::cout << "Vec2 not yet bound, creating new binding" << std::endl;
    }

    sol::usertype<Engine::Utils::Vec2> vec2_type = lua.new_usertype<Engine::Utils::Vec2>("Vec2");
    
    vec2_type[sol::call_constructor] = sol::factories(
        []() { return Engine::Utils::Vec2(); },
        [](float x, float y) { return Engine::Utils::Vec2(x, y); }
    );
    
    vec2_type["x"] = &Engine::Utils::Vec2::x;
    vec2_type["y"] = &Engine::Utils::Vec2::y;

    vec2_type["magnitude"] = [](const Engine::Utils::Vec2 &v) -> float {
        return std::sqrt(v.x * v.x + v.y * v.y);
    };
    
    vec2_type["normalize"] = [](const Engine::Utils::Vec2 &v) -> Engine::Utils::Vec2 {
        float len = std::sqrt(v.x * v.x + v.y * v.y);
        if (len > 0.0f) {
            return Engine::Utils::Vec2(v.x / len, v.y / len);
        }
        return Engine::Utils::Vec2(0.0f, 0.0f);
    };
    
    vec2_type["toString"] = &Engine::Utils::Vec2::toString;

    vec2_type[sol::meta_function::addition] = [](const Engine::Utils::Vec2 &a, const Engine::Utils::Vec2 &b) {
        return Engine::Utils::Vec2(a.x + b.x, a.y + b.y);
    };
    
    vec2_type[sol::meta_function::subtraction] = [](const Engine::Utils::Vec2 &a, const Engine::Utils::Vec2 &b) {
        return Engine::Utils::Vec2(a.x - b.x, a.y - b.y);
    };
    
    vec2_type[sol::meta_function::multiplication] = [](const Engine::Utils::Vec2 &v, float f) {
        return Engine::Utils::Vec2(v.x * f, v.y * f);
    };

    vec2_type["distance"] = [](const Engine::Utils::Vec2 &a, const Engine::Utils::Vec2 &b) -> float {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    };
    
    vec2_type["dot"] = [](const Engine::Utils::Vec2 &a, const Engine::Utils::Vec2 &b) -> float {
        return a.x * b.x + a.y * b.y;
    };
    
    std::cout << "Vec2 binding completed" << std::endl;

    sol::usertype<Engine::Utils::Vec2Int> vec2int_type = lua.new_usertype<Engine::Utils::Vec2Int>("Vec2Int");
    
    vec2int_type[sol::call_constructor] = sol::factories(
        []() { return Engine::Utils::Vec2Int(); },
        [](int x, int y) { return Engine::Utils::Vec2Int(x, y); }
    );
    
    vec2int_type["x"] = &Engine::Utils::Vec2Int::x;
    vec2int_type["y"] = &Engine::Utils::Vec2Int::y;
    vec2int_type["toString"] = &Engine::Utils::Vec2Int::toString;

    vec2int_type[sol::meta_function::addition] = [](const Engine::Utils::Vec2Int &a, const Engine::Utils::Vec2Int &b) {
        return Engine::Utils::Vec2Int(a.x + b.x, a.y + b.y);
    };
    
    vec2int_type[sol::meta_function::subtraction] = [](const Engine::Utils::Vec2Int &a, const Engine::Utils::Vec2Int &b) {
        return Engine::Utils::Vec2Int(a.x - b.x, a.y - b.y);
    };
    
    vec2int_type[sol::meta_function::multiplication] = [](const Engine::Utils::Vec2Int &v, int f) {
        return Engine::Utils::Vec2Int(v.x * f, v.y * f);
    };

    sol::usertype<Engine::Utils::Rect> rect_type = lua.new_usertype<Engine::Utils::Rect>("Rect");
    
    rect_type[sol::call_constructor] = sol::factories(
        []() { return Engine::Utils::Rect(); },
        [](float x, float y, float width, float height) { return Engine::Utils::Rect(x, y, width, height); }
    );
    
    rect_type["x"] = &Engine::Utils::Rect::x;
    rect_type["y"] = &Engine::Utils::Rect::y;
    rect_type["width"] = &Engine::Utils::Rect::width;
    rect_type["height"] = &Engine::Utils::Rect::height;
    rect_type["toString"] = &Engine::Utils::Rect::toString;

    rect_type["contains"] = sol::overload(
        [](const Engine::Utils::Rect& rect, float x, float y) {
            return rect.contains(x, y);
        },
        [](const Engine::Utils::Rect& rect, const Engine::Utils::Vec2& point) {
            return rect.contains(point);
        }
    );
    
    rect_type["intersects"] = &Engine::Utils::Rect::intersects;

    rect_type["getCenter"] = [](const Engine::Utils::Rect& rect) -> Engine::Utils::Vec2 {
        return Engine::Utils::Vec2(rect.x + rect.width / 2.0f, rect.y + rect.height / 2.0f);
    };
    
    rect_type["getArea"] = [](const Engine::Utils::Rect& rect) -> float {
        return rect.width * rect.height;
    };
    
    std::cout << "Utility classes bound to Lua" << std::endl;
}

void Engine::LuaLoader::executeScript(const std::string &scriptPath)
{
    try {
        lua.script_file(scriptPath);
        std::cout << "Successfully executed Lua script: " << scriptPath << std::endl;
    } catch (const sol::error &e) {
        std::cerr << "Lua script error in " << scriptPath << ": " << e.what() << std::endl;
    }
}

void Engine::LuaLoader::executeScriptString(const std::string &script)
{
    try {
        lua.script(script);
        std::cout << "Successfully executed Lua script string" << std::endl;
    } catch (const sol::error &e) {
        std::cerr << "Lua script string error: " << e.what() << std::endl;
    }
}

void Engine::LuaLoader::executeLuaFunction(const std::string &functionName)
{
    try {
        if (lua[functionName].valid()) {
            sol::function luaFunc = lua[functionName];
            luaFunc();
        } else {
            std::cerr << "Lua function '" << functionName << "' not found" << std::endl;
        }
    } catch (const sol::error &e) {
        std::cerr << "Lua function execution error for '" << functionName << "': " << e.what() << std::endl;
    }
}

Engine::LuaLoader::~LuaLoader()
{
}
