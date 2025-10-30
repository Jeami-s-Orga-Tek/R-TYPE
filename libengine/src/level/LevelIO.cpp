#include "level/LevelIO.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <system_error>
#include <utility>

#include <nlohmann/json.hpp>

#include "Components/Sprite.hpp"
#include "Components/Transform.hpp"

namespace rtype::level {

namespace {

constexpr std::size_t kSpriteNameCapacity = 255;

IOResult makeSuccess()
{
	return IOResult{true, {}};
}

IOResult makeError(std::string message)
{
	return IOResult{false, std::move(message)};
}

Engine::Components::Transform buildTransform(const nlohmann::json& json)
{
	Engine::Components::Transform transform {};
	transform.pos.x = json.value("x", 0.f);
	transform.pos.y = json.value("y", 0.f);
	transform.pos.width = json.value("w", 0.f);
	transform.pos.height = json.value("h", 0.f);
	transform.rot = json.value("rot", 0.f);
	transform.scale = json.value("scale", 1.f);
	return transform;
}

Engine::Components::Sprite buildSprite(const nlohmann::json& json)
{
	Engine::Components::Sprite sprite {};
	sprite.sprite_name.fill('\0');
	const auto name = json.value("name", std::string{});
	std::strncpy(sprite.sprite_name.data(), name.c_str(), kSpriteNameCapacity - 1);
	sprite.frame_nb = json.value("frame_nb", 1u);
	sprite.scrolling = json.value("scrolling", false);
	sprite.is_background = json.value("is_background", false);
	return sprite;
}

bool tryParseComponentJson(const std::string& raw, const std::string& componentName, nlohmann::json& outJson, std::string& error)
{
	try {
		outJson = nlohmann::json::parse(raw);
		return true;
	} catch (const std::exception& e) {
		error = "Invalid JSON for component '" + componentName + "': " + e.what();
		return false;
	}
}

} // namespace

IOResult loadFromFile(const std::string& path, Level& level)
{
	std::ifstream input(path);
	if (!input.is_open()) {
		return makeError("Unable to open level file: " + path);
	}

	nlohmann::json json;
	try {
		input >> json;
	} catch (const std::exception& e) {
		return makeError(std::string{"Invalid level JSON: "} + e.what());
	}

	Level loaded;
	loaded.version = json.value("version", loaded.version);

	if (const auto metaIt = json.find("meta"); metaIt != json.end() && metaIt->is_object()) {
		loaded.meta.name = metaIt->value("name", loaded.meta.name);
		loaded.meta.author = metaIt->value("author", loaded.meta.author);
		loaded.meta.description = metaIt->value("description", loaded.meta.description);
	}

	if (const auto gridIt = json.find("grid"); gridIt != json.end() && gridIt->is_object()) {
		loaded.grid.size = gridIt->value("size", loaded.grid.size);
		loaded.grid.snap = gridIt->value("snap", loaded.grid.snap);
	}

	if (const auto layersIt = json.find("layers"); layersIt != json.end() && layersIt->is_array()) {
		loaded.layers.clear();
		for (const auto& entry : *layersIt) {
			if (entry.is_string()) {
				loaded.layers.push_back(entry.get<std::string>());
			}
		}
	}
	if (loaded.layers.empty()) {
		loaded.layers = {"background", "gameplay", "foreground"};
	}

	loaded.entities.clear();
	if (const auto entitiesIt = json.find("entities"); entitiesIt != json.end() && entitiesIt->is_array()) {
		for (const auto& entityJson : *entitiesIt) {
			if (!entityJson.is_object()) {
				continue;
			}
			Level::EntityDesc entity;
			entity.name = entityJson.value("name", std::string{});
			entity.layer = entityJson.value("layer", entity.layer);
			if (const auto compIt = entityJson.find("components"); compIt != entityJson.end() && compIt->is_object()) {
				for (const auto& [compName, compJson] : compIt->items()) {
					entity.componentsJson[compName] = compJson.dump();
				}
			}
			loaded.entities.push_back(std::move(entity));
		}
	}

	level = std::move(loaded);
	return makeSuccess();
}

IOResult saveToFile(const Level& level, const std::string& path)
{
	const std::filesystem::path targetPath(path);
	std::error_code ec;
	if (!targetPath.parent_path().empty()) {
		std::filesystem::create_directories(targetPath.parent_path(), ec);
		if (ec) {
			return makeError("Unable to create directory '" + targetPath.parent_path().string() + "': " + ec.message());
		}
	}

	nlohmann::json json;
	json["version"] = level.version;
	json["meta"] = {
		{"name", level.meta.name},
		{"author", level.meta.author},
		{"description", level.meta.description}
	};
	json["grid"] = {
		{"size", level.grid.size},
		{"snap", level.grid.snap}
	};

	json["layers"] = nlohmann::json::array();
	for (const auto& layer : level.layers) {
		json["layers"].push_back(layer);
	}

	json["entities"] = nlohmann::json::array();
	for (const auto& entity : level.entities) {
		nlohmann::json entityJson;
		entityJson["name"] = entity.name;
		entityJson["layer"] = entity.layer;

		nlohmann::json componentsJson = nlohmann::json::object();
		for (const auto& [componentName, componentData] : entity.componentsJson) {
			if (componentData.empty()) {
				continue;
			}
			std::string error;
			nlohmann::json parsed;
			if (tryParseComponentJson(componentData, componentName, parsed, error)) {
				componentsJson[componentName] = parsed;
			} else {
				return makeError(error);
			}
		}

		entityJson["components"] = std::move(componentsJson);
		json["entities"].push_back(std::move(entityJson));
	}

	std::ofstream output(path);
	if (!output.is_open()) {
		return makeError("Unable to write level file: " + path);
	}

	output << std::setw(2) << json;
	if (!output.good()) {
		return makeError("Failed to write level file: " + path);
	}

	return makeSuccess();
}

IOResult applyLevelToWorld(const Level& level, Engine::Mediator& mediator)
{
	for (const auto& entityDesc : level.entities) {
		const Engine::Entity entity = mediator.createEntity();

		for (const auto& [componentName, componentData] : entityDesc.componentsJson) {
			if (componentData.empty()) {
				continue;
			}

			std::string error;
			nlohmann::json parsed;
			if (!tryParseComponentJson(componentData, componentName, parsed, error)) {
				return makeError(error);
			}

			if (componentName == "Transform") {
				mediator.addComponent(entity, buildTransform(parsed));
			} else if (componentName == "Sprite") {
				mediator.addComponent(entity, buildSprite(parsed));
			}
		}
	}

	return makeSuccess();
}

} // namespace rtype::level
