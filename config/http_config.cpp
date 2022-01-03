#include <fstream>
#include <iostream>
#include <boost/json.hpp>
#include <utility>
#include <arpa/inet.h>

#include "http_config.h"

void tag_invoke(boost::json::value_from_tag, boost::json::value &json_value, const ListenConfig &config)
{
	int family = config._address.family;
	char addr_str[INET6_ADDRSTRLEN];
	inet_ntop(family, &config._address.addr, addr_str, sizeof(addr_str));
	json_value = {{"address", addr_str},
	              {"port",    ntohs(config._address.port)}};
}

sockaddr_generic ListenConfig::get_address() const
{
	return _address;
}

void ListenConfig::set_address(const std::string &addr)
{
	int family = AF_INET;
	if (std::string::npos != addr.find(':'))
	{
		family = AF_INET6;
	}
	inet_pton(family, addr.c_str(), &_address.addr);
	_address.family = family;
}

unsigned short ListenConfig::get_port() const
{
	return ntohs(_address.port);
}

void ListenConfig::set_port(unsigned short port)
{
	_address.port = htons(port);
}

void tag_invoke(boost::json::value_from_tag, boost::json::value &json_value, const TLSConfig &config)
{
	json_value = {{"tls_cert_file", config.tls_cert_file},
	              {"tls_key_file",  config.tls_key_file}};
}

std::string TLSConfig::get_cert_file() const
{
	return tls_cert_file;
}

void TLSConfig::set_cert_file(std::string file_name)
{
	tls_cert_file = std::move(file_name);
}

std::string TLSConfig::get_key_file() const
{
	return tls_key_file;
}

void TLSConfig::set_key_file(std::string file_name)
{
	tls_key_file = std::move(file_name);
}

void HttpConfig::print_http_config(const HttpConfig &config)
{
	char addr_str[INET6_ADDRSTRLEN];
	std::cout << "server name:" << config.server_name << std::endl;
	std::cout << "listen: \n";
	for (const ListenConfig &tmp: config.listen)
	{
		auto addr = tmp.get_address();
		inet_ntop(addr.family, &addr.addr, addr_str, sizeof(addr_str));
		std::cout << "address: " << addr_str << "@" << tmp.get_port() << std::endl;
	}
	std::cout << "web root" << config.web_root << std::endl;
	std::cout << "tls cert file: " << config.tls_config.get_cert_file();
	std::cout << "tls key file" << config.tls_config.get_key_file() << std::endl;
}

void HttpConfig::parser_json_value(const boost::json::value &json_value)
{
	boost::json::object config_obj = boost::json::value_to<boost::json::object>(json_value);
	server_name = boost::json::value_to<std::string>(config_obj.at("server_name"));
	std::vector<boost::json::object> listen_vec = boost::json::value_to<std::vector<boost::json::object>>(
			config_obj.at("listen"));
	std::cout << "listen: [";
	for (boost::json::object tmp_obj: listen_vec)
	{
		ListenConfig listen_config;
		listen_config.set_address(boost::json::value_to<std::string>(tmp_obj.at("address")));
		listen_config.set_port(boost::json::value_to<int>(tmp_obj.at("port")));
		listen.push_back(listen_config);
	}
	std::cout << "]" << std::endl;
	web_root = boost::json::value_to<std::string>(config_obj.at("web_root"));
	boost::json::object tls_config_obj = boost::json::value_to<boost::json::object>(config_obj.at("tls_config"));
	tls_config.set_cert_file(boost::json::value_to<std::string>(tls_config_obj.at("tls_cert_file")));
	tls_config.set_key_file(boost::json::value_to<std::string>(tls_config_obj.at("tls_key_file")));
}

void tag_invoke(boost::json::value_from_tag, boost::json::value &json_value, const HttpConfig &config)
{
	json_value = {{"server_name", config.server_name},
	              {"listen",      config.listen},
	              {"web_root",    config.web_root},
	              {"tls_config",  config.tls_config}};
}

std::string HttpConfig::get_server_name()
{
	return server_name;
}

void HttpConfig::set_server_name(std::string name)
{
	server_name = std::move(name);
}

std::vector<ListenConfig> HttpConfig::get_listen_cfg()
{
	return listen;
}

void HttpConfig::set_listen_cfg(std::vector<ListenConfig> cfg)
{
	listen = std::move(cfg);
}

std::string HttpConfig::get_web_root()
{
	return web_root;
}

void HttpConfig::set_web_root(std::string root)
{
	web_root = std::move(root);
}

TLSConfig HttpConfig::get_tls_cfg()
{
	return tls_config;
}

void HttpConfig::set_tls_cfg(TLSConfig cfg)
{
	tls_config = std::move(cfg);
}