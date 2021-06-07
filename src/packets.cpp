#include "packets.h"
#include <stdexcept>

namespace metin2_clientless_bot
{
namespace packet
{
namespace client
{
packet_info::packet_info() : packets_()
{
    set(headers::header_server_status, header_type::static_type, sizeof(server_status));
}

void packet_info::set(header_t id, header_type type, uint32_t size)
{
    if (packets_.find(id) != packets_.end())
        throw std::logic_error("packet id " + id + std::string(" already exists!"));

    packets_.emplace(id, std::make_shared<header_info_>(type, size));
}
} // namespace client
namespace server
{
} // namespace server
} // namespace packet
} // namespace metin2_clientless_bot
