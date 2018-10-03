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

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <boost/function.hpp>
#include <stdint.h>
#include <map>

class Task
{
	public:
		Task(uint32_t _when, const boost::function<void (void)>& _function);

		uint32_t getWhen() const { return when; }
		void operator()() const {
			m_function();
		}

		void setStopped(bool stopped) { this->stopped = stopped; }
		bool getStopped() const { return stopped; }

		friend Task* createTask(uint32_t delay, const boost::function<void (void)>& func);

	private:
		boost::function<void (void)> m_function;
		uint32_t when;
		bool stopped;
};

inline Task* createTask(uint32_t delay, const boost::function<void (void)>& func)
{
	if (!delay)
		delay = 0;

	return new Task(delay, func);
}

typedef std::map<uint32_t, Task*> TasksMap;

class Dispatcher
{
	public:
		Dispatcher();
		~Dispatcher();

		void start();

		void dispatcherThread();

		uint32_t addEvent(Task* task);
		void addTask(Task* task);
		void stopEvent(uint32_t eventId);

		void stop();

	private:
		TasksMap m_tasks;

		enum ThreadState_t
		{
			TERMINATED,
			RUNNING
		};

		ThreadState_t m_state;
		uint32_t m_id;
};
#endif
