#include <iostream>
#include <sockpp/tcp_connector.h>
#include <sockpp/version.h>
#include <chrono>
#include <cxxopts.hpp>

static bool parse_command_line(int argc, char**& argv)
{
	try
	{
		cxxopts::Options options("Metin2-Clientless-Bot");
		options.positional_help("[optional args]").show_positional_help();
		options.allow_unrecognised_options().add_options()
			("i, ip", "-i <127.0.0.1>", cxxopts::value<std::string>())
			("p, port", "-p <12345>", cxxopts::value<std::uint16_t>())
			("h, help", "Print help");

		const auto result = options.parse(argc, argv);
		const auto& arguments = result.arguments();

		if (arguments.empty() || result.count("help"))
		{
			std::cout << options.help() << std::endl;
			return true;
		}

		const auto ip = result.count("ip");
		const auto port = result.count("port");

		if (ip && port)
		{
			const auto ip_arg = result["ip"].as<std::string>();
			const auto port_arg = result["port"].as<std::uint16_t>();

			sockpp::tcp_connector conn({ip_arg, port_arg});
			if (!conn)
			{
				std::cerr << "Error connecting to server at " << sockpp::inet_address(ip_arg, port_arg) << "\n" << conn.
					last_error_str() << std::endl;
				return false;
			}

			std::cout << "Created a connection from " << conn.address() << std::endl;

			if (!conn.read_timeout(std::chrono::seconds(5)))
			{
				std::cerr << "Error setting timeout on TCP stream: " << conn.last_error_str() << std::endl;
				return false;
			}

			std::string data = "@";
			while (true)
			{
				data += data;
				if (conn.write(data) != static_cast<ssize_t>(data.length()))
				{
					std::cerr << "Error writing to the TCP stream: " << conn.last_error_str() << std::endl;
					break;
				}
				std::cout << "Send " << data.size() << " bytes" << std::endl;
			}
		}
		else
		{
			std::cout << options.help() << std::endl;
			return true;
		}
	}
	catch (const cxxopts::OptionException& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	catch (const std::exception& e)
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

int main(int argc, char** argv)
{
	sockpp::socket_initializer sock_init;
	return parse_command_line(argc, argv);
}
