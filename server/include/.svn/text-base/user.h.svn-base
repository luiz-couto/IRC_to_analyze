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

#ifndef USER_H
#define USER_H

#include <string>
#include <set>

typedef std::set<char> Modes;
typedef std::set<std::string> InvitedToChannelsSet;

class Client;
class User;
class Ban;
class Channel;

#define IRC_PROPERTY(type_name, watTo, var_name)\
	type_name get##watTo() {\
		return var_name;\
	}\
	void set##watTo(const type_name& it) {\
		var_name = it;\
	}

enum UserType_t
{
	UT_NONE,
	UT_FAKEUSER,
	UT_LOCALUSER,
	UT_MASTER,
	UT_IRC_OP,
	UT_IRC_ADMINSTRATOR
};

enum MessageType_t
{
	MESSAGE_UNKNOWN,
	MESSAGE_MOTD,
	MESSAGE_001,
	MESSAGE_002,
	MESSAGE_004,
	MESSAGE_005,
	MESSAGE_NAMESLIST,
	MESSAGE_ENDOFNAMESLIST,
	MESSAGE_JOIN,
	MESSAGE_PART,
	MESSAGE_QUIT,
	MESSAGE_MODE,
	MESSAGE_MODE_I,
	MESSAGE_ENDOFMOTD
};

#include <map>
#include <ctime>

/*!
* @std::string: channel name
* @Ban* the ban information.
*/
typedef std::map<std::string, Ban*> BansMap;
typedef std::map<std::string, Channel*> ChansMap;

#include <stdint.h>

class User
{
	public:
		/*!
		* @param _name: User name
		* @param client: the client connecting from it.
		 */
		User(const std::string& _name, Client* _client);
		/*!
		* Destructor.
		 */
		~User();

		/*!
		* @brief number of users online.
		 */
		static unsigned int userCount;

		/*!
		* @brief operators online.
		 */
		static unsigned int operCount;

		/*!
		* @return user name.
		 */
		std::string getUserName();

		/*!
		* @return real name.
		  */
		std::string getRealName();

		/*!
		* @return user nick.
		 */
		std::string getNick();

		/*!
		* @return mode string.
		 */
		std::string getModes();

		/*!
		* @return user's host.
		 */
		std::string getHost();

		/*!
		* @return user's ident.
		 */
		std::string getIdent() const;

		/*!
		* @return get the server name the user connected to.
		 */
		std::string getServer() const;

		/*!
		* @return a ban from the server
		 */
		Ban* getBan();

		/*!
		* Set a mode a string.
		* @param the mode string.
		* Example: +i, +x
		 */
		bool setMode(char mode);

		/*!
		* @brief join channel
		* @param channel name.
		 */
		bool joinChannel(const std::string& channel);

		/*!
		* @brief part a channel.
		* @param channel name.
		 */
		bool partChannel(const std::string& channel);

		/*!
		* @brief set nick name.
		* @param new nick.
		 */
		bool setNick(const std::string& newNick);

		/*!
		* @brief set an ident.
		* @param new ident.
		 */
		bool setIdent(const std::string& newIdent);

		/*!
		* @brief set a host name.
		* @param new host name.
		 */
		bool setHost(const std::string& newHost);

		/*!
		* @brief set his hostname from the client.
		 */
		void setHost();

		/*!
		* @brief set name.
		 */
		void setRealName(const std::string& __name);

		/*!
		* @brief set user name.
		  */
		void setUserName(const std::string& __name);

		/*!
		* @return
		* true if the user is banned from a channel
		* false if hes not banned.
		 */
		bool isBanned(const std::string& channel);

		/*!
		* @return the bans the user has.
		 */
		BansMap getBans();

		/*!
		* @brief add a ban (gline, kline)
		 */
		void addServerBan(Ban* ban);

		/*!
		* @brief add a channel ban.
		* @param channel: channel name, ban: ban info.
		 */
		void addChannelBan(const std::string& channel, Ban* ban);

		/*!
		* @brief send the server motd.
		* only sends it one time.
		 */
		void sendMotd(const char *fmt, ...);

		/*!
		* @return if hes an irc op
		 */
		bool isIrcOp();

		/*!
		* @return if hes an irc admin.
		  */
		bool isIrcAdmin();

		/*!
		* @brief returns the user type.
		 */
		UserType_t getType();

		/*!
		* @brief set the user type
		* @param new type.
		 */
		void setType(UserType_t newType);

		/*!
		* @brief send a message.
		 */
		void sendMessage(MessageType_t _type, const std::string& msg);

		/*!
		* @brief quit
		 */
		void quit(const std::string& __msg);

		/*!
		* @brief get the client connecting from.
		 */
		Client* getClient() const;

		/*!
		* @brief temporary quit.
		* used for changing host etc.
		 */
		void tmp_quit(const char *msg);

		/*!
		* @brief can join channels or not
		 */
		bool canJoinChannels();

		/*!
		* @brief send join msg.
		 */
		void sendJoinMessage(const std::string& __name, const std::string& __ident,
			const std::string& __host, const std::string& __channel);

		/*!
		* @brief send quit msg.
		 */
		void sendQuitMessage(const std::string& __name, const std::string& __ident,
			const std::string& __host, const std::string& __msg = std::string());

		/*!
		* @brief send mode msg.
		 */
		void sendModeMessage(const std::string& __name, const std::string& __ident,
			const std::string& __host, char mode);

		/*!
		* @brief send a part message
		 */
		void sendPartMessage(const std::string& __name, const std::string& __ident,
			const std::string& __host, const std::string& __channel,
			const std::string& __msg = std::string());

		/*!
		* @brief send topic message.
		 */
		void sendTopicMessage(const std::string& __name, const std::string& __ident,
			const std::string& __host, const std::string& __channel,
			const std::string& __topic);

		/*!
		* @brief send invite to channel message.
		 */
		void sendInviteMessage(const std::string& __name, const std::string& __ident,
			const std::string& __host, const std::string& __channel);

		/*!
		* @brief send a private message.
		 */
		void sendPrivateMessage(const std::string& __name, const std::string& __ident,
			const std::string& __host, const std::string& __from,
			const std::string& __msg);

		/*!
		* @brief is still connected.
		 */
		bool isAlive();

		/*!
		* @brief kill
		 */
		void kill(const char *msg);

		/*!
		* @brief disconnect with gline
		 */
		void gline(const char *msg, int length = 0xFF);

		/*!
		* @brief disconnect with kline
		 */
		void kline(const char *msg, int length = -1);

		/*!
		* @brief check if the other user is equal to this.
		 */
		bool operator==(User* user)
		{
			return this->getNick() == user->getNick();
		}

		bool operator!=(User* user)
		{
			return !(*this == user);
		}

        /*!
        * @brief operator =
        */
        User& operator=(User& user)
        {
            this->nick = user.nick;
            this->host = user.host;
            this->ident = user.ident;
            this->idleTime = user.idleTime;
            this->joinTime = user.joinTime;
            this->awaymsg = user.awaymsg;
            this->client = user.client;
            this->invis = user.invis;
            this->realName = user.realName;
            return *this;
        }

		/*!
		* @brief send a who is line
		 */
		void sendWhoIsLine(const char *fmt, ...);

		/*!
		* @brief get channel by name
		 */
		Channel* getChannelByName(const std::string& __name) const;

		/*!
		* @brief send a message with a format.
		 */
		void sendLine(const char *fmt, ...);

		/*!
		* @brief has a mode or not
		 */
		bool hasMode(char mode);

		/*!
		* @brief clear channels.
		 */
		void clearChannels();

		/*!
		* @brief check irc operator information
		 */
		bool checkOperPassword(const std::string& __account, const std::string& __password);

		/*!
		* @brief send motd, reads from file 'motd.txt'
		 */
		void sendFileMotd();

		/*!
		* @brief get channels.
		 */
		ChansMap* getChannels() const { return const_cast<ChansMap*>(&m_channels); }

		/*!
		* @brief has been invited to a channel or not.
		  */
		bool isInvited(const std::string& __channel);

		/* properties */
		IRC_PROPERTY(std::string, AwayMessage, awaymsg)
		IRC_PROPERTY(bool, Invisible, invis)
		IRC_PROPERTY(time_t, IdleTime, idleTime)
		IRC_PROPERTY(time_t, JoinTime, joinTime)
		IRC_PROPERTY(std::string, AuthPassword, authPass)
		IRC_PROPERTY(std::string, AuthName, authName);

		bool isAuthed() const { return !authPass.empty(); }

	private:
		/* real name */
		std::string realName;
		/* user name */
		std::string userName;
		/* host */
		std::string host;
		/* ident */
		std::string ident;
		/* nickname */
		std::string nick;
		/* type (irc op/network adminstrator) etc */
		UserType_t type;
		/* IP address */
		std::string IP;
		/* away message */
		std::string awaymsg;
		/* idle time */
		time_t idleTime;
		/* join time */
		time_t joinTime;
		/* bans map */
		BansMap m_bans;
		/* auth password */
		std::string authPass;
		/* auth name */
		std::string authName;
		/* modes */
		Modes m_modes;
		/* server */
		std::string server;
		/* client connectin from */
		Client* client;
		/* channels map */
		ChansMap m_channels;
		/* how many times changed name */
		int sentUser;
		/* channels invited to */
		InvitedToChannelsSet m_invitedToChannels;
		/* invislbe */
		bool invis;
		/* remove an invited channel */
		bool removeInvitedChannel(const std::string& __channel);
		void addEventIdle();
		void addEventJoin();
		uint32_t m_idleEvent;
		uint32_t m_joinEvent;
};

class FakeUser : public User
{
	public:
		FakeUser(const std::string& name, Client* client)
			: User(name, client)
		{
			setType(UT_FAKEUSER);
		}
};

class LocalUser : public User
{
	public:
		LocalUser(const std::string& name, Client* client)
			: User(name, client)
		{
			setType(UT_LOCALUSER);
		}
};

#endif
