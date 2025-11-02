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

Level::EntityDesc parseEntityDesc(const nlohmann::json& entityJson)
{
	Level::EntityDesc entity;
	if (!entityJson.is_object()) {
		return entity;
	}
	entity.name = entityJson.value("name", std::string{});
	entity.layer = entityJson.value("layer", entity.layer);
	if (const auto compIt = entityJson.find("components"); compIt != entityJson.end() && compIt->is_object()) {
		for (const auto& [compName, compJson] : compIt->items()) {
			if (compJson.is_null()) {
				continue;
			}
			entity.componentsJson[compName] = compJson.dump();
		}
	}
	return entity;
}

std::unordered_map<std::string, std::string> parseParamObject(const nlohmann::json& json)
{
	std::unordered_map<std::string, std::string> params;
	if (!json.is_object()) {
		return params;
	}
	for (const auto& [key, value] : json.items()) {
		if (value.is_string()) {
			params[key] = value.get<std::string>();
		} else if (value.is_number_float()) {
			params[key] = std::to_string(value.get<double>());
		} else if (value.is_number_integer()) {
			params[key] = std::to_string(value.get<long long>());
		} else if (value.is_boolean()) {
			params[key] = value.get<bool>() ? "true" : "false";
		} else {
			params[key] = value.dump();
		}
	}
	return params;
}

nlohmann::json buildParamObject(const std::unordered_map<std::string, std::string>& params)
{
	nlohmann::json json = nlohmann::json::object();
	for (const auto& [key, value] : params) {
		json[key] = value;
	}
	return json;
}

bool appendEntityJson(nlohmann::json& array, const Level::EntityDesc& entity, std::string& error)
{
	nlohmann::json entityJson;
	entityJson["name"] = entity.name;
	entityJson["layer"] = entity.layer;

	nlohmann::json componentsJson = nlohmann::json::object();
	for (const auto& [componentName, componentData] : entity.componentsJson) {
		if (componentData.empty()) {
			continue;
		}
		nlohmann::json parsed;
		if (tryParseComponentJson(componentData, componentName, parsed, error)) {
			componentsJson[componentName] = parsed;
		} else {
			return false;
		}
	}

	entityJson["components"] = std::move(componentsJson);
	array.push_back(std::move(entityJson));
	return true;
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
			loaded.entities.push_back(parseEntityDesc(entityJson));
		}
	}

	loaded.waves.clear();
	if (const auto wavesIt = json.find("waves"); wavesIt != json.end() && wavesIt->is_array()) {
		for (const auto& waveJson : *wavesIt) {
			if (!waveJson.is_object()) {
				continue;
			}
			Level::Wave wave;
			wave.name = waveJson.value("name", std::string{});
			if (const auto entitiesItWave = waveJson.find("entities"); entitiesItWave != waveJson.end() && entitiesItWave->is_array()) {
				for (const auto& entityJson : *entitiesItWave) {
					wave.entities.push_back(parseEntityDesc(entityJson));
				}
			}
			loaded.waves.push_back(std::move(wave));
		}
	}

	loaded.triggers.clear();
	if (const auto triggersIt = json.find("triggers"); triggersIt != json.end() && triggersIt->is_array()) {
		for (const auto& triggerJson : *triggersIt) {
			if (!triggerJson.is_object()) {
				continue;
			}
			Level::Trigger trigger;
			trigger.type = triggerJson.value("type", std::string{});
			if (const auto paramsIt = triggerJson.find("params"); paramsIt != triggerJson.end()) {
				trigger.params = parseParamObject(*paramsIt);
			}
			if (const auto actionsIt = triggerJson.find("actions"); actionsIt != triggerJson.end() && actionsIt->is_array()) {
				for (const auto& actionJson : *actionsIt) {
					if (!actionJson.is_object()) {
						continue;
					}
					Level::TriggerAction action;
					action.type = actionJson.value("type", std::string{});
					if (const auto actionParamsIt = actionJson.find("params"); actionParamsIt != actionJson.end()) {
						action.params = parseParamObject(*actionParamsIt);
					}
					trigger.actions.push_back(std::move(action));
				}
			}
			loaded.triggers.push_back(std::move(trigger));
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
	std::string error;
	for (const auto& entity : level.entities) {
		if (!appendEntityJson(json["entities"], entity, error)) {
			return makeError(error);
		}
	}

	json["waves"] = nlohmann::json::array();
	for (const auto& wave : level.waves) {
		nlohmann::json waveJson;
		waveJson["name"] = wave.name;
		waveJson["entities"] = nlohmann::json::array();
		for (const auto& entity : wave.entities) {
			if (!appendEntityJson(waveJson["entities"], entity, error)) {
				return makeError(error);
			}
		}
		json["waves"].push_back(std::move(waveJson));
	}

	json["triggers"] = nlohmann::json::array();
	for (const auto& trigger : level.triggers) {
		nlohmann::json triggerJson;
		triggerJson["type"] = trigger.type;
		triggerJson["params"] = buildParamObject(trigger.params);
		triggerJson["actions"] = nlohmann::json::array();
		for (const auto& action : trigger.actions) {
			nlohmann::json actionJson;
			actionJson["type"] = action.type;
			actionJson["params"] = buildParamObject(action.params);
			triggerJson["actions"].push_back(std::move(actionJson));
		}
		json["triggers"].push_back(std::move(triggerJson));
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
	auto spawnEntity = [&](const Level::EntityDesc& entityDesc) -> IOResult {
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
		return makeSuccess();
	};

	for (const auto& entityDesc : level.entities) {
		auto result = spawnEntity(entityDesc);
		if (!result) {
			return result;
		}
	}

	for (const auto& wave : level.waves) {
		for (const auto& entityDesc : wave.entities) {
			auto result = spawnEntity(entityDesc);
			if (!result) {
				return result;
			}
		}
	}

	return makeSuccess();
}

} // namespace rtype::level
