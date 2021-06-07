#pragma once

#include <cstdint>

namespace packet
{
using header_t = std::uint8_t;
namespace client
{
enum headers : header_t
{
    header_server_status = 206,
};

struct server_status
{
    header_t header;
    server_status() : header(header_server_status)
    {
    }
};

} // namespace client
namespace server
{

} // namespace server
} // namespace packet
