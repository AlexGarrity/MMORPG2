#pragma once

#include "Common/Network/ClientID.hpp"
#include "Common/Network/SerialisedComponent.hpp"
#include "SFML/System/Vector2.hpp"
#include <functional>
#include <optional>

namespace Server
{

	struct Player : public Common::Network::SerialisedComponent<Player>
	{
		sf::Vector2f position;

		auto serialise(Common::Network::MessageData& data) -> void;
		auto deserialise(Common::Network::MessageData& data) -> void;
	};

	class PlayerManager
	{
	public:
		auto createPlayer(Common::Network::ClientID clientID) -> void;
		auto destroyPlayer(Common::Network::ClientID clientID) -> void;

		auto getPlayer(Common::Network::ClientID clientID) -> std::optional<std::reference_wrapper<Player>>;
	};

	extern PlayerManager g_playerManager;

} // namespace Server