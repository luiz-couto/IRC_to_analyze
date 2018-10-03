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

#ifndef AUXILIAR_H
#define AUXILIAR_H

#ifdef WIN32
	#include <io.h>
	#include <direct.h>
#else
	#include <unistd.h>
#endif

#include <vector>
#include <string>
#include <ctime>
#include <iostream>
#include <boost/bind.hpp>
#include <ostream>
#include <stdint.h>

#ifndef IRC_API
	#define IRC_API extern
#endif

#include "dispatcher.h"

extern Dispatcher* g_dispatcher;

enum ConnectionError_t
{
#ifdef _WIN32
	WSA_NOT_INITED,
#endif
	SOCKET_NOT_INITED,
	INVALID_HOST,
	SOCKET_NOT_CONNECTED,
	SOCKET_UNABLE_TO_WRITE,
	SOCKET_UNABLE_TO_READ,
	CONNECTION_SUCCESS
};

typedef std::vector<std::string> StringVec;
IRC_API StringVec splitString(const std::string& str, const std::string& sep = " ");

IRC_API std::string transformPassword(const std::string&);
IRC_API std::string toLower(std::string);
IRC_API bool replaceString(std::string text, const std::string& key, const std::string& value);

IRC_API ConnectionError_t ident(const std::string& Ip);

IRC_API std::string getTime();

class User;

IRC_API void doWhoIs(User* from, User* dest, bool show);

IRC_API const char* getMessage(const char*);

#ifdef assert
	#undef assert
#endif
#define assert(p)\
	if (!p) {\
		std::clog << "Asseration to: " << #p << " failed. line: " << __LINE__ << ", file: " << __FILE__ << " (" << __TIME__ << " - " << __DATE__ << "." << std::endl;\
		g_dispatcher->addEvent(new Task(10, boost::bind(exit, 5)));\
	}

#define irc_warning(x, y)\
	std::clog << "[Warning - " << #x << "] " << #y << std::endl;

#define irc_error(x, y)\
	std::clog << "[Error - " << #x << "] " << #y << std::endl;

#define irc_notice(x, y)\
	std::clog << "[Notice - " << #x << "] " << #y << std::endl;

char* replace_string(char *str, char *orig, char *rep);
template<typename __T>
inline void*
__construct(__T* __p)
{ return ::new(static_cast<void*>(__p)) __T(); }

template<typename __T>
inline void
__destruct(__T* __p)
{ assert(__p != 0); __p->~__T(); }

typedef size_t size_type;

template<typename __T>
inline __T*
__allocate(size_type __n, const void* = 0)
{ return static_cast<__T*>(::operator new(__n * sizeof(__T*))); }

template<typename __T>
inline void
__deallocate(__T* __p)
{ ::operator delete(__p); }

#endif
