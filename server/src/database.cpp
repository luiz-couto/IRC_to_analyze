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

#include "database.h"
#include "defines.h"

#include <iostream>
#include <cstdlib>
#include "auxiliar.h"
#include "configreader.h"

extern Config* g_config;

Database::Database()
{
	if (!mysql_init(&m_handle)) {
		irc_error(Database::Database, Unable to initalize mysql)
		return;
	}
	m_connected = false;
}

Database::~Database()
{
	//
}

bool Database::connect()
{
	if (m_connected)
		return true;

	if (!mysql_real_connect(&m_handle, g_config->getString(0x00).c_str(), g_config->getString(0x03).c_str(),
		g_config->getString(0x04).c_str(), g_config->getString(0x02).c_str(),
		g_config->getInt(0x01), NULL, 0)) {
		std::clog << "[Error - Database::connect] MYSQL ERROR: " << mysql_error(&m_handle) << " (" << mysql_errno(&m_handle) << ")" << std::endl;
		return false;
	}
	m_connected = true;
	return true;
}

bool Database::query(const std::stringstream& ss)
{
	if (!m_connected)
		return false;

	if (mysql_real_query(&m_handle, ss.str().c_str(), ss.str().length())) {
		int32_t error = mysql_errno(&m_handle);
		if (error == CR_SERVER_LOST || error == CR_SERVER_GONE_ERROR)
			m_connected = false;

		std::clog << "[Error - Database::query] mysql_real_query(): " << ss.str() << " - MYSQL ERROR: " << mysql_error(&m_handle) << " (" << error << ")" << std::endl;
		return false;
	}
	if (MYSQL_RES* tmp = mysql_store_result(&m_handle)) {
		mysql_free_result(tmp);
		tmp = NULL;
	}
	return true;
}

Result* Database::verfiyResult(Result *r)
{
	if(r->next())
		return r;

	r->free();
	r = NULL;
	return NULL;
}

Result* Database::storeQuery(const std::stringstream& ss)
{
	if (!m_connected)
		return NULL;

	int32_t error = 0;
	if (mysql_real_query(&m_handle, ss.str().c_str(), ss.str().length())) {
		error = mysql_errno(&m_handle);
		if (error == CR_SERVER_LOST || error == CR_SERVER_GONE_ERROR) {
			irc_warning(Database::storeQuery, Lost connection to mysql);
			irc_notice(Database::storeQuery, Attempting to reconnect..);
			m_connected = false;
			connect();
		}
		std::clog << "[Error - Database::storeQuery] mysql_real_query(): " << ss.str() << " - MYSQL ERROR: " << mysql_error(&m_handle) << " (" << error << ")" << std::endl;
		return NULL;
	}
	if (MYSQL_RES* presult = mysql_store_result(&m_handle)) {
		Result* result = new Result(presult);
		return verfiyResult(result);
	}

	error = mysql_errno(&m_handle);
	if (error == CR_UNKNOWN_ERROR || error == CR_SERVER_LOST)
		return NULL;

	std::clog << "[Error - Database::storeQuery] mysql_real_query(): " << ss.str() << " - MYSQL ERROR: " << mysql_error(&m_handle) << " (" << error << ")" << std::endl;
	return NULL;
}

Result::Result(MYSQL_RES* res)
{
	if (!res)
		return;

	m_handle = res;
	m_listNames.clear();

	int32_t i = 0;
	MYSQL_FIELD* field = NULL;
	while ((field = mysql_fetch_field(m_handle)))
		m_listNames[field->name] = i++;
}

Result::~Result()
{
	if (!m_handle)
		return;

	mysql_free_result(m_handle);
	m_listNames.clear();
}

int32_t Result::getDataInt(const std::string& s)
{
	ListNames::iterator it = m_listNames.find(s);
	if (it != m_listNames.end())
		return m_row[it->second] ? atoi(m_row[it->second]) : -1;

	std::clog << "[Error - Result::getDataInt] " << s << "." << std::endl;
	return -1;
}

int64_t Result::getDataLong(const std::string& s)
{
	ListNames::iterator it = m_listNames.find(s);
	if (it != m_listNames.end())
		return m_row[it->second] ? atoll(m_row[it->second]) : 0;

	std::clog << "[Error - Result::getDataLong] " << s << "." << std::endl;
	return -1;
}

std::string Result::getDataString(const std::string& s)
{
	ListNames::iterator it = m_listNames.find(s);
	if (it != m_listNames.end())
		return m_row[it->second] ? std::string(m_row[it->second]) : std::string();

	std::clog << "[Error - Result::getDataString] " << s << "." << std::endl;
	return std::string();
}

bool Result::next()
{
	m_row = mysql_fetch_row(m_handle);
	return m_row;
}

bool Result::free()
{
	if (!m_handle) {
		irc_error(Result::free, Trying to free already freed result!!!!)
		return false;
	}

	mysql_free_result(m_handle);
	m_handle = NULL;
	m_listNames.clear();
	delete this;
	return true;
}
