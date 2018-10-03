////////////////////////////////////////////////////////////////////////// Textual IRC Server - An open source irc server////////////////////////////////////////////////////////////////////////// Copyright (C) 2010 - 2011 Fallen <F.Fallen45@gmail.com>////////////////////////////////////////////////////////////////////////// This program is free software: you can redistribute it and/or modify// it under the terms of the GNU General Public License as published by// the Free Software Foundation, either version 3 of the License, or// (at your option) any later version.//// This program is distributed in the hope that it will be useful,// but WITHOUT ANY WARRANTY; without even the implied warranty of// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the// GNU General Public License for more details.//// You should have received a copy of the GNU General Public License// along with this program.  If not, see <http://www.gnu.org/licenses/>.////////////////////////////////////////////////////////////////////////#include "mainframe.h"#include <iostream>#include <sstream>#include "auxiliar.h"#include "client.h"#include "user.h"#include "server.h"#include "ban.h"#include "dispatcher.h"#include "configreader.h"#include "wildcard.h"#include <boost/bind.hpp>#define OPENCONNECTIONS_INTERVAL 100extern Dispatcher* g_dispatcher;extern Config* g_config;MainFrame::MainFrame(){	m_eventId = 0;	StringVec ports = splitString(g_config->getString(Config::C_PORTS), ";");	if (!ports.size() || ports.size() < 1)		return;	for (int32_t i = 0; i < ports.size(); ++i) {		std::stringstream ss;		ss << ports[i];		std::string port;		ss >> port;		Server *server = new Server(port);		if (!server)			continue;		m_servers.push_back(server);	}}void MainFrame::start(){	m_eventId = g_dispatcher->addEvent(new Task(OPENCONNECTIONS_INTERVAL, boost::bind(&MainFrame::openConnectionsThread, this)));	for (ServersVec::iterator it = m_servers.begin(); it != m_servers.end(); ++it)		(*it)->start();	for (ServersVec::iterator it = m_servers.begin(); it != m_servers.end(); ++it)		(*it)->run();}void MainFrame::openConnectionsThread(){	uint32_t ip = 0, times = 0;	for (ConnectionsMap::iterator it = m_connections.begin(); it != m_connections.end(); ++it) {		if (it->first != 0) {			times++;			ip = it->first;		}	}	if (times > 5 && ip != 0) {		irc_notice(MainFrame::openConnectionsThread, "Connections from: " << ip << " bigger than 5.")		irc_notice(MainFrame::openConnectionsThread, "Closing connection: " << ip << ".")		for (ConnectionsMap::const_iterator it = m_connections.begin(); it != m_connections.end(); ++it) {			if (it->first == ip) {				it->second->close();				delete it->second;				m_connections.erase(it->first);				break;			}		}	}	m_eventId = g_dispatcher->addEvent(new Task(OPENCONNECTIONS_INTERVAL, boost::bind(&MainFrame::openConnectionsThread, this)));}MainFrame::~MainFrame(){	for (BanLinesMap::iterator it = m_bans.begin(); it != m_bans.end(); ++it)		delete it->second;	m_bans.clear();	for (UsersMap::iterator it = m_users.begin(); it != m_users.end(); ++it)		delete it->second;	m_users.clear();	for (ConnectionsMap::iterator it = m_connections.begin(); it != m_connections.end(); ++it)		delete it->second;	m_connections.clear();	for (ServersVec::iterator it = m_servers.begin(); it != m_servers.end(); ++it)        delete *it;}Ban* MainFrame::getBan(const std::string& IP){	for (BanLinesMap::const_iterator it = m_bans.begin(); it != m_bans.end(); ++it) {		if (wildcardfit(const_cast<char*>(it->first.c_str()), const_cast<char*>(IP.c_str())))			return it->second;	}	return NULL;}User* MainFrame::getUserByName(const std::string& __name){	UsersMap::const_iterator it = m_users.find(toLower(__name));	if (it == m_users.end())		return NULL;	return it->second;}void MainFrame::addUser(User* p){	for (UsersMap::const_iterator it = m_users.begin(); it != m_users.end(); ++it) {		if (it->second != p) {			if (toLower(it->second->getNick()) == toLower(p->getNick())) {				p->sendLine(":%s 433 * %s :Nickname is already in use.\r\n", p->getServer().c_str(),					p->getNick().c_str());				return;			}		}	}	m_users.insert(std::make_pair(toLower(p->getNick()), p));	std::clog << "Current Users Online: ";	for (UsersMap::const_iterator it = m_users.begin(); it != m_users.end(); ++it) {		if (!it->second || !it->second->isAlive())			continue;		std::clog << it->second->getNick() << "\t";	}	std::clog << std::endl;}std::vector<User*> MainFrame::getIrcOps(){	std::vector<User*> ret;	if (!m_users.size())		return std::vector<User*>();	for (UsersMap::const_iterator it = m_users.begin(); it != m_users.end(); ++it) {		if (!it->second || !it->second->isAlive())			continue;		if (it->second->getType() != UT_IRC_OP)			continue;		ret.push_back(it->second);	}	return ret;}void MainFrame::broadcastMessage(const std::string& message, bool toIrcOps){	if (toIrcOps) {		std::vector<User*> ircops = getIrcOps();		if (!ircops.size())			return;		for (std::vector<User*>::iterator it = ircops.begin(); it != ircops.end(); ++it)			(*it)->sendLine(":%s!%s@%s PRIVMSG %s :%s\r\n", "ServerMessage", (*it)->getServer().c_str(),				(*it)->getServer().c_str(), (*it)->getNick().c_str(), message.c_str());		return;	}	for (UsersMap::const_iterator it = m_users.begin(); it != m_users.end(); ++it) {		if (!it->second || !it->second->isAlive())			continue;		it->second->sendPrivateMessage("ServerMessage", it->second->getServer(), it->second->getServer(),			it->second->getNick(), message.c_str());	}}void MainFrame::removeUser(const std::string& name){	UsersMap::iterator it = m_users.find(toLower(name));	if (it == m_users.end())		return;	m_users.erase(it);}voidMainFrame::addClient(Client* c){	if (!c)		return;	std::stringstream ss;	ss << c->getIP();	uint32_t ip;	ss >> ip;	ConnectionsMap::iterator it = m_connections.find(ip);	if (it != m_connections.end())		return;	m_connections[ip] = c;}boolMainFrame::changeNick(const std::string& oldNick, const std::string& newNick){	if (oldNick == newNick)		return false;	UsersMap::iterator it = m_users.find(oldNick);	if (it == m_users.end())		return false;	User* tmpUser = it->second;	m_users.erase(it);	m_users.insert(std::make_pair(newNick, tmpUser));	return true;}void MainFrame::updateUser(User* user){	UsersMap::iterator it = m_users.find(toLower(user->getNick()));	if (it == m_users.end())		return;	m_users.erase(it);	m_users.insert(std::make_pair(toLower(user->getNick()), user));}