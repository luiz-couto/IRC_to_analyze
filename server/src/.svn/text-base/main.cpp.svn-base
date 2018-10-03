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

#include "mainframe.h"
#include "database.h"
#include "configreader.h"
#include "auxiliar.h"
#include "channels.h"
#include "dispatcher.h"

#include <iostream>
#include <cstdlib>

#ifdef _WIN32
	#include <conio.h>
#endif

Config* g_config;
Channels* g_channels;
MainFrame* g_mainFrame;
Dispatcher* g_dispatcher;

void allocationFailure()
{
	std::clog << "Allocation failed, server is out of memory." << std::endl;
#ifdef _WIN32
	getch();
#endif
}

int main(int argc, char **argv)
{
	std::set_new_handler(allocationFailure);
	g_config = new Config;
	std::clog << "Loading config.conf....";
	if (!g_config->load()) {
		std::clog << "[Failed]" << std::endl;
#ifdef _WIN32
		std::system("pause");
#endif
		delete g_config;
		return 1;
	}
	std::clog << "[Success]" << std::endl;
	std::clog << "Connecting to database...";
	Database *db = Database::instance();
	if (!db->connect()) {
		std::clog << "[Failed]" << std::endl;
#ifdef _WIN32
		std::system("pause");
#endif
		delete db;
		return 1;
	}
	std::clog << "[Success]" << std::endl;
	std::clog << "Initliazing channels...";
	g_channels = new Channels();
	if (!g_channels->init()) {
		std::clog << "[Failed]" << std::endl;
#ifdef _WIN32
		std::system("pause");
#endif
		delete g_channels;
		return 1;
	}
	std::clog << "[Success]" << std::endl;
	std::clog << "Initalizing dispatcher....";
	g_dispatcher = new Dispatcher();
	g_dispatcher->start();
	std::clog << "[Success]" << std::endl;
	std::clog << "Server is starting up...." << std::endl;
	std::clog<< "Binding ports: ";
	StringVec ports = splitString(g_config->getString(Config::C_PORTS), ";");
	if (!ports.size() || ports.size() < 1) {
		std::clog << "\nERROR: No ports provided, exiting..." << std::endl;
#ifdef _WIN32
		std::system("pause");
#endif
		return 1;
	}
	for (size_t i = 0; i < ports.size(); ++i)
		std::clog << ports[i] << "\t";

	std::clog << std::endl;
	g_mainFrame = new MainFrame;
	std::clog << g_config->getString(Config::C_SERVER_NAME) << " Server online!" << std::endl;
	g_mainFrame->start();
	return 0;
}
