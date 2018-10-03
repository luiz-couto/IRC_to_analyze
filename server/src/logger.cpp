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

#include "logger.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

Logger::Logger()
{
	//
}

Logger::~Logger()
{
	//
}

void Logger::Log(LogFile_t fileType, LogType_t type, const std::string& n, bool newLine, 
	const char *fmt, ...)
{
	std::string file = "IrcLogs";
	switch(fileType) {
		case LOGFILE_COMMANDS:
		{
			file += "/commands.log";
			break;
		}
		case LOGFILE_WARNINGS:
		{
			file += "/warnings.log";
			break;
		}
		case LOGFILE_ERRORS:
		{
			file += "/errors.log";
			break;
		}
		case LOGFILE_AUTHS:
		{
			file += "/auths.log";
			break;
		}
		case LOGFILE_CHANNELS:
		{
			file += "/channels/";
			if (n != "")
				file += n + ".log";
			else
				file += "channels.log";
			break;
		}
		case LOGFILE_NONE:
		default:
			break;
	}
	FILE *fp = fopen(file.c_str(), "a+");
	if (!fp)
		(void) std::system("mkdir IrcLogs");

	std::string str = "";
	switch (type) {
		case LOG_WARNING:
		{
			str = "[Warning] ";
			break;
		}
		case LOG_ERROR:
		{
			str = "[Error] ";
			break;
		}
		case LOG_NOTICE:
		{
			str = "[Notice] ";
			break;
		}
		case LOG_NONE:
		default:
		{
			break;
		}
	}
	va_list p;
	va_start(p, fmt);
	char buffer[512];
	vsprintf(buffer, fmt, p);
	va_end(p);
	str += std::string(buffer);
	if (newLine)
		str += "\n";

	fprintf(fp, "%s", str.c_str());
	fclose(fp);
}
