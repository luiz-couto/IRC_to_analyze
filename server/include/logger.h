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

#ifndef LOGGER_H
#define LOGGER_H

#include <string>

typedef enum
{
	LOG_NONE,
	LOG_WARNING,
	LOG_ERROR,
	LOG_NOTICE
} LogType_t;

typedef enum
{
	LOGFILE_NONE,
	LOGFILE_COMMANDS,
	LOGFILE_WARNINGS,
	LOGFILE_ERRORS,
	LOGFILE_AUTHS,
	LOGFILE_CHANNELS
} LogFile_t;

class Logger
{
	public:
		Logger();
		~Logger();

		static Logger *getInstance()
		{
			static Logger instance;
			return &instance;
		}

		void Log(LogFile_t fileType, LogType_t type, const std::string& n,
			bool newLine, const char *fmt, ...);
};
#endif
