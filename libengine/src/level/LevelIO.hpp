#ifndef RTYPE_LIBENGINE_SRC_LEVEL_LEVELIO_HPP
#define RTYPE_LIBENGINE_SRC_LEVEL_LEVELIO_HPP

#include <string>

#include "Mediator.hpp"
#include "level/Level.hpp"

namespace rtype::level {

struct IOResult {
	bool success {false};
	std::string message;

	explicit operator bool() const { return success; }
};

IOResult loadFromFile(const std::string& path, Level& level);
IOResult saveToFile(const Level& level, const std::string& path);
IOResult applyLevelToWorld(const Level& level, Engine::Mediator& mediator);

} // namespace rtype::level

#endif // RTYPE_LIBENGINE_SRC_LEVEL_LEVELIO_HPP
