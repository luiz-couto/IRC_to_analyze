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

#ifndef SERVER_H
#define SERVER_H

#ifdef _WIN32
	#define _WIN32_WINNT 0x0501
#endif

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <stdint.h>

class Client;
typedef boost::shared_ptr<Client> ClientPtr;

class Server : private boost::noncopyable
{
	public:
		explicit Server(const std::string& port);

		void start();
		void run();

	private:
		boost::asio::io_service m_io;
		void handle_accept(const boost::system::error_code& e);

		boost::asio::ip::tcp::acceptor m_acceptor;
		ClientPtr m_client;
		uint32_t m_connections, m_maxConnections;
};
#endif
