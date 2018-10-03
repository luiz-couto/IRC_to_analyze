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

#include "user.h"
#include "client.h"
#include "auxiliar.h"
#include "channels.h"
#include "dispatcher.h"

#include "ban.h"
#include "database.h"
#include "configreader.h"
#include "defines.h"

#include <iostream>
#include <cstdio>
#include <sstream>
#include <cstdarg>

extern Channels *g_channels;
extern Config* g_config;
extern Dispatcher* g_dispatcher;

unsigned int User::operCount;
unsigned int User::userCount;

User::User(const std::string& _name, Client* _client)
	: userName(_name), client(_client)
{
	sentUser = 0;
	idleTime = joinTime = 0;
	type = UT_NONE;
	setHost();
	ident = userName;
	IP = client->getIP();
	server = g_config->getString(0x08);
	userCount++;
	nick = "";
	invis = false;
	m_idleEvent = g_dispatcher->addEvent(new Task(1, boost::bind(&User::addEventIdle, this))); //1 second
	m_joinEvent = g_dispatcher->addEvent(createTask(1, boost::bind(&User::addEventJoin, this))); //1 second
}

User::~User()
{
	m_modes.clear();

	for (BansMap::iterator it = m_bans.begin(); it != m_bans.end(); ++it)
		__deallocate(it->second);

	m_bans.clear();
	m_channels.clear();
	//stop events
	g_dispatcher->stopEvent(m_idleEvent );
	g_dispatcher->stopEvent(m_joinEvent);
}

std::string User::getUserName()
{
	return userName;
}

std::string User::getRealName()
{
	return realName;
}

std::string User::getNick()
{
	return nick;
}

std::string User::getModes()
{
	std::string modes = "";
	for (Modes::iterator it = m_modes.begin(); it != m_modes.end(); ++it) {
		char mode = *it;
		mode -= 65;
		modes += mode;
	}
	return modes;
}

std::string User::getHost()
{
	return host;
}

std::string User::getServer() const
{
	return server;
}

Client* User::getClient() const
{
	return client;
}

void User::tmp_quit(const char *msg)
{
	for (ChansMap::const_iterator it = m_channels.begin(); it != m_channels.end(); ++it) {
		if (!it->second)
			continue;

		it->second->postQuitMessage(this, std::string(msg));
		it->second->removeUser(this);
		it->second->addUser(this);
	}
}

bool User::canJoinChannels()
{
	if (getType() >= UT_IRC_OP)
		return true;

	return m_channels.size() < 20;
}

Ban* User::getBan()
{
	//TODO
	return NULL;
}

void User::quit(const std::string& __msg)
{
	for (ChansMap::iterator it = m_channels.begin(); it != m_channels.end(); ++it) {
		if (!it->second)
			continue;

		it->second->postQuitMessage(this, __msg);
	}
}

bool User::isAlive()
{
	return client != NULL;
}

bool User::setMode(char mode)
{
	Modes::const_iterator it = m_modes.find(mode);
	if (it != m_modes.end())
		return false;

	m_modes.insert(mode - 65);
	return true;
}

bool User::joinChannel(const std::string& channel)
{
	ChansMap::iterator it = m_channels.find(channel);
	if (it != m_channels.end())
		return false;

	Channel* p = g_channels->getChannel(channel);
	if (p) {
		m_channels[channel] = p;
		return true;
	}
	return false;
}

bool User::partChannel(const std::string& channel)
{
	ChansMap::iterator it = m_channels.find(channel);
	if (it == m_channels.end())
		return false;

	m_channels.erase(it);
	return true;
}

bool User::setNick(const std::string& newNick)
{
	if (nick == newNick)
		return false;

	nick = newNick;
	return true;
}

void User::setUserName(const std::string& __name)
{
	if (userName == __name)
		return;

	++sentUser;
	if (sentUser == 1)
		return;

	userName = __name;
}

void User::setRealName(const std::string& __name)
{
	realName = __name;
}

bool User::setIdent(const std::string& newIdent)
{
	if (ident == newIdent)
		return false;

	ident = newIdent;
	return true;
}

bool User::setHost(const std::string& newHost)
{
	if (host == newHost)
		return false;

	host = newHost;
	return true;
}

bool User::isBanned(const std::string& channel)
{
	Channel *tmp = g_channels->getChannel(channel);
	if (!tmp)
		return false;

	Ban *p = tmp->getBan(this);
	if (!p)
		return false;

	return true;
}

BansMap User::getBans()
{
	return m_bans;
}

void User::addServerBan(Ban* ban)
{
	switch (ban->getType()) {
		case BT_KLINE:
		{
			client->closeConnection("K-Lined");
			break;
		}
		case BT_GLINE:
		{
			client->closeConnection("G-Lined");
			break;
		}
		default:
			return;
	}
}

void User::addChannelBan(const std::string& channel, Ban* ban)
{
	Channel* tmp = g_channels->getChannel(channel);
	if (!tmp)
		return;

	tmp->addBan(this, ban);
}

void User::sendMotd(const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	char buffer[512];
	vsprintf(buffer, fmt, arg);
	if (client)
		client->send(std::string(buffer));

	va_end(arg);
}

bool User::isIrcOp()
{
	return type == UT_IRC_OP;
}

bool User::isIrcAdmin()
{
	return type == UT_IRC_ADMINSTRATOR;
}

UserType_t User::getType()
{
	return type;
}

void User::setType(UserType_t newType)
{
	if (type == newType)
		return;

	type = newType;
	if (type >= UT_IRC_OP) {
		userCount--;
		operCount++;
	}
}

void User::sendMessage(MessageType_t _type, const std::string& msg)
{
	std::string tmp = ":" + getServer() + " ";
	std::string _name = getUserName();
	switch (_type) {
		case MESSAGE_UNKNOWN:
		{
			tmp.clear();
			tmp = msg;
			break;
		}
		case MESSAGE_MOTD:
		{
			tmp += "372 " + _name + " " + msg;
			break;
		}
		case MESSAGE_001:
		{
			tmp += "001 " + _name + " " + msg;
			break;
		}
		case MESSAGE_002:
		{
			tmp += "002 " + _name + " " + msg;
			break;
		}
		case MESSAGE_004:
		{
			tmp += "004 " + _name + " " + msg;
			break;
		}
		case MESSAGE_005:
		{
			tmp += "005 " + _name + " " + msg;
			break;
		}
		case MESSAGE_NAMESLIST:
		{
			tmp += "353 " + _name + " " + msg;
			break;
		}
		case MESSAGE_ENDOFNAMESLIST:
		{
			tmp += "366 " + _name + " " + msg;
			break;
		}
		case MESSAGE_JOIN:
		{
			tmp.clear();
			tmp = ":" + _name + "!" + getIdent() + "@" + getHost() + " JOIN " + msg;
			break;
		}
		case MESSAGE_PART:
		{
			tmp.clear();
			tmp = ":" + _name + "!" + getIdent() + "@" + getHost() + " PART " + msg;
			break;
		}
		case MESSAGE_QUIT:
		{
			tmp.clear();
			tmp = ":" + _name + "!" + getIdent() + "@" + getHost() + " QUIT :Quit";
			if (!msg.empty())
				tmp += ":" + msg;
			break;
		}
		case MESSAGE_MODE:
		{
			tmp.clear();
			tmp = ":" + _name + "!" + getIdent() + "@" + getHost() + " MODE " + getNick() + " " + msg;
			break;
		}
		case MESSAGE_MODE_I:
		{
			tmp += "221 " + _name + " +i";
			break;
		}
		case MESSAGE_ENDOFMOTD:
		{
			tmp += "376 " + _name + " :End of /MOTD command.";
			break;
		}
		default:
		{
			tmp.clear();
			break;
		}
	}
	tmp += "\r\n";
	if (client)
		client->send(tmp);
}

void User::sendJoinMessage(const std::string& __name, const std::string& __ident,
	const std::string& __host, const std::string& __channel)
{
	std::stringstream ss;
	ss << ":" << __name << "!" << __ident << "@" << __host << " JOIN " << __channel << "\r\n";
	if (client)
		client->send(ss.str());
	ss.str("");
}

void User::sendQuitMessage(const std::string& __name, const std::string& __ident,
	const std::string& __host, const std::string& __msg)
{
	std::stringstream ss;
	ss << ":" << __name << "!" << __ident << "@" << __host << " QUIT ";
	if (!__msg.empty())
		ss << ":" << __msg;
	else
		ss << ":Quit";

	ss << "\r\n";
	if (client)
		client->send(ss.str());
	ss.str("");
}

void User::sendModeMessage(const std::string& __name, const std::string& __ident,
	const std::string& __host, char mode)
{
	std::stringstream ss;
	ss << ":" << __name << "!" << __ident << "@" << __host << " MODE " << __name << " " << mode << "\r\n";
	if (client)
		client->send(ss.str());
	ss.str("");
}

void User::sendPartMessage(const std::string& __name, const std::string& __ident,
	const std::string& __host, const std::string& __channel,
	const std::string& __msg)
{
	std::stringstream ss;
	ss << ":" << __name << "!" << __ident << "@" << __host << " PART " << __channel;
	if (!__msg.empty())
		ss << " :" << __msg;

	ss << "\r\n";
	if (client)
		client->send(ss.str());
	ss.str("");
}

void User::sendTopicMessage(const std::string& __name, const std::string& __ident,
	const std::string& __host, const std::string& __channel,
	const std::string& __topic)
{
	std::stringstream ss;
	ss << ":" << __name << "!" << __ident << "@" << __host << " TOPIC " << __channel << " :" << __topic << "\r\n";
	if (client)
		client->send(ss.str());
	ss.str("");
}

void User::sendInviteMessage(const std::string& __name, const std::string& __ident,
	const std::string& __host, const std::string& __channel)
{
	std::stringstream ss;
	ss << ":" << __name << "!" << __ident << "@" << __host << " INVITE " << __channel << "\r\n";
	if (client)
		client->send(ss.str());
}

void User::sendPrivateMessage(const std::string& __name, const std::string& __ident,
	const std::string& __host, const std::string& __from,
	const std::string& __msg)
{
	std::stringstream ss;
	ss << ":" << __name << "!" << __ident << "@" << __host << " PRIVMSG " << __from << " :" << __msg;
	ss << "\r\n";
	if (client)
		client->send(ss.str());
}

std::string User::getIdent() const
{
	return ident;
}

void User::setHost()
{
	if (!client)
		return;

	if (!client->getHostName().empty())
		host = client->getHostName();
	else
		host = client->getIP();
}

void User::kill(const char *msg)
{
	addServerBan(new Ban(static_cast<time_t>(-1), BT_KLINE, this, std::string(msg)));
	std::stringstream ss;
	ss << "ERROR :Closing Link: " << getHost() << " (Killed *";
	std::string p = g_config->getString(Config::C_HOST);
	ss << "." << p.substr(p.find("."), p.length()-p.find(".")) << " (" << msg << ")";
	ss << ")";
	ss << "\r\n";
	if (client) {
		std::string quitmsg = "Killed *." + p.substr(p.find("."), p.length()-p.find(".")) + " (" + msg + ")";
		this->quit(quitmsg);
		client->send(ss.str());
		client->close();
	}
#ifdef _DEBUG
	else {
		std::clog << "[Warning: User::kill] - Trying to send a msg to a user without a client." << std::endl;
		std::clog << "With User: " << this << " - " << this->getUserName() << " " << this->getHost() << "." << std::endl;
		std::clog << "[User::kill]: Failed to kill user." << std::endl;
	}
#endif
}

void User::gline(const char *msg, int length)
{
	addServerBan(new Ban((time_t)length, BT_GLINE, this, std::string(msg)));
	std::stringstream ss;
	ss << "ERROR :Closing Link: " << getHost() << " (G-Lined)";
	if (client) {
		this->quit("G-Lined");
		client->send(ss.str());
		client->close();
	}
#ifdef _DEBUG
	else {
		std::clog << "[Warning: User::gline] - Trying to send a msg to a user without a client." << std::endl;
		std::clog << "With User: " << this << " - " << this->getUserName() << " " << this->getHost() << "." << std::endl;
		std::clog << "[User::gline]: Failed to gline user." << std::endl;
	}
#endif
}

void User::kline(const char *msg, int length)
{
	addServerBan(new Ban((time_t)length, BT_KLINE, this, std::string(msg)));
	std::stringstream ss;
	ss << "ERROR :Closing Link: " << getHost() << " (K-Lined)";
	if (client) {
		this->quit("K-Lined");
		client->send(ss.str());
		client->close();
	}
#ifdef _DEBUG
	else {
		std::clog << "[Warning: User::kline] - Trying to send a msg to a user without a client." << std::endl;
		std::clog << "With User: " << this << " - " << this->getUserName() << " " << this->getHost() << "." << std::endl;
		std::clog << "[User::kline]: Failed to kline user." << std::endl;
	}
#endif
}

Channel* User::getChannelByName(const std::string& __name) const
{
	ChansMap::const_iterator it = m_channels.find(__name);
	if (it == m_channels.end())
		return NULL;

	return it->second;
}

void User::sendLine(const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	char buffer[512];
	vsprintf(buffer, fmt, arg);
	va_end(arg);
	client->send(std::string(buffer));
}

bool User::hasMode(char mode)
{
	Modes::const_iterator it = m_modes.find(mode);
	if (it != m_modes.end())
		return true;

	return false;
}

void User::clearChannels()
{
	m_channels.clear();
}

bool User::checkOperPassword(const std::string& __account, const std::string& __password)
{
	Database *db = Database::instance();
	if (!db->connect())
		return false;

	std::stringstream ss;
	ss << "SELECT `name`, `pass`, `group` FROM `ircops` WHERE `id` != 0;";
	Result* result = db->storeQuery(ss);
	if (!result)
		return false;

	bool success = false;
	do {
		std::string name = result->getDataString("name");
		std::string password = result->getDataString("pass");
		int group = result->getDataInt("group");
		if (name == __account) {
			if (password == transformPassword(__password)) {
				switch (group) {
					default:
					case 1:
						setType(UT_IRC_OP);
						break;
					case 2:
						setType(UT_IRC_ADMINSTRATOR);
						break;
				}
				success = true;
			} else if (transformPassword(password) == transformPassword(__password)) {
				switch (group) {
					default:
					case 1:
						setType(UT_IRC_OP);
						break;
					case 2:
						setType(UT_IRC_ADMINSTRATOR);
						break;
				}
				success = true;
			}
		}
	} while (result->next());
	result->free();
	return success;
}

void User::sendFileMotd()
{
	FILE *fp = fopen("motd.txt", "r");
	if (!fp) {
		sendMotd(":%s 372 :MOTD file missing.\r\n", getServer().c_str());
		delete fp;
		fp = NULL;
		return;
	}
	char buffer[1024];
	while (fgets(buffer, sizeof(buffer), fp))
		sendMotd(":%s 372 %s :%s\r\n", getServer().c_str(),
			getNick().c_str(), buffer);
}

bool User::isInvited(const std::string& __channel)
{
	InvitedToChannelsSet::iterator it = m_invitedToChannels.find(__channel);
	if (it == m_invitedToChannels.end())
		return false;

	return true;
}

bool User::removeInvitedChannel(const std::string& __channel)
{
	InvitedToChannelsSet::iterator it = m_invitedToChannels.find(__channel);
	if (it == m_invitedToChannels.end())
		return false;

	m_invitedToChannels.erase(it);
	return true;
}

void User::addEventIdle()
{
	idleTime++;
	m_idleEvent = g_dispatcher->addEvent(new Task(1, boost::bind(&User::addEventIdle, this)));
}

void User::addEventJoin()
{
    joinTime++;
    m_joinEvent = g_dispatcher->addEvent(createTask(1, boost::bind(&User::addEventJoin, this)));
}
