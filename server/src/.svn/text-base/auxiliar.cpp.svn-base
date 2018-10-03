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

#include "auxiliar.h"
#include "user.h"
#include "channels.h"
#include "configreader.h"
#include "client.h"

#include <cryptopp/sha.h>
#include <cryptopp/adler32.h>
#include <cryptopp/hmac.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>
#include <cryptopp/cryptlib.h>

#include <algorithm>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <ctype.h>
#include <sstream>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <sys/timeb.h>
#endif

#ifdef _WIN32
	#include <winsock2.h>
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
#endif

#ifndef SOCKET
	#define SOCKET int
#endif

#ifndef SOCKET_ERROR
	#define SOCKET_ERROR -1
#endif

#ifndef SOCKADDR
	#define SOCKADDR sockaddr
#endif

extern Config* g_config;

StringVec splitString(const std::string& str, const std::string& sep)
{
	StringVec ret;
	std::string::size_type start = 0, end = 0;
	while ((end = str.find(sep, start)) != std::string::npos) {
		ret.push_back(str.substr(start, end - start));
		start = end + sep.size();
	}
	ret.push_back(str.substr(start));
	return ret;
}

bool replaceString(std::string text, const std::string& key, const std::string& value)
{
	if (text.find(key) == std::string::npos)
		return false;

	std::string::size_type start = 0, pos = 0;
	while ((start = text.find(key, pos)) != std::string::npos) {
		text.replace(start, key.size(), value);
		pos = start + value.size();
	}
	return true;
}

std::string getTime()
{
	char t[80];
	time_t rawtime;
	tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(t, sizeof(t), "%H:%M:%S", timeinfo);
	char __time[120];
#ifdef _WIN32
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	sprintf(__time, "[%s.%d]", t, sysTime.wMilliseconds);
#else
	timeb s;
	ftime(&s);
	sprintf(__time, "[%s.%d]", t, s.millitm);
#endif
	return std::string(__time);
}

char *replace_string(char *str, char *orig, char *rep)
{
	static char buffer[4096];
	char *p;
	if (!(p = strstr(str, orig)))
		return str;

	strncpy(buffer, str, p-str);
	buffer[p-str] = '\0';
	sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));
	return buffer;
}

std::string transformPassword(const std::string& pass)
{
	CryptoPP::SHA1 hash;
	byte digest[CryptoPP::SHA1::DIGESTSIZE];
	hash.CalculateDigest(digest, (const byte*)pass.c_str(), pass.length());
	CryptoPP::HexEncoder encoder;
	std::string output;

	encoder.Attach(new CryptoPP::StringSink(output));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();

	return toLower(output);
}

std::string toLower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
	return str;
}

ConnectionError_t ident(const std::string& Ip)
{
#ifdef _WIN32
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		return WSA_NOT_INITED;
#endif
	SOCKET p = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (p == SOCKET_ERROR)
		return SOCKET_NOT_INITED;

	sockaddr_in service;
#ifdef _WIN32
	LPHOSTENT host;
#else
	hostent* host;
#endif
	host = gethostbyname(Ip.c_str());
	if (!host)
		return INVALID_HOST;

	service.sin_family = AF_INET;
#ifdef _WIN32
	service.sin_addr = *((LPIN_ADDR)*host->h_addr_list);
#else
	service.sin_addr = *((in_addr*)host->h_addr);
#endif
	service.sin_port = htons(113);
	if (connect(p, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
		return SOCKET_NOT_CONNECTED;

	std::string msg = "6667 : 113";
	int s = send(p, msg.c_str(), msg.length(), 0);
	if (s == SOCKET_ERROR)
		return SOCKET_UNABLE_TO_WRITE;

	char buffer[120];
	s = recv(p, buffer, sizeof(buffer), 0);
	if (s == SOCKET_ERROR)
		return SOCKET_UNABLE_TO_READ;

	return CONNECTION_SUCCESS;
}

void doWhoIs(User* from, User* dest, bool show)
{
	if (!from || !dest)
		return;

	from->sendLine(":%s 311 %s %s %s %s\r\n", from->getServer().c_str(),
		from->getNick().c_str(), dest->getNick().c_str(), dest->getIdent().c_str(),
			dest->getHost().c_str());
	std::string channels = "";
	ChansMap userChannels = *dest->getChannels();
	for (ChansMap::iterator it = userChannels.begin(); it != userChannels.end(); ++it) {
		if (!it->second)
			continue;

		if (it->second->getUserMode(dest, UM_OP))
			channels += "@" + it->first + " ";
		else if (it->second->getUserMode(dest, UM_VOICE))
			channels += "+" + it->first + " ";
		else
			channels += it->first + " ";
	}

	if (!channels.empty() && dest->getType() < UT_IRC_OP) {
		from->sendLine(":%s 319 %s %s :%s\r\n", from->getServer().c_str(),
			from->getNick().c_str(), dest->getNick().c_str(), channels.c_str());
	}

	if ((from == dest && from->getType() >= UT_IRC_OP) || from->getType() >= UT_IRC_OP) {
		from->sendLine(":%s 379 %s %s :is using modes +%s\r\n", from->getServer().c_str(),
			from->getNick().c_str(), dest->getNick().c_str(), dest->getModes().c_str());
	}
	if (from == dest)
		from->sendLine(":%s 378 %s %s :is connecting from *@%s %s\r\n", from->getServer().c_str(),
			from->getNick().c_str(), dest->getNick().c_str(), dest->getHost().c_str(), dest->getClient()->getIP().c_str());

	from->sendLine(":%s 312 %s %s :%s\r\n", from->getServer().c_str(),
		from->getNick().c_str(), dest->getNick().c_str(), g_config->getString(Config::C_SERVER_NAME).c_str());

	if (dest->getType() >= UT_IRC_OP) {
		std::stringstream ss;
		ss << ":" << from->getServer() << " 313 " << from->getNick() << " " << dest->getNick() << " :is a ";
		switch (dest->getType()) {
			case UT_IRC_OP:
				ss << "IRC operator\r\n";
				break;

			case UT_IRC_ADMINSTRATOR:
				ss << "Network Adminstrator\r\n";
				break;

			default:
				ss.str("");
				break;
		}
		if (!ss.str().empty()) {
			from->sendLine("%s", ss.str().c_str());
			ss.str("");
			ss << ":" << from->getServer() << " 310 " << from->getNick() << " " << dest->getNick() << " :is available for help.\r\n";
			from->sendLine("%s", ss.str().c_str());
		}
	}
	if (show && (from->getType() < UT_IRC_OP || from == dest || from->getNick() == dest->getNick())) {
		from->sendLine(":%s 317 %s %s %d %d :seconds idle, sigon time\r\n", from->getServer().c_str(),
			from->getNick().c_str(), dest->getNick().c_str(), dest->getIdleTime(), dest->getJoinTime());
	}
	from->sendLine(":%s 318 %s %s :End of /WHOIS list.\r\n", from->getServer().c_str(),
		from->getNick().c_str(), dest->getNick().c_str());
}

const char* getMessage(const char* str)
{
	return NULL;
}
