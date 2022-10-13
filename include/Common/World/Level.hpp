#pragma once

#include "Common/World/Tile.hpp"
#include <array>

namespace Common::World
{

	const auto LEVEL_WIDTH  = std::size_t(256);
	const auto LEVEL_HEIGHT = std::size_t(256);

	class Level
	{
	public:
		Level();

		auto setTile(std::size_t xPosition, std::size_t yPosition, Tile value) -> void;
		[[nodiscard]] auto getTile(std::size_t xPosition, std::size_t yPosition) const -> Tile;

	private:
		std::array<Tile, LEVEL_WIDTH * LEVEL_HEIGHT> m_data;
	};

} // namespace Common::World