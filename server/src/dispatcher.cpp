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

#include "dispatcher.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <ctime>

Task::Task(uint32_t _when, const boost::function<void (void)>& _function)
	: m_function(_function)
{
	stopped = false;
	when = time(NULL) + _when;
}

Dispatcher::Dispatcher()
{
	m_id = 0;
	m_state = TERMINATED;
}

Dispatcher::~Dispatcher()
{
	for (TasksMap::iterator it = m_tasks.begin(); it != m_tasks.end(); ++it)
		delete it->second;

	m_tasks.clear();
}

void Dispatcher::start()
{
	if (m_state == TERMINATED)
		(void) new boost::thread(boost::bind(&Dispatcher::dispatcherThread, this));
	else
		std::clog << "[Error - Dispatcher::start] Called while threading is still running." << std::endl;
}

void Dispatcher::addTask(Task* task)
{
	m_tasks.insert(std::make_pair(0, task));
}

uint32_t Dispatcher::addEvent(Task* task)
{
	++m_id;
	m_tasks.insert(std::make_pair(m_id, task));
	return m_id;
}

void Dispatcher::stopEvent(uint32_t eventId)
{
	TasksMap::iterator it = m_tasks.find(eventId);
	if (it == m_tasks.end())
		return;

	it->second->setStopped(true);
	delete it->second;
	m_tasks.erase(it);
}

void Dispatcher::stop()
{
	if (m_state == RUNNING)
		m_state = TERMINATED;
	else
		std::clog << "[Error - Dispatcher::stop] Called while thread is termianted." << std::endl;

	for (TasksMap::iterator it = m_tasks.begin(); it != m_tasks.end(); ++it)
		it->second->setStopped(true);
}

void Dispatcher::dispatcherThread()
{
	m_state = RUNNING;
	while (m_state == RUNNING) {
		for (TasksMap::iterator it = m_tasks.begin(); it != m_tasks.end(); ++it) {
			if (!it->second)
				continue;

			if (it->second->getStopped())
				continue;

			if (static_cast<uint32_t>(std::time(NULL)) >= it->second->getWhen()) {
				(*it->second)();
				m_tasks.erase(it);
				break;
			}
		}
	}
}
