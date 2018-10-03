////////////////////////////////////////////////////////////////////////
// Textual IRC Server - An open source irc server
////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010 - 2011 Fallen <F.Fallen45@gmail.com>
////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////

#include "server.h"
#include "client.h"

#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <sstream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <vector>

#include "defines.h"
#include "auxiliar.h"
#include "configreader.h"

extern Config* g_config;

Server::Server(const std::string& port)
	: m_client(new Client(m_io)), m_acceptor(m_io)
{
	m_maxConnections = static_cast<uint32_t>(g_config->getInt(Config::C_MAX_PENDING_CONNECTIONS));
	m_connections = 0;
	boost::asio::ip::tcp::resolver resolver(m_io);
	boost::asio::ip::tcp::resolver::query query(g_config->getString(Config::C_HOST), port);
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	m_acceptor.open(endpoint.protocol());
	m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	m_acceptor.bind(endpoint);
	m_acceptor.listen();
}

void Server::run()
{
	std::vector<boost::shared_ptr<boost::thread> > threads;
	for (int32_t i = 0; i < 2; ++i) {
		boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&boost::asio::io_service::run, &m_io)));
		threads.push_back(thread);
	}
	for (std::vector<boost::shared_ptr<boost::thread> >::iterator it = threads.begin(); it != threads.end(); ++it)
		(*it)->join();
}

void Server::start()
{
	m_acceptor.async_accept(m_client->socket(),
		boost::bind(&Server::handle_accept, this,
			boost::asio::placeholders::error));
}

void Server::handle_accept(const boost::system::error_code& e)
{
	if (!e) {
		m_client->start();
		m_client.reset(new Client(m_io));
		if (m_maxConnections == 0 || ++m_connections <= m_maxConnections)
			m_acceptor.async_accept(m_client->socket(),
				boost::bind(&Server::handle_accept, this,
					boost::asio::placeholders::error));
	} else {
		std::clog << "[Error - Server::handle_accept] " << e.message() << "." << std::endl;
	}
}
