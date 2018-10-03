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

#ifndef CHANNELS_H
#define CHANNELS_H

#include <string>
#include <map>

class User;
class Ban;
class Channel;

enum UMode_t
{
	UM_NONE,
	UM_VOICE,
	UM_OP
};
enum CMode_t
{
	CM_TOPIC,
	CM_NO_EXTERNAL_MESSAGES,
	CM_INVITE_ONLY,
	CM_MODERATED,
	CM_PRIVATE,
	CM_SECRET,
	CM_KEY,
	CM_LIMIT,
	CM_REGISTERED_USERS,
	CM_NO_NOTICES,
	CM_NO_CTCPS,
	CM_NO_COLORS
};

#include <set>

typedef std::map<UMode_t, User*> _UsersMap;
typedef std::set<CMode_t> ChanModes;
typedef std::map<Ban*, User*> ChanBansMap;

#include <ctime>
#include <stdint.h>

class Channel
{
	public:
		/*!
		* @param channel name
		 */
		Channel(const std::string& _name);

		~Channel();

		/*!
		* @param ban info
		* @brief grant a ban
		 */
		bool addBan(User* user, Ban* ban);

		/*!
		* @param ban info
		* @brief remove a ban
		 */
		bool removeBan(User* user);

		/*!
		* @return users in the channel
		 */
		_UsersMap* getUsers();

		/*!
		* @brief set a mode.
		  */
		bool addMode(CMode_t);

		/*!
		* @param the user to add
		 */
		bool addUser(User* user);

		/*!
		* @param the user to remove
		 */
		bool removeUser(User* user, bool quit = false);

		/*!
		* @return channel modes
		 */
		ChanModes *getModes();

		/*!
		* @return all bans
		 */
		ChanBansMap getBans();

		/*!
		* @return ban info
		 */
		Ban* getBan(User* user);

		/*!
		* @return channel name
		 */
		std::string getName() const;

		/*!
		* @return true on success
		 */
		bool hasMode(CMode_t mode);

		/*!
		* @return a user by name
		 */
		User* getUserByName(const std::string& __name);

		/*!
		* @brief get a mode
		 */
		bool getMode(CMode_t mode);

		/*!
		* @brief get a user mode.
		 */
		bool getUserMode(User* user, UMode_t mode);

		/*!
		* @brief post a quit message
		 */
		void postQuitMessage(User* user, const std::string& __msg);

		/*!
		* @brief update user mode.
		 */
		void updateUserMode(User* user, UMode_t mode);

		/*!
		* @brief set topic.
		 */
		void setTopic(User* user, const std::string& newTopic = std::string());

		/*!
		* @brief get topic.
		 */
		std::string getTopic();

		/*!
		* @brief invite user.
		 */
		bool inviteUser(User* dest, User* from);

		/*!
		* @brief has user.
		 */
		bool hasUser(User* user) const;

		/*!
		* @brief change user nick
		  */
		bool changeUserNick(const std::string& oldNick, User* user);

		/*!
		* @return creation time.
		  */
		time_t getCreated() const { return creationTime; }
	private:
		ChanModes m_modes;
		_UsersMap m_users;
		ChanBansMap m_bans;
		std::string name;
		time_t topicTime;
		std::string topic;
		uint32_t topicEvent;
		time_t creationTime;

	protected:
		void addEventTopic();
		void onChangeTopic();
		void resetTopicTime();
};

typedef std::map<std::string, Channel*> ChannelsMap;

class Channels
{
	public:
		Channels();
		~Channels();

		bool init() { return true; }
		bool addChannel(Channel* channel);

		bool removeChannel(const std::string& name);

		Channel* getChannel(const std::string& name);

		const ChannelsMap* getChannels() const { return &m_channels; }

		void updateChannel(Channel* channel);
	private:
		ChannelsMap m_channels;
};
#endif
