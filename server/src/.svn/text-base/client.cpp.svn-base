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

#include "client.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <sstream>
#include <istream>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string.h>

#include "user.h"
#include "channels.h"
#include "auxiliar.h"
#include "defines.h"
#include "configreader.h"
#include "dispatcher.h"
#include "mainframe.h"
#include "ban.h"

extern Config* g_config;
extern Channels* g_channels;
extern MainFrame* g_mainFrame;
extern Dispatcher* g_dispatcher;

uint32_t Client::clientCount;

Client::Client(boost::asio::io_service& service)
	: m_socket(service),
	readStrand(service),
	writeStrand(service),
	m_resolver(service)
{
	m_eventId = m_pongEvent = 0;
	sentNick = sentUser = sentCommon = sentMotd = false;
	m_user = NULL;
	closed = false;
	m_ident = false;
	m_host = "";
	lastPing = "";
	lastPong = 0;
	m_reply = false;
	notPingReply = 0;
	isConnecting = true;
	firstPing = false;
}

Client::~Client()
{
	if (clientCount > 0)
		clientCount--;

	std::clog << "Client destructor: " << this << ":" << getIP() << "." << std::endl;
	//stop events
	if (m_eventId)
		g_dispatcher->stopEvent(m_eventId);

	if (m_pongEvent)
		g_dispatcher->stopEvent(m_pongEvent);

	if (m_user)
		g_mainFrame->removeUser(m_user->getNick());
}

void Client::sendLine(const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	char buffer[512];
	vsprintf(buffer, fmt, arg);
	va_end(arg);
	send(std::string(buffer));
}

void Client::firstTime()
{
	std::stringstream ss;
	ss << MOTD_COMMON << "Looking up your hostname...\r\n";
	send(ss.str());
	ss.str("");
	ss << MOTD_COMMON << "Checking Ident\r\n";
	send(ss.str());
	ss.str("");
	if (!getHostName().empty())
		ss << MOTD_COMMON << "Found your hostname: " << getHostName() << ".";
	else
		ss << MOTD_COMMON << "Couldn't resolve your hostname; using your IP address instead";

	ss << "\r\n";
	send(ss.str());
	ss.str("");
	switch (ident(getIP())) {
		case SOCKET_NOT_INITED:
			irc_notice(Client::useIdent, Unable to initalize socket.);
			break;

		case SOCKET_NOT_CONNECTED:
			irc_notice(Client::useIdent, Unable to connect socket.);
			break;

		case SOCKET_UNABLE_TO_WRITE:
			irc_notice(Client::useIdent, Unable to write data on the socket.);
			break;

		case SOCKET_UNABLE_TO_READ:
			irc_notice(Client::useIdent, Unable to read data on the socket.);
			break;

		case CONNECTION_SUCCESS:
		default:
		{
			ss << MOTD_COMMON << "Got ident response";
			ss << "\r\n";
			send(ss.str());
			m_ident = true;
			return;
		}
	}
	ss.str("");
	ss << MOTD_COMMON << "No ident response";
	ss << "\r\n";
	send(ss.str());
}

void Client::sendPing()
{
	m_reply = false;
	char ping[50];
	if (!firstPing) {
		int32_t ping_[12];
		for (int32_t i = 1; i < 11; ++i)
			ping_[i] = rand() % i;

		sprintf(ping, "%d", ping_);
		firstPing = true;
	} else {
		sprintf(ping, "%s", g_config->getString(Config::C_SERVER_HOST).c_str());
	}

	std::stringstream ss;
	ss << "PING :" << ping << "\r\n";
	send(std::string(ss.str()));
	lastPong++;
}

void Client::start()
{
	if (Ban* ban = g_mainFrame->getBan(getIP())) {
		sendLine(":%s NOTICE :*** You are permanently banned from %s <%s>", g_config->getString(Config::C_HOST).c_str(),
			g_config->getString(Config::C_SERVER_NAME).c_str(), ban->getComment().c_str());
		closeConnection("(User permanently banned from " + g_config->getString(Config::C_SERVER_NAME) + " (" +
			ban->getComment() + ")");
		return;
	}

	hostThread();
	std::clog << "New Client." << std::endl;
	firstTime();
	clientCount++;
	g_mainFrame->addClient(this);
	receive();
}

#define CHECK_SOCKET()\
	if (!m_socket.is_open()) {\
		std::clog << "[Notice - Client::" << __FUNCTION__ << ":" << __LINE__ << "] Socket is not open." << std::endl;\
		return;\
	}

void Client::receive()
{
	CHECK_SOCKET()
	boost::asio::async_read_until(m_socket, m_buffers,
		"\n",
			readStrand.wrap(
				boost::bind(&Client::receiveFromSocket, shared_from_this(),
					boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred)));
}

std::string Client::getIP() const
{
	if (!m_socket.is_open()) {
		std::clog << "[Error - Client::getIP] Socket is not open." << std::endl;
		return std::string();
	}

	return m_socket.remote_endpoint().address().to_string();
}

void Client::hostThread()
{
	boost::asio::ip::tcp::resolver::query query(getIP(), "6667");
	m_resolver.async_resolve(query,
		boost::bind(&Client::handle_resolve, this,
			boost::asio::placeholders::error,
				boost::asio::placeholders::iterator));
}

void Client::handle_resolve(const boost::system::error_code& e, 
	boost::asio::ip::tcp::resolver::iterator iterator)
{
	CHECK_SOCKET()
	if (e) {
		std::clog << "[Error - Client::handle_resolve] " << e.message() << "." << std::endl;
		return;
	}

	m_host = std::string((*iterator).host_name());
}

void Client::send(const std::string& msg)
{
	CHECK_SOCKET()
	boost::asio::async_write(m_socket, boost::asio::buffer(msg, msg.length()),
		writeStrand.wrap(
			boost::bind(&Client::handle_write, shared_from_this(),
				boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)));
}

void Client::handle_write(const boost::system::error_code& e, size_t bytes)
{
	if (e)
		closeWithError(0x04);
}

void Client::closeWithError(int error)
{
	switch (error) {
		case 10054:
			closeConnection("Read error: Connection reset by peer");
			break;

		case 10053:
			closeConnection("Software caused connection abort");
			break;

		case 0x02:
			closeConnection("Read error: EOF from client");
			break;

		case 0x03:
			closeConnection("EOF from client");
			break;

		case 0x04:
			closeConnection("Write error: Operation not permitted");
			break;

		case 0x05:
			closeConnection("Read error: Operation timed out");
			break;

		default:
			closeConnection("Input/Output error");
			break;
	}
}

void Client::receiveFromSocket(const boost::system::error_code& e, std::size_t bytes)
{
	if (e) {
		if (e == boost::system::errc::connection_reset)
			closeWithError(10054);
		else if (e == boost::system::errc::operation_not_permitted)
			closeWithError(0x05);
		else
			closeWithError(0x02);

		return;
	}

	std::string str = "";
	std::istream s(&m_buffers);
	std::getline(s, str);
	size_t sep = str.rfind("\r");
	if (sep != std::string::npos)
		str = str.substr(0, str.length()-1);

	std::clog << "Received message from client: " << getIP() << " :" << str << std::endl;

	StringVec splited = splitString(str, " ");
	if (!splited.size() || splited.size() < 0) {
		receive();
		return;
	}

	if (splited[0] == "USER") {
		if (splited.size() < 4) {
			receive();
			return;
		}

		if (sentUser && m_user) {
			m_user->sendLine(":%s 462 %s :You may not rereigster\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str());
			return;
		}

		std::string realName;
		for (uint32_t __i = 4; __i < static_cast<uint32_t>(splited.size()); ++__i) {
			realName += splited[static_cast<size_t>(__i)];
			if (splited.size() >= 5)
				realName += " ";
		}

		std::string userName = splited[1];

		realName = realName.substr(1, realName.length()-1);
		if (!m_user) {
			if (userName.length() > 15)
				m_user = new User(userName.substr(0, 15), this);
			else
				m_user = new User(userName, this);
		} else {
			if (userName.length() > 15)
				m_user->setUserName(userName.substr(0, 15));
			else
				m_user->setUserName(userName);
		}
		m_user->setRealName(realName);
		std::clog << "New user: " << getIP() << ": " << userName << ":" << realName << "." << std::endl;
		sentUser = true;
	} else if (splited[0] == "NICK") {
		if (!isConnecting) {
			handleRequest(splited);
			receive();
			return;
		}

		if (splited.size() < 1) {
			receive();
			return;
		}

		User* user = NULL;
		if ((user = g_mainFrame->getUserByName(toLower(splited[1])))) {
			sendLine(":%s 433 %s %s :Nickname is already in use.\r\n", g_config->getString(Config::C_SERVER_HOST).c_str(),
				splited[1].c_str(), splited[1].c_str());
			receive();
			return;
		}
		std::string nickName = splited[1];
		std::clog << "New nick: " << getIP() << ": " << nickName << "." << std::endl;
		size_t s = nickName.find(":");
		if (s != std::string::npos)
			nickName.erase(s);

		bool canUse = true;
		for (uint32_t i = 0; i < static_cast<uint32_t>((sizeof(forbiddenNames) / sizeof(const char*))); ++i) {
			if (toLower(nickName) == toLower(std::string(&forbiddenNames[i]))) {
				canUse = false;
				sendLine(":%s 433 %s %s :Nickname is already in use.\r\n", g_config->getString(Config::C_SERVER_HOST).c_str(),
					nickName.c_str(), nickName.c_str());
				break;
			}
		}

		for (uint32_t __i = 0; __i < static_cast<uint32_t>((sizeof(errnNickNames) / sizeof(const char*))); ++__i) {
			if (toLower(nickName).find(toLower(std::string(&errnNickNames[__i]))) != std::string::npos) {
				canUse = false;
				sendLine(":%s 433 %s %s :Nickname is already in use.\r\n", g_config->getString(Config::C_SERVER_HOST).c_str(),
					nickName.c_str(), nickName.c_str());
				break;
			}
		}

		if (!canUse) {
			receive();
			return;
		}

		if (!m_user) {
			if (nickName.length() > 15) {
				m_user = new User(nickName.substr(0, 15), this);
				m_user->setNick(nickName.substr(0, 15));
			} else {
				m_user = new User(nickName, this);
				m_user->setNick(nickName);
			}
		} else {
			if (nickName.length() > 15)
				m_user->setNick(nickName.substr(0, 15));
			else
				m_user->setNick(nickName);
		}

		if (!sentNick)
			sentNick = true;

	} else if (splited[0] == "PONG") {
		if (!sentUser || !sentNick) {
			receive();
			return;
		}

		if (splited.size() > 1) {
			std::string pong = splited[1];
			size_t sep = pong.find(":");
			if (sep != std::string::npos)
				pong.erase(sep);

			if (pong == lastPing && getLastPingTime() != 120) {
				lastPong = 0xF * 8;
				m_reply = true;
				g_dispatcher->addEvent(new Task(lastPong, boost::bind(
					&Client::sendPing, this)));
				if (m_eventId)
					g_dispatcher->stopEvent(m_eventId);

			} else {
				notPingReply++;
				std::stringstream ss;
				ss << "Ping timeout: " << notPingReply << " seconds";
				m_eventId = g_dispatcher->addEvent(createTask(120, boost::bind(
					&Client::closeConnection, this, ss.str())));
			}
		}
	} else {
		if ((!sentUser || !m_user) && !closed) {
			//flooders
			m_eventId = g_dispatcher->addEvent(createTask(40, boost::bind(&Client::closeConnection, this, "Registration timeout")));
			closed = true;
		}
	}
	if (sentUser && sentNick) {
		if (m_pongEvent == 0)
			m_pongEvent = g_dispatcher->addEvent(createTask(0, boost::bind(&Client::sendPing, this)));

		if (gotReply()) {
			g_dispatcher->stopEvent(m_eventId);
			handleRequest(splited);
			std::clog << "Got ping reply from: " << getIP() << "." << std::endl;
			closed = false;
			if (!sentCommon) {
				m_user->sendMotd(":%s 001 %s :Welcome to %s IRC network, %s\r\n", m_user->getServer().c_str(),
					m_user->getUserName().c_str(), g_config->getString(0x07).c_str(), m_user->getNick().c_str());
				m_user->sendMotd(":%s 002 %s :Your host is %s running version %s\r\n", m_user->getServer().c_str(),
					m_user->getUserName().c_str(), m_user->getHost().c_str(), __DISTURBTION__);
				m_user->sendMotd(":%s 003 %s :This server was created %s %s\r\n", m_user->getServer().c_str(),
					m_user->getUserName().c_str(), __TIME__, __DATE__);
				m_user->sendMotd(":%s 252 %s %d :operator(s) online\r\n", m_user->getServer().c_str(),
					m_user->getUserName().c_str(), User::operCount);
				m_user->sendFileMotd();
				m_user->sendMotd(":%s NOTICE %s :Highest connection count: %d (%d clients)\r\n", m_user->getServer().c_str(),
					m_user->getUserName().c_str(), User::userCount, Client::clientCount);
				m_user->sendMotd(":%s 221 %s +i\r\n", m_user->getServer().c_str(),
					m_user->getNick().c_str());
				m_user->setMode('i');
				m_user->sendMotd(":%s!%s@%s MODE %s +i\r\n", m_user->getUserName().c_str(),
					m_user->getIdent().c_str(), m_user->getHost().c_str(),
						m_user->getNick().c_str());
				g_mainFrame->addUser(m_user);
				sentCommon = true;
				isConnecting = false;
			}
		}
	}
	if (m_user)
		g_mainFrame->updateUser(m_user);

	receive();
}

int32_t Client::getLastPingTime() const
{
	return lastPong;
}

void Client::disconnectClient()
{
	CHECK_SOCKET()
	std::clog << "Client disconnected: " << getIP() << std::endl;
	close();
}

void Client::closeConnection(const std::string& reason)
{
	CHECK_SOCKET()
	std::clog << "Closing connection: " << getIP() << " reason: " << reason << "...";
	if (m_user) {
		m_user->quit(reason);
	}
	disconnectClient();
	std::clog << "[OK]" << std::endl;
}

void Client::handleRequest(const StringVec& __msg)
{
	StringVec splited = __msg;
	if (!splited.size())
		return;

	/* change mode */
	if (splited[0] == "MODE") {
		if (splited.size() < 2)
		 	return;

		if (toLower(splited[1]) != toLower(m_user->getNick())) { //channel mode
			if (splited[1].substr(0, 1) == "#") {
				Channel* chan = m_user->getChannelByName(splited[1]);
				if (!chan) {
					m_user->sendLine(":%s 442 %s %s :You're not on that channel\r\n", m_user->getServer().c_str(),
						m_user->getUserName().c_str(), splited[1].c_str());
				} else {
					if (splited.size() < 3) {
						std::stringstream ss;
						ChanModes chanModes = *chan->getModes();
						for (ChanModes::iterator it = chanModes.begin(); it != chanModes.end(); ++it)
							ss << *it;

						m_user->sendLine(":%s 324 %s %s +%s\r\n", m_user->getServer().c_str(),
							m_user->getNick().c_str(), splited[1].c_str(), ss.str().c_str());
						m_user->sendLine(":%s 329 %s %s %d\r\n", m_user->getServer().c_str(),
							m_user->getNick().c_str(), splited[1].c_str(), chan->getCreated());
						return;
					}

					if (chan->getUserMode(m_user, UM_OP)) {
						std::stringstream ss;
						ss << splited[2];
						int32_t mode;
						ss >> mode;
						if (chan->addMode(static_cast<CMode_t>(mode))) {
							for (_UsersMap::iterator it = chan->getUsers()->begin(); it != chan->getUsers()->end(); ++it) {
								if (!it->second)
									continue;

								it->second->sendLine(":%s!%s@%s MODE %s %s\r\n", m_user->getNick().c_str(),
									m_user->getIdent().c_str(), m_user->getHost().c_str(), splited[1].c_str(),
										(splited.size() >= 2 ? splited[2].c_str() : ""));
							}
						}
					} else {
						m_user->sendLine(":%s 482 %s %s :You're not channel operator\r\n", m_user->getServer().c_str(),
							m_user->getUserName().c_str(), splited[1].c_str());
					}
					g_channels->updateChannel(chan);
				}
			} else {
				m_user->sendLine(":%s 502 %s :Cant change mode for other users\r\n", m_user->getServer().c_str(),
					m_user->getNick().c_str());
			}
		} else if (toLower(splited[1]) == toLower(m_user->getNick())) { //user mode
			if (m_user->hasMode(*const_cast<char*>(splited[2].c_str()) - 65))
				return;

			m_user->setMode(*const_cast<char*>(splited[2].c_str()));
			m_user->sendLine(":%s!%s@%s MODE %s %s\r\n", m_user->getNick().c_str(),
				m_user->getIdent().c_str(), m_user->getHost().c_str(), m_user->getNick().c_str(),
					splited[2].c_str());
		} else {
			m_user->sendLine(":%s 502 %s :Cant change mode for other users\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str());
		}
	/* join a channel */
	} else if (splited[0] == "JOIN") {
		if (splited.size() < 2) {
			m_user->sendLine(":%s 461 %s %s :Not enough parameters\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str(), splited[0].c_str());
			return;
		}

		if (splited[1] == "#opers" && m_user->getType() < UT_IRC_OP) {
			m_user->sendLine(":%s 479 %s %s :Cannot join channel (G-lined: invalid channel)\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str(), splited[1].c_str());
			return;
		}

		toLower(splited[1]);
		Channel* tmpChannel = NULL;
		if ((tmpChannel = m_user->getChannelByName(toLower(splited[1]))))
			return;

		if (splited[1].substr(0, 1) != "#") {
			m_user->sendLine(":%s 403 %s %s :No such channel\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str(), splited[1].c_str());
			return;
		}

		Channel* p = NULL;
		if (!(p = g_channels->getChannel(toLower(splited[1])))) {
			Channel* chan = new Channel(splited[1]);
			m_user->sendLine(":%s!%s@%s JOIN %s\r\n", m_user->getUserName().c_str(),
				m_user->getIdent().c_str(), m_user->getHost().c_str(), splited[1].c_str());
			m_user->sendLine(":%s 353 %s = %s :@%s\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str(), chan->getName().c_str(), m_user->getNick().c_str());
			m_user->sendLine(":%s 366 %s %s :End of /NAMES list.\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str(), chan->getName().c_str());
			chan->addUser(m_user);
			g_channels->updateChannel(chan);
			m_user->joinChannel(splited[1]);
		} else {
			Ban *b = p->getBan(m_user);
			if ((m_user->isInvited(splited[1]) && b) || !b) {
				if (!p->addUser(m_user))
					return;

				std::string users = "";
				_UsersMap *__u = p->getUsers();
				for (_UsersMap::const_iterator it = __u->begin(); it != __u->end(); ++it) {
					if (!it->second)
						continue;

					if (p->getUserMode(it->second, UM_OP))
						users += "@" + it->second->getNick() + " ";
					else if (p->getUserMode(it->second, UM_VOICE))
						users += "+" + it->second->getNick() + " ";
					else
						users += it->second->getNick() + " ";
				}
				m_user->sendLine(":%s 353 %s = %s :%s\r\n", m_user->getServer().c_str(),
					m_user->getNick().c_str(), p->getName().c_str(), users.c_str());
				m_user->sendLine(":%s 366 %s %s :End of /NAMES list.\r\n", m_user->getServer().c_str(),
					m_user->getNick().c_str(), p->getName().c_str());
				g_channels->updateChannel(p);
				m_user->joinChannel(splited[1]);
			} else {
				m_user->sendLine(":%s 474 %s %s :Cannot join channel, you are banned (+b)\r\n",
					m_user->getServer().c_str(), m_user->getNick().c_str(), splited[1].c_str());
			}
		}
	/* part a channel */
	} else if (splited[0] == "PART") {
		if (splited.size() < 2) {
			m_user->sendLine(":%s 461 %s PART :Not enough parameters\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str());
			return;
		}

		Channel* p = NULL;
		bool success = true;
		if ((!(p = m_user->getChannelByName(splited[1]))) && (p = g_channels->getChannel(splited[1]))) {
			m_user->sendLine(":%s 442 %s %s :You're not on that channel\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str(), splited[1].c_str());
			success = false;
		} else if (!(p = m_user->getChannelByName(splited[1]))) {
			m_user->sendLine(":%s 403 %s %s :No such channel\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str(), splited[1].c_str());
			success = false;
		}

		if (!success)
			return;

		m_user->sendMessage(MESSAGE_PART, splited[1]);
		p->removeUser(m_user);
		g_channels->getChannel(splited[1])->removeUser(m_user);
		g_channels->updateChannel(p);
	/* become an operator */
	} else if (splited[0] == "OPER") {
		if (splited.size() > 2) {
			if (!m_user->checkOperPassword(splited[1], splited[2])) {
				m_user->sendLine(":%s 491 %s :No Operator block for your host\r\n", m_user->getServer().c_str(),
					m_user->getNick().c_str());
			} else {
				m_user->sendLine(":%s 381 %s :You are now IRC Operator\r\n", m_user->getServer().c_str(),
					m_user->getNick().c_str());
				std::stringstream ss;
				ss << "JOIN #opers";
				StringVec tmp = splitString(ss.str(), " ");
				handleRequest(tmp);
			}
		}
	/* who is someone */
	} else if (splited[0] == "WHOIS") {
		if (splited.size() < 2) {
			m_user->sendLine(":%s 431 %s :No nickname given\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str());
			return;
		}

		if (toLower(splited[1]) != toLower(m_user->getNick()) && !g_mainFrame->getUserByName(toLower(splited[1]))) {
			m_user->sendLine(":%s 401 %s %s :No such nick\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str(), splited[1].c_str());
			m_user->sendLine(":%s 318 %s %s :End of /WHOIS list.\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str(), splited[1].c_str());
			return;
		}

		if (splited.size() >= 3 && toLower(splited[1]) == toLower(splited[2]))
			doWhoIs(m_user, g_mainFrame->getUserByName(splited[1]), true);
		else
			doWhoIs(m_user, g_mainFrame->getUserByName(splited[1]), false);

	/* private message channel/someone */
	} else if (splited[0] == "PRIVMSG") {
		if (splited.size() < 2) {
			m_user->sendLine(":%s 411 %s :No recipient given (PRIVMSG)\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str());
			return;
		}

		m_user->setIdleTime(0);
		if (splited[1].substr(0, 1) == "#") {
			//private message a channel
			//lets do some checks on b&
			//or +m
			Channel* p = g_channels->getChannel(splited[1]);
			if (!p) {
				m_user->sendLine(":%s 403 %s %s :No such channel.\r\n", m_user->getServer().c_str(),
					m_user->getNick().c_str(), splited[1].c_str());
			} else if ((p = m_user->getChannelByName(splited[1]))) {
				if (p->getMode(CM_MODERATED)) {
					if (p->getUserMode(m_user, UM_NONE)) {
						m_user->sendLine(":%s 404 %s %s :Cannot send to channel\r\n", m_user->getServer().c_str(),
							m_user->getNick().c_str(), splited[1].c_str());
					} else {
						std::string msg = "";
						for (int32_t i = 2; i < static_cast<int32_t>(splited.size()); ++i)
							msg += splited[i] + " ";

						if (!msg.length()) {
							m_user->sendLine(":%s 412 %s :No text to send\r\n", m_user->getServer().c_str(),
								m_user->getNick().c_str());
							return;
						}

						msg = msg.substr(1, msg.length()-1);
						_UsersMap *__u = p->getUsers();
						if (__u) {
							for (_UsersMap::const_iterator it = __u->begin(); it != __u->end(); ++it) {
								if (!it->second)
									continue;

								if (it->second->getNick() == m_user->getNick())
									continue;

								it->second->sendPrivateMessage(m_user->getNick(), m_user->getIdent(),
									m_user->getHost(), splited[1], msg);
							}
						}
					}
				} else {
					std::string msg = "";
					for (int32_t i = 2; i < static_cast<int32_t>(splited.size()); ++i)
						msg += splited[i] + " ";

					if (!msg.length()) {
						m_user->sendLine(":%s 412 %s :No text to send\r\n", m_user->getServer().c_str(),
							m_user->getNick().c_str());
						return;
					}

					msg = msg.substr(1, msg.length()-1);
					_UsersMap *__u = p->getUsers();
					for (_UsersMap::const_iterator it = __u->begin(); it != __u->end(); ++it) {
						if (!it->second)
							continue;

						if (it->second->getNick() == m_user->getNick())
							continue;

						it->second->sendPrivateMessage(m_user->getNick(), m_user->getIdent(),
							m_user->getHost(), splited[1], msg);
					}
				}
			}
		} else {
			// private message someone
			// maybe hes using modes +R
			User* user = g_mainFrame->getUserByName(splited[1]);
			if (!user) {
				m_user->sendLine(":%s 401 %s %s :No such nick\r\n", m_user->getServer().c_str(),
					m_user->getNick().c_str(), splited[1].c_str());
				return;
			} else {
				if (splited.size() < 2) {
					m_user->sendLine(":%s 412 %s :No text to send\r\n", m_user->getServer().c_str(),
						m_user->getNick().c_str());
					return;
				}

				if (!user->hasMode('R')) {
					std::string msg = "";
					for (int i = 2; i < static_cast<int>(splited.size()); ++i)
						msg += splited[i] + " ";

					user->sendPrivateMessage(m_user->getNick(), m_user->getIdent(),
						m_user->getHost(), user->getNick(), msg);
				}
			}
		}
	/* restart server */
	} else if (splited[0] == "RESTART") {
		if (m_user->getType() < UT_IRC_OP) {
			m_user->sendLine(":%s 481 %s :Premission Denied: Insufficient privileges\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str());
			return;
		}
		std::string exec_name = g_config->getString(Config::C_EXECUTABLE_NAME);
		std::string cmd;
	#ifdef _WIN32
		cmd = exec_name + ".exe";
	#else
		cmd = "./" + exec_name;
	#endif
		(void) std::system(cmd.c_str());
		std::exit(1);
	/* quit */
	} else if (splited[0] == "QUIT") {
		if (splited.size() > 0) {
			std::string quitmsg = "";
			for (int32_t i = 1; i < static_cast<int32_t>(splited.size()); ++i)
				quitmsg += splited[i] + " ";

			quitmsg = quitmsg.substr(1, quitmsg.length()-1);
			closeConnection("Quit: " + quitmsg);
		} else {
			closeConnection("");
		}
	/* user host */
	} else if (splited[0] == "USERHOST") {
		if (splited.size() < 1)
			return;

		User* target = g_mainFrame->getUserByName(splited[1]);
		if (!target)
			return;

		m_user->sendLine(":%s 302 %s :%s=+%s@%s\r\n", m_user->getServer().c_str(),
			m_user->getNick().c_str(), target->getUserName().c_str(),
			target->getIdent().c_str(), target->getHost().c_str());
	/* reload config */
	} else if (splited[0] == "REHASH") {
		if (m_user->getType() < UT_IRC_OP) {
			m_user->sendLine(":%s 481 %s :Premission Denied: Insufficient privileges\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str());
			return;
		}

		std::string nick, ident, host;
		Node* n = g_config->getNodeValue("nick");
		if (n)
			nick = std::string(n->content);

		n = g_config->getNodeValue("host");
		if (n)
			host = std::string(n->content);

		n = g_config->getNodeValue("ident");
		if (n)
			ident = std::string(n->content);

		if (g_config->reload())
			m_user->sendPrivateMessage(nick, ident, host, nick,
				"Reloaded config.");
		else
			m_user->sendPrivateMessage(nick, ident, host, nick,
				"Unable to reload config.");
	} else if (splited[0] == "NICK") {
		if (splited.size() < 1) {
			m_user->sendLine(":%s 431 %s :No nickname given\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str());
			return;
		}

		if (splited[1] == m_user->getNick())
			return;

		User* user = NULL;
		if ((user = g_mainFrame->getUserByName(splited[1]))) {
			m_user->sendLine(":%s 433 %s %s :Nickname is already in use.\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str(), splited[1].c_str());
			return;
		}

		std::string oldNick = m_user->getNick();
		size_t sep = splited[1].find(":");
		if (sep != std::string::npos)
			splited[1] = splited[1].substr(1, splited[1].length()-1);

		m_user->setNick(splited[1]);
		m_user->sendLine(":%s!%s@%s NICK :%s\r\n", m_user->getUserName().c_str(),
			m_user->getIdent().c_str(), m_user->getHost().c_str(), splited[1].c_str());

		ChansMap userChannels = *m_user->getChannels();
		for (ChansMap::iterator it = userChannels.begin(); it != userChannels.end(); ++it) {
			if (!it->second)
				continue;

			it->second->changeUserNick(oldNick, m_user);
			_UsersMap chanUsers = *it->second->getUsers();
			for (_UsersMap::iterator user = chanUsers.begin(); user != chanUsers.end(); ++user) {
				if (!user->second)
					continue;

				if (user->second->getNick() == m_user->getNick())
					continue;

				user->second->sendLine(":%s!%s@%s NICK :%s\r\n", m_user->getUserName().c_str(),
					m_user->getIdent().c_str(), m_user->getHost().c_str(), splited[1].c_str());
			}
			Channel *chan = g_channels->getChannel(it->first);
			if (chan)
				chan->changeUserNick(oldNick, m_user);

			g_channels->updateChannel(chan);
		}
	} else if (splited[0] == "AWAY") {
		if (splited.size() < 1) {
			m_user->setAwayMessage("");
			m_user->sendLine(":%s 305 %s :You are no longer marked as being away\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str());
			return;
		}
		std::string awaymsg = "";
		for (uint32_t __i = 1; __i < static_cast<uint32_t>(splited.size()); ++__i)
			awaymsg += splited[static_cast<size_t>(__i)];

		size_t sep = awaymsg.find(":");
		if (sep != std::string::npos)
			awaymsg = awaymsg.substr(1, awaymsg.length()-1);

		m_user->setAwayMessage(awaymsg);
		m_user->sendLine(":%s 306 %s :You have been marked as being away\r\n", m_user->getServer().c_str(),
			m_user->getNick().c_str());
	} else if (splited[0] == "SETHOST") {
		if (splited.size() < 1) {
			m_user->sendLine(":%s 461 %s %s :Not enough parameters\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str(), splited[0].c_str());
			return;
		} else if (m_user->getType() < UT_IRC_OP) {
			m_user->sendLine(":%s 481 %s :Permission Denied- You do not have the correct IRC operator privileges\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str());
			return;
		} else if (m_user->getHost() == splited[1]) {
			m_user->sendLine(":%s NOTICE %s :*** /SetHost Error: requested host is same as current host.\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str());
			return;
		}

		m_user->setHost(splited[1]);
		m_user->setMode('x');
		m_user->sendMessage(MESSAGE_MODE, "+x");
		m_user->sendLine(":%s NOTICE %s :Your nick!user@host is now (%s!%s@%s) - To disable type /mode %s -x\r\n",
			m_user->getServer().c_str(), m_user->getNick().c_str(), m_user->getNick().c_str(),
				m_user->getUserName().c_str(), m_user->getHost().c_str());
	} else if (splited[0] == "KILL") {
		if (splited.size() < 2) {
			m_user->sendLine(":%s 461 %s %s :Not enough parameters\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str(), splited[0].c_str());
			return;
		} else if (m_user->getType() < UT_IRC_OP) {
			m_user->sendLine(":%s 481 %s :Permission Denied- You do not have the correct IRC operator privileges\r\n", m_user->getServer().c_str(),
				m_user->getNick().c_str());
			return;
		}

		User* user = 0;
		if (!(user = g_mainFrame->getUserByName(splited[1]))) {
			//TODO
		}
		//
	} else {
		m_user->sendLine(":%s 421 %s %s :Unknown command\n", m_user->getServer().c_str(),
			m_user->getNick().c_str());
	}
}

void Client::close()
{
	CHECK_SOCKET()
	boost::system::error_code ignored_ec;
	m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	if (ignored_ec) {
		std::clog << "[Error - Client::close] " << ignored_ec.message() << "." << std::endl;
	}
	m_socket.close();
}
