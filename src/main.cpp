#include "packets.h"
#include <cassert>
#include <chrono>
#include <csignal>
#include <cxxopts.hpp>
#include <iostream>
#include <sockpp/tcp_connector.h>
#include <sockpp/version.h>
#include <thread>

using namespace metin2_clientless_bot;

static bool parse_command_line(int argc, char **&argv)
{
    try
    {
        cxxopts::Options options("Metin2-Clientless-Bot");
        options.positional_help("[optional args]").show_positional_help();
        options.allow_unrecognised_options().add_options()("i, ip", "-i <127.0.0.1>", cxxopts::value<std::string>())(
            "p, port", "-p <12345>", cxxopts::value<std::uint16_t>())("h, help", "Print help");

        const auto result = options.parse(argc, argv);
        const auto &arguments = result.arguments();

        if (arguments.empty() || result.count("help"))
        {
            std::cout << options.help() << std::endl;
            return true;
        }

        const auto ip = result.count("ip");
        const auto port = result.count("port");

        if (ip && port)
        {
            const auto &ip_arg = result["ip"].as<std::string>();
            const auto port_arg = result["port"].as<std::uint16_t>();

            sockpp::tcp_connector conn({ip_arg, port_arg});
            if (!conn)
            {
                std::cerr << "Error connecting to server at " << sockpp::inet_address(ip_arg, port_arg) << "\n"
                          << conn.last_error_str() << std::endl;
                return false;
            }

            std::cout << "Created a connection from " << conn.address() << std::endl;

            if (!conn.read_timeout(std::chrono::seconds(5)))
            {
                std::cerr << "Error setting timeout on TCP stream: " << conn.last_error_str() << std::endl;
                return false;
            }

            const packet::header_t state = 206;
            if (conn.write_n(&state, sizeof(state)) != static_cast<ssize_t>(sizeof(state)))
            {
                std::cerr << "Error writing to the TCP stream: " << conn.last_error_str() << std::endl;
                return false;
            }
            std::cout << "Send " << sizeof(state) << " byte(s) to " << conn.peer_address() << " !" << std::endl;

            packet::header_t header;
            while (conn.read_n(&header, sizeof(header)) == static_cast<ssize_t>(sizeof(header)))
            {
                assert(header > 0);

                switch (header)
                {

                case packet::server::headers::header_game_phase: {
                    uint8_t phase_type;
                    if (conn.read_n(&phase_type, sizeof(phase_type) != -1))
                        std::cout << "phase " << static_cast<std::int32_t>(phase_type) << std::endl;
                }
                break;

                case packet::server::headers::header_channel_status: {
                    int32_t size;
                    if (conn.read_n(&size, sizeof(size)) != -1)
                    {
                        std::vector<packet::server::channel_status> channel_status(size);
                        if (conn.read_n(&channel_status[0], size * sizeof(packet::server::channel_status)) != -1)
                        {
                            for (const auto &v : channel_status)
                            {
                                std::cout << "port: " << v.port << (v.status == 1 ? " ON" : " OFF") << std::endl;
                            }
                        }
                        channel_status.clear();
                    }
                    uint8_t succes;
                    if (conn.read_n(&succes, sizeof(succes)) != -1)
                    {
                        std::cout << (succes ? "Success" : "Failed") << " channel status!" << std::endl;
                    }
                }
                break;

                case packet::server::headers::header_handshake: {
                    uint32_t handshake;
                    if (conn.read_n(&handshake, sizeof(handshake)) != -1)
                    {
                        std::cout << "handshake: " << handshake << std::endl;
                    }
                    uint32_t time;
                    if (conn.read_n(&time, sizeof(time)) != -1)
                    {
                        std::cout << "time: " << time << std::endl;
                    }
                    int32_t delta;
                    if (conn.read_n(&delta, sizeof(delta)) != -1)
                    {
                        std::cout << "delta: " << time << std::endl;
                    }
                }
                break;

                default:
                    std::cerr << "unknown header " << static_cast<std::int32_t>(header) << std::endl;
                    break;
                }
            }
        }
        else
        {
            std::cout << options.help() << std::endl;
            return true;
        }
    }
    catch (const cxxopts::OptionException &e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    catch (...)
    {
        std::cerr << "unhandled exception" << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char **argv)
{
    sockpp::socket_initializer sock_init;
    return parse_command_line(argc, argv);
}
