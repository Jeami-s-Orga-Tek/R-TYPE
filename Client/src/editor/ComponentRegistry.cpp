/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ComponentRegistry.cpp
*/

#include "editor/ComponentRegistry.hpp"

#include <algorithm>

namespace rtype::editor {

namespace {

FieldSpec makeFloatField(std::string key, float min, float max, float step)
{
    FieldSpec spec;
    spec.key = std::move(key);
    spec.kind = FieldSpec::Kind::F32;
    spec.minF = min;
    spec.maxF = max;
    spec.stepF = step;
    return spec;
}

FieldSpec makeStringField(std::string key)
{
    FieldSpec spec;
    spec.key = std::move(key);
    spec.kind = FieldSpec::Kind::Str;
    return spec;
}

FieldSpec makeBoolField(std::string key)
{
    FieldSpec spec;
    spec.key = std::move(key);
    spec.kind = FieldSpec::Kind::Bool;
    return spec;
}

} // namespace

const std::vector<ComponentSpec>& specs()
{
    static const std::vector<ComponentSpec> kSpecs = {
        ComponentSpec{
            "Transform",
            {
                makeFloatField("x", -10000.f, 10000.f, 1.f),
                makeFloatField("y", -10000.f, 10000.f, 1.f),
                makeFloatField("rot", -360.f, 360.f, 1.f),
                makeFloatField("scale", 0.1f, 10.f, 0.1f),
            }
        },
        ComponentSpec{
            "Sprite",
            {
                makeStringField("name"),
                makeBoolField("is_background"),
                makeBoolField("scrolling"),
                makeFloatField("frame_nb", 0.f, 1000.f, 1.f),
            }
        },
        ComponentSpec{
            "Hitbox",
            {
                makeFloatField("w", 0.f, 1000.f, 1.f),
                makeFloatField("h", 0.f, 1000.f, 1.f),
                makeBoolField("active"),
                makeStringField("layer"),
                makeFloatField("damage", 0.f, 1000.f, 1.f),
            }
        },
        ComponentSpec{
            "Gravity",
            {
                makeFloatField("force_x", -1000.f, 1000.f, 0.1f),
                makeFloatField("force_y", -1000.f, 1000.f, 0.1f),
                makeFloatField("density", 0.f, 100.f, 0.1f),
                makeFloatField("friction", 0.f, 1.f, 0.01f),
                makeFloatField("restitution", 0.f, 1.f, 0.01f),
            }
        },
        ComponentSpec{
            "Animation",
            {
                makeFloatField("total_frames", 0.f, 100.f, 1.f),
                makeFloatField("pause", 0.f, 10.f, 0.01f),
                makeFloatField("pause_timer", 0.f, 10.f, 0.01f),
                makeBoolField("is_playing"),
                makeBoolField("destroy_at_end"),
                makeBoolField("looping"),
            }
        },
        ComponentSpec{
            "RigidBody",
            {
                makeFloatField("velocity_x", -10000.f, 10000.f, 1.f),
                makeFloatField("velocity_y", -10000.f, 10000.f, 1.f),
                makeFloatField("acceleration_x", -10000.f, 10000.f, 1.f),
                makeFloatField("acceleration_y", -10000.f, 10000.f, 1.f),
                makeBoolField("has_body_been_created"),
            }
        },
        ComponentSpec{
            "Sound",
            {
                makeStringField("sound_name"),
                makeBoolField("looping"),
                makeBoolField("has_played"),
            }
        },
        ComponentSpec{
            "EnemyInfo",
            {
                makeFloatField("health", 0.f, 10000.f, 1.f),
                makeFloatField("maxHealth", 0.f, 10000.f, 1.f),
                makeFloatField("type", 0.f, 10.f, 1.f),
                makeFloatField("scoreValue", 0.f, 10000.f, 1.f),
                makeFloatField("speed", 0.f, 10000.f, 0.1f),
                makeBoolField("isActive"),
            }
        },
        ComponentSpec{
            "ShootingCooldown",
            {
                makeFloatField("cooldown_time", 0.f, 10000.f, 1.f),
                makeFloatField("cooldown", 0.f, 10000.f, 1.f),
            }
        },
        ComponentSpec{
            "PlayerInfo",
            {
                makeFloatField("player_id", 0.f, 10000.f, 1.f),
            }
        },
    };

    return kSpecs;
}

const ComponentSpec* findSpec(const std::string& name)
{
    const auto& list = specs();
    auto it = std::find_if(list.begin(), list.end(), [&](const ComponentSpec& spec) { return spec.name == name; });
    if (it == list.end()) {
        return nullptr;
    }
    return &(*it);
}

} // namespace rtype::editor
