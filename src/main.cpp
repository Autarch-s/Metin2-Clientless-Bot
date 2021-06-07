#include "packets.h"
#include <chrono>
#include <csignal>
#include <cxxopts.hpp>
#include <iostream>
#include <sockpp/tcp_connector.h>
#include <sockpp/version.h>
#include <thread>

namespace
{
volatile bool interrupted{false};
}

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

            std::signal(SIGINT, [](int) { interrupted = true; });

            // packet::client::server_status status;
            const std::uint8_t header = 254;
            do
            {
                if (conn.write_n(&header, sizeof(header)) != static_cast<ssize_t>(sizeof(header)))
                {
                    std::cerr << "Error writing to the TCP stream: " << conn.last_error_str() << std::endl;
                    break;
                }
                std::cout << "Send " << sizeof(header) << " byte(s) to " << conn.peer_address() << " !" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            } while (!interrupted);
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
