#pragma once

#include <cstdint>
#include <vector>


namespace Common::Network
{

	class MessageData
	{
	public:
		MessageData();

		auto operator<<(bool value) -> MessageData&;
		auto operator<<(std::uint8_t value) -> MessageData&;
		auto operator<<(std::uint16_t value) -> MessageData&;
		auto operator<<(std::uint32_t value) -> MessageData&;
		auto operator<<(std::uint64_t value) -> MessageData&;
		auto operator<<(float value) -> MessageData&;
		auto operator<<(double value) -> MessageData&;

		auto operator>>(bool& value) -> MessageData&;
		auto operator>>(std::uint8_t& value) -> MessageData&;
		auto operator>>(std::uint16_t& value) -> MessageData&;
		auto operator>>(std::uint32_t& value) -> MessageData&;
		auto operator>>(std::uint64_t& value) -> MessageData&;
		auto operator>>(float& value) -> MessageData&;
		auto operator>>(double& value) -> MessageData&;

		[[nodiscard]] auto size() const -> std::size_t;
		[[nodiscard]] auto data() const -> const void*;
		[[nodiscard]] auto data() -> void*;
		auto resize(std::size_t newSize) -> void;

	private:
		std::vector<std::uint8_t> m_data;
		std::size_t m_readHead;
	};

} // namespace Common::Network