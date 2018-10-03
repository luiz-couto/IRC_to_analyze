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

#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <string>
#include <map>
#include <set>
#include <stdio.h>

struct Node
{
	char *content;
	Node()
	{ content = new char[256]; }
	friend Node* createNode(char *content);
};

typedef std::map<std::string, Node*> Nodes;

inline Node* createNode(char *content)
{
	Node *n = new Node;
	sprintf(n->content, "%s", content);
	return n;
}

struct Root
{
	char *name;
	Nodes nodes;

	Nodes::const_iterator 
	beginNodes()
	{ return nodes.begin(); }

	Nodes::const_iterator 
	endNodes()
	{ return nodes.end(); }

	template<typename __T, typename __V>
	inline void
	insert(__T* k, __V* v)
	{ nodes.insert(std::make_pair(*k, v)); }

	Root()
	{ name = new char[256]; }

	~Root() {
		for (Nodes::iterator it = nodes.begin(); it != nodes.end(); ++it)
			delete it->second;

		nodes.clear();
	}
};

typedef std::set<Root*> Roots;

class Config
{
	public:
		Config();
		~Config();

		bool load();
		bool reload();

		std::string getString(int arg);
		int getInt(int arg);
		Node* getNodeValue(const std::string& nodeName);

		typedef enum
		{
			C_MYSQL_HOST = 0x00,
			C_MYSQL_PORT = 0x01,
			C_MYSQL_DB = 0x02,
			C_MYSQL_USER = 0x03,
			C_MYSQL_PASS = 0x04,
			C_HOST = 0x05,
			C_PORTS = 0x06,
			C_SERVER_NAME = 0x07,
			C_SERVER_HOST = 0x08,
			C_MAX_PENDING_CONNECTIONS = 0x09,
			C_EXECUTABLE_NAME = 0x20
		} Config_t;

	private:
		typedef std::map<Config_t, std::string> ConfigMap;
		ConfigMap m_config;
		Roots m_roots;
		bool m_loaded;
};
#endif
