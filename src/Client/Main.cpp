#include "Common/Network/ClientID.hpp"
#include "Common/Network/MessageType.hpp"
#include "Common/Network/ServerProperties.hpp"
#include "NetworkManager.hpp"
#include "SFML/System/Vector2.hpp"
#include "Version.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <vector>

const unsigned int WINDOW_WIDTH  = 1280;
const unsigned int WINDOW_HEIGHT = 720;
const unsigned int BIT_DEPTH     = 32;

auto sendPlayerInput()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P))
	{
		sf::Packet packet;
		packet << Common::Network::MessageType::Terminate;
		Client::g_networkManager.pushTCPMessage(std::move(packet));
		return;
	}

	sf::Vector2f playerDelta{0.0F, 0.0F};

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
	{
		playerDelta.x -= 1.0F;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
	{
		playerDelta.x += 1.0F;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
	{
		playerDelta.y -= 1.0F;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
	{
		playerDelta.y += 1.0F;
	}

	if (playerDelta.lengthSq() > 0.0F)
	{
		sf::Packet packet;
		packet << Common::Network::MessageType::Movement << playerDelta.x << playerDelta.y;
		Client::g_networkManager.pushUDPMessage(std::move(packet));
	}
}

auto parseUDP(std::unordered_map<Common::Network::ClientID, sf::Sprite, Common::Network::ClientIDHash>& sprites, sf::Texture& playerTexture) -> void
{
	std::optional<sf::Packet> optPacket;
	while ((optPacket = Client::g_networkManager.getNextUDPMessage()).has_value())
	{
		auto& packet = optPacket.value();

		Common::Network::MessageType messageType = Common::Network::MessageType::None;
		packet >> messageType;

		switch (messageType)
		{
			case Common::Network::MessageType::Position:
			{
				Common::Network::ClientID_t clientID = -1;
				packet >> clientID;

				auto iterator = sprites.find(Common::Network::ClientID(clientID));
				if (iterator == sprites.end())
				{
					auto [pair, success] = sprites.emplace(clientID, sf::Sprite{});
					pair->second.setTexture(playerTexture);
					iterator = pair;
				}

				sf::Vector2f position;
				packet >> position.x >> position.y;
				iterator->second.setPosition(position);
			}
			break;
			case Common::Network::MessageType::CreateEntity:
			{
				Common::Network::ClientID_t clientID = -1;
				packet >> clientID;
				if (clientID == -1)
				{
					// This probably wasn't supposed to happen
					break;
				}

				auto iterator = sprites.find(Common::Network::ClientID(clientID));
				if (iterator == sprites.end())
				{
					auto [pair, success] = sprites.emplace(clientID, sf::Sprite{});
					pair->second.setTexture(playerTexture);
				}
			}
			break;
			case Common::Network::MessageType::DestroyEntity:
			{
				Common::Network::ClientID_t clientID = -1;
				packet >> clientID;
				sprites.erase(Common::Network::ClientID(clientID));
			}
			break;
			default:
				// Do nothing
				break;
		}
	}
}

auto main(int /* argc */, char** argv) -> int
{
#if !defined(NDEBUG)
	spdlog::set_level(spdlog::level::debug);
#endif

	spdlog::info("Client version {}.{}.{}", Client::Version::getMajor(), Client::Version::getMinor(), Client::Version::getPatch());

	auto executablePath = std::filesystem::path(*argv);
	std::filesystem::current_path(executablePath.parent_path());

	spdlog::debug("Initialising the network manager");
	Client::g_networkManager.init();

	std::unordered_map<Common::Network::ClientID, sf::Sprite, Common::Network::ClientIDHash> sprites;

	sf::RenderWindow renderWindow;
	renderWindow.create(sf::VideoMode{sf::Vector2u{WINDOW_WIDTH, WINDOW_HEIGHT}, BIT_DEPTH}, "Client");
	renderWindow.setVerticalSyncEnabled(true);

	sf::Clock clock;
	clock.restart();

	sf::Texture playerTexture;
	if (!playerTexture.loadFromFile("assets/player.png"))
	{
		return 2;
	}

	Client::g_networkManager.connect();

	bool windowShouldClose = false;
	while (!windowShouldClose)
	{
		// Get the time elapsed since the previous frame
		float deltaTime = clock.restart().asSeconds();

		// Check if the window is closed
		sf::Event event{};
		while (renderWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				windowShouldClose = true;
			}
		}


		Client::g_networkManager.update();

		sendPlayerInput();
		parseUDP(sprites, playerTexture);

		// Render all sprites
		renderWindow.clear();
		for (auto& [id, sprite] : sprites)
		{
			renderWindow.draw(sprite);
		}
		renderWindow.display();
	}

	Client::g_networkManager.shutdown();

	return 0;
}