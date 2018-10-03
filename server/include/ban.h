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

#ifndef BAN_H
#define BAN_H

class Ban;
class User;
enum BanType_t
{
	BT_GLINE = 0,
	BT_KLINE = 1,
	BT_CHANBAN = 2
};

#include <string>
#include <ctime>

class Ban
{
	public:
		/*!
		* @param the ban type
		* @param the user to ban
		 */
		Ban(time_t _t, BanType_t _type, User* _user, const std::string& comment);

		~Ban();

		/*!
		* @return true if the ban has finished else false
		 */
		bool hasFinished(time_t now);

		void setType(BanType_t _type);

		User* getUser() const;

		BanType_t getType() const { return type; }

		std::string getComment() const { return m_comment; }

	private:
		User* user;
		BanType_t type;
		time_t t;
		std::string m_comment;
};
#endif
