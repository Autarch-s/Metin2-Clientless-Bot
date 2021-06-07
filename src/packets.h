#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace metin2_clientless_bot
{

namespace packet
{
using header_t = std::uint8_t;

enum class header_type
{
    static_type,
    dynamic_type
};

namespace client
{
class packet_info
{
  public:
    packet_info();
    ~packet_info() = default;
    void set(header_t id, header_type type, uint32_t size);

    constexpr auto get(header_t id)
    {
        return packets_.find(id) != packets_.end() ? packets_[id].get() : nullptr;
    }

  private:
    struct header_info_
    {
        header_type type_;
        uint32_t size_;
        header_info_(header_type type, uint32_t size) : type_(type), size_(size)
        {
        }

        constexpr auto get_size()
        {
            return size_;
        }
        constexpr auto get_type()
        {
            return type_;
        }
    };
    std::unordered_map<header_t, std::shared_ptr<header_info_>> packets_;
};

} // namespace client
namespace server
{

enum headers : header_t
{
    header_channel_status = 210,
    header_game_phase = 253,
    header_handshake = 255,
};

#pragma pack(push)
#pragma pack(1)

struct channel_status
{
    uint16_t port;
    uint8_t status;
};

#pragma pack(pop)

} // namespace server
} // namespace packet
} // namespace metin2_clientless_bot
