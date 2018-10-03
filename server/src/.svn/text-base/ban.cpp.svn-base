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

#include "ban.h"
#include "user.h"

Ban::Ban(time_t _t, BanType_t _type, User* _user,
	const std::string& comment)
	: user(_user), type(_type), m_comment(comment)
{
	if (_t == -1 || _t == 0)
		t = -1;
	else
		t = time(NULL) + _t;
}

Ban::~Ban()
{
	//
}

bool Ban::hasFinished(time_t now)
{
	if ((((type & BT_GLINE) == BT_GLINE) || (type & BT_KLINE) == BT_KLINE) && t == -1) //server ban, cannot be unbanned
		return false;

	if (now > t)
		return true;

	return false;
}

void Ban::setType(BanType_t _type)
{
	if (type == _type)
		return;

	type = _type;
}

User* Ban::getUser() const
{
	return user;
}
