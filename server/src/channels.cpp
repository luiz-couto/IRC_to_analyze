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

#include "channels.h"
#include "ban.h"
#include "auxiliar.h"
#include "dispatcher.h"
#include "user.h"

#include <stdint.h>
#include <cstdio>
#include <sstream>
#include <boost/bind.hpp>

extern Dispatcher* g_dispatcher;

Channel::Channel(const std::string& _name)
	: name(_name)
{
	topicTime = 0;
	creationTime = 0;
}

void Channel::addEventTopic()
{
	topicTime++;
	g_dispatcher->addEvent(new Task(1, boost::bind(&Channel::addEventTopic, this)));
}

void Channel::onChangeTopic()
{
	g_dispatcher->stopEvent(topicEvent);
	resetTopicTime();
}

void Channel::resetTopicTime()
{
	topicTime = time(NULL);
	g_dispatcher->addEvent(new Task(1, boost::bind(&Channel::addEventTopic, this)));
}

Channel::~Channel()
{
	m_users.clear();
	for (ChanBansMap::iterator it = m_bans.begin(); it != m_bans.end(); ++it)
		delete it->second;

	m_bans.clear();
	m_modes.clear();
}

bool Channel::addBan(User* user, Ban* ban)
{
	ChanBansMap::const_iterator it = m_bans.find(ban);
	if (it != m_bans.end())
		return false;

	m_bans.insert(std::make_pair(ban, user));
	return true;
}

bool Channel::hasUser(User* user) const
{
	for (_UsersMap::const_iterator it = m_users.begin(); it != m_users.end(); ++it) {
		if (!it->second)
			continue;

		if (!user)
			continue;

		if (it->second->getNick() == user->getNick())
			return true;
	}
	return false;
}

bool Channel::removeBan(User* user)
{
	if (!user)
		return false;

	for (ChanBansMap::iterator it = m_bans.begin(); it != m_bans.end(); ++it) {
		if (it->second->getNick() == user->getNick()) {
			m_bans.erase(it);
			break;
		}
	}
	return true;
}

void Channel::postQuitMessage(User* user, const std::string& __msg)
{
	for (_UsersMap::const_iterator it = m_users.begin(); it != m_users.end(); ++it) {
		if (it->second->getNick() != user->getNick())
			it->second->sendQuitMessage(user->getNick(), user->getHost(), user->getIdent(), __msg);
	}
	for (_UsersMap::iterator it = m_users.begin(); it != m_users.end(); ++it) {
		if (it->second->getUserName() == user->getUserName()) {
			m_users.erase(it);
			break;
		}
	}
}

std::string Channel::getTopic()
{
	return topic;
}

void Channel::setTopic(User* user, const std::string& newTopic)
{
	if (topic == newTopic)
		return;

	topic = newTopic;
	//send to channel users
	for (_UsersMap::const_iterator it = m_users.begin(); it != m_users.end(); ++it) {
		if (it->second->isAlive())
			it->second->sendTopicMessage(user->getNick(), user->getIdent(), user->getHost(),
				this->getName(), newTopic);
	}
	topicTime = time(NULL);
	onChangeTopic();
}

_UsersMap* Channel::getUsers()
{
	return &m_users;
}

User* Channel::getUserByName(const std::string& __name)
{
	_UsersMap::iterator it = m_users.begin();
	while (it != m_users.end()) {
		if (it->second->getNick() == __name)
			return it->second;
		++it;
	}
	return NULL;
}

bool Channel::addUser(User* user)
{
	for (_UsersMap::const_iterator it = m_users.begin(); it != m_users.end(); ++it) {
		if (it->second->getNick() == user->getNick())
			return false;
	}
	bool success = false;
	if (m_users.empty()) {
		m_users[UM_OP] = user;
		success = true;
	}
	if (!success)
		m_users[UM_NONE] = user;

	//derp
	for (_UsersMap::const_iterator it = m_users.begin(); it != m_users.end(); ++it)
		it->second->sendJoinMessage(user->getNick(), user->getIdent(), user->getHost(), this->getName());

	return true;
}

bool Channel::removeUser(User* user, bool quit)
{
	for (_UsersMap::iterator it = m_users.begin(); it != m_users.end(); ++it) {
		if (it->second->getNick() == user->getNick()) {
			m_users.erase(it);
			break;
		}
	}
	//derp
	for (_UsersMap::const_iterator it = m_users.begin(); it != m_users.end(); ++it) {
		if (quit)
			it->second->sendQuitMessage(user->getNick(), user->getIdent(), user->getHost(), "");
		else
			it->second->sendPartMessage(user->getNick(), user->getIdent(), user->getHost(),
				this->getName());
	}
	return true;
}

ChanModes *Channel::getModes()
{
	return &m_modes;
}

ChanBansMap Channel::getBans()
{
	return m_bans;
}

std::string Channel::getName() const
{
	return name;
}

bool Channel::hasMode(CMode_t mode)
{
	ChanModes::const_iterator it = m_modes.find(mode);
	if (it == m_modes.end())
		return false;

	return true;
}

bool Channel::getUserMode(User* user, UMode_t mode)
{
	for (_UsersMap::const_iterator it = m_users.begin(); it != m_users.end(); ++it) {
		if (it->second->getNick() == user->getNick() && (it->first & mode) == mode)
			return true;
	}
	return false;
}

bool Channel::getMode(CMode_t mode)
{
	ChanModes::const_iterator it = m_modes.find(mode);
	if (it == m_modes.end())
		return false;

	return true;
}

bool Channel::inviteUser(User* dest, User* from)
{
	if (!from || !from->isAlive())
		return false;

	if (!dest || !dest->isAlive())
		return false;

	for (_UsersMap::const_iterator it = m_users.begin(); it != m_users.end(); ++it) {
		if (it->second->isAlive() && it->second->getUserName() == dest->getUserName())
			return false;

		if (it->second->getUserName() != from->getUserName())
			return false;

		if ((it->first & UM_OP) != UM_OP)
			return false;
	}
	dest->sendInviteMessage(from->getNick(), from->getHost(), from->getIdent(), this->getName());
	return true;
}

Channels::Channels()
{
	//
}

Channels::~Channels()
{
	for (ChannelsMap::iterator it = m_channels.begin(); it != m_channels.end(); ++it)
		delete it->second;

	m_channels.clear();
}

bool Channels::addChannel(Channel* channel)
{
	if (!channel)
		return false;

	if (m_channels.find(toLower(channel->getName())) != m_channels.end())
		return false;

	m_channels[toLower(channel->getName())] = channel;
	return true;
}

bool Channels::removeChannel(const std::string& name)
{
	ChannelsMap::iterator it = m_channels.find(toLower(name));
	if (it == m_channels.end())
		return false;

	m_channels.erase(it);
	return true;
}

Channel* Channels::getChannel(const std::string& name)
{
	ChannelsMap::iterator it = m_channels.find(toLower(name));
	if (it != m_channels.end())
		return it->second;

	return NULL;
}

Ban* Channel::getBan(User* user)
{
	if (!user)
		return NULL;

	for (ChanBansMap::const_iterator it = m_bans.begin(); it != m_bans.end(); ++it) {
		if (it->second->getNick() == user->getNick())
			return it->first;
	}
	return NULL;
}

void Channel::updateUserMode(User* user, UMode_t mode)
{
	std::stringstream ss;
	for (_UsersMap::iterator it = m_users.begin(); it != m_users.end(); ++it) {
		if (it->second->getUserName() == user->getUserName())
			ss << it->first;
	}
	//derp
	for (_UsersMap::iterator it = m_users.begin(); it != m_users.end(); ++it) {
		if (it->second->getUserName() == user->getUserName()) {
			m_users.erase(it);
			break;
		}
	}
	ss << mode;
	int __modes = 0;
	ss >> __modes;
	UMode_t modes = static_cast<UMode_t>(__modes);
	m_users[modes] = user;
	std::stringstream p;
	p << mode;
	char _mode;
	p >> _mode;
	for (_UsersMap::const_iterator it = m_users.begin(); it != m_users.end(); ++it)
		it->second->sendModeMessage(user->getNick(), user->getHost(), user->getIdent(), _mode);
}

bool Channel::changeUserNick(const std::string& oldNick, User* user)
{
	for (_UsersMap::iterator it = m_users.begin(); it != m_users.end(); ++it) {
		if (it->second->getNick() == oldNick) {
			it->second = user;
			return true;
		}
	}
	return false;
}

bool Channel::addMode(CMode_t mode)
{
	if (m_modes.find(mode) != m_modes.end())
		return false;

	m_modes.insert(mode);
	return true;
}

void Channels::updateChannel(Channel* channel)
{
    std::clog << "Updating channel...." << std::endl;
	if (!channel)
		return;

    std::clog << "With name: " << channel->getName() << "." << std::endl;
	ChannelsMap::iterator it = m_channels.find(toLower(channel->getName()));
	if (it != m_channels.end()) {
		m_channels.erase(it);
		m_channels.insert(std::make_pair(channel->getName(), channel));
		std::clog << "Done." << std::endl;
		return;
	}
	m_channels.insert(std::make_pair(channel->getName(), channel));
	std::clog << "Done #2" << std::endl;
}
