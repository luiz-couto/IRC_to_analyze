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

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <string>

class Client;
class MainFrame;
class User;
class Ban;
class Server;

#include <map>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <stdint.h>
#include <vector>

typedef std::map<std::string, User*> UsersMap;
typedef std::map<std::string, Ban*> BanLinesMap;
typedef std::map<uint32_t, Client*> ConnectionsMap;
typedef std::vector<Server*> ServersVec;

class MainFrame
{
	public:
		MainFrame();
		~MainFrame();

		/*!
		* @brief call "server" to start accepting connections.
		  */
		void start();

		/*!
		* @__name user name
		* @brief get an instance of "User" by his name.
		  */
		User* getUserByName(const std::string& __name);

		/*!
		* @IP client IP to get a ban from
		* @brief kline/gline/zline ban
		  */
		Ban* getBan(const std::string& IP);

		/*!
		* @brief broadcast a message.
		  */
		void broadcastMessage(const std::string& message, bool toIrcOps);

		/*!
		* @brief change user's nick
		  */
		bool changeNick(const std::string&, const std::string&);

	private:
		/* get all irc ops online */
		std::vector<User*> getIrcOps();
		ServersVec m_servers;
		UsersMap m_users;
		BanLinesMap m_bans;
		ConnectionsMap m_connections;
		uint32_t m_eventId;
		friend class Client;

	protected:
		void addUser(User*);
		void removeUser(const std::string& name);
		void addClient(Client* c);
		void openConnectionsThread();
		void updateUser(User* user);
};
#endif
