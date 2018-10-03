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

#ifndef CLIENT_H
#define CLIENT_H

#ifdef _WIN32
	#define _WIN32_WINNT 0x0501
#endif

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "auxiliar.h"

class User;

class Client : private boost::noncopyable,
	public boost::enable_shared_from_this<Client>
{
	public:
		Client(boost::asio::io_service& service);
		~Client();

		static uint32_t clientCount;

		void start();

		void send(const std::string& msg);
		void close();

		void closeConnection(const std::string& reason);

		std::string getHostName() const { return m_host; }

		std::string getIP() const;

		boost::asio::ip::tcp::socket& socket() { return m_socket; }

		bool isClosed() const { return !m_socket.is_open(); }
	protected:
		void receiveFromSocket(const boost::system::error_code& e, std::size_t bytes);
		void disconnectClient();

	private:
		User* m_user;
		bool sentMotd;

		void firstTime();
		void handleRequest(const StringVec&);
		void sendLine(const char *fmt, ...);
		bool sentUser;
		bool sentNick;
		bool sentCommon;
		bool closed;
		bool m_ident;
		bool isConnecting;
		boost::asio::ip::tcp::socket m_socket;
		boost::asio::io_service::strand readStrand, writeStrand;
		boost::asio::streambuf m_buffers;
		boost::asio::ip::tcp::resolver m_resolver;
		uint32_t m_eventId;

		void handle_write(const boost::system::error_code& e, size_t bytes);
		void handle_resolve(const boost::system::error_code&, 
			boost::asio::ip::tcp::resolver::iterator);

		void closeWithError(int error);
		void receive();
		void hostThread();
		std::string m_host;

	protected:
		void sendPing();
		int32_t getLastPingTime() const;
		bool gotReply() const { return m_reply; }
		uint32_t m_pongEvent;
		int32_t lastPong;
		int32_t notPingReply;
		std::string lastPing;
		bool m_reply;
		bool firstPing;
};
#endif
