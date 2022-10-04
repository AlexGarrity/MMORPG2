#pragma once

#include "Client.hpp"
#include "Common/Network/ClientID.hpp"
#include "Common/Network/MessageQueue.hpp"
#include "Message.hpp"
#include "SFML/Network/IpAddress.hpp"
#include "SFML/Network/SocketSelector.hpp"
#include "SFML/Network/TcpListener.hpp"
#include "SFML/Network/UdpSocket.hpp"
#include <list>
#include <unordered_map>

namespace Server
{

	class NetworkManager
	{
	public:
		auto init() -> void;
		auto shutdown() -> void;

		auto update() -> void;

		auto getNextUDPMessage() -> std::optional<Message>;
		auto getNextTCPMessage() -> std::optional<Message>;

		auto pushUDPMessage(Message&& message) -> void;
		auto pushTCPMessage(Message&& message) -> void;

		auto resolveClientID(sf::IpAddress ipAddress) -> std::optional<Common::Network::ClientID>;
		auto setClientUdpPort(Common::Network::ClientID clientID, std::uint16_t udpPort) -> void;
		auto markForDisconnect(Common::Network::ClientID clientID) -> void;

	private:
		auto generateClientID() -> Common::Network::ClientID;
		auto acceptNewConnection() -> void;
		auto closeConnection(Common::Network::ClientID clientID) -> void;
		auto disconnectClients() -> void;

		auto sendUDP() -> void;
		auto receiveUDP() -> void;

		auto sendTCP() -> void;
		auto receiveTCP(Common::Network::ClientID clientID, Client& client) -> void;

		sf::SocketSelector m_socketSelector;
		sf::TcpListener m_tcpListener;
		sf::UdpSocket m_udpSocket;

		std::unordered_map<Common::Network::ClientID, Client, Common::Network::ClientIDHash> m_clientList;
		std::unordered_map<std::uint32_t, Common::Network::ClientID> m_clientIPMap;
		std::list<Common::Network::ClientID> m_clientsPendingDisconnection;

		Common::Network::ClientID_t m_nextClientID;

		Common::Network::MessageQueue<Message> m_tcpQueue;
		Common::Network::MessageQueue<Message> m_udpQueue;
	};

	extern NetworkManager g_networkManager;

} // namespace Server