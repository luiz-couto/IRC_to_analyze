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

#ifndef DATABASE_H
#define DATABASE_H

#ifdef WIN32
	#include <winsock2.h>
#endif

#include <mysql/mysql.h>
#include <mysql/errmsg.h>
#include <map>
#include <sstream>
#include <stdint.h>

class Result;

class Database
{
	public:
		Database();
		~Database();
		static Database* instance()
		{
			static Database ins;
			return &ins;
		}

		bool connect();

		bool query(const std::stringstream& ss);

		Result* storeQuery(const std::stringstream& q);

		Result* verfiyResult(Result *r);

	private:
		MYSQL m_handle;
		bool m_connected;
};

class Result
{
	public:
		Result(MYSQL_RES* res);
		~Result();

		int32_t getDataInt(const std::string& s);
		int64_t getDataLong(const std::string& s);
		std::string getDataString(const std::string& s);

		bool free();
		bool next();

	private:
		typedef std::map<const std::string, uint32_t> ListNames;
		ListNames m_listNames;
		MYSQL_RES* m_handle;
		MYSQL_ROW m_row;
};
#endif
