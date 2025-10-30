/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ComponentRegistry.hpp
*/

#ifndef COMPONENTREGISTRY_HPP
#define COMPONENTREGISTRY_HPP

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace rtype::editor {

struct FieldSpec {
    enum class Kind { F32, I32, Bool, Str };

    std::string key;
    Kind kind;
    float minF {0.f};
    float maxF {0.f};
    float stepF {1.f};
    int minI {0};
    int maxI {0};
    int stepI {1};
};

struct ComponentSpec {
    std::string name;
    std::vector<FieldSpec> fields;
};

const std::vector<ComponentSpec>& specs();
const ComponentSpec* findSpec(const std::string& name);

} // namespace rtype::editor

#endif // COMPONENTREGISTRY_HPP
