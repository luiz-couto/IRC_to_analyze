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

#ifndef DEFINES_H
#define DEFINES_H

#ifndef MOTD_COMMON
	#define MOTD_COMMON "NOTICE AUTH :*** "
#endif
#ifndef __DISTURBTION__
	#define __DISTURBTION__ "TextualIRCD1.0"
#endif
#ifndef SOFTWARE_VERSION
	#define SOFTWARE_VERSION "1.0"
#endif
#ifndef CREATED
	#define CREATED "Fri Nov 19 2010"
#endif
static const char forbiddenNames[] = {
	'A', 'B', 'C', 'D', 'E', 'F'
};

static const char errnNickNames[] = {
	'~', '@', '+', '%', '&', '!'
};

class User;

struct WhoWas
{
	User* user;
};

#endif
