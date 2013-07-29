
/*
 *  Copyright (C) 2013 Andrey Dudakov
 *
 *  Authors: Andrey "amd00" Dudakov
 *
 *  This file is part of clamavqtengine.
 *
 *  clamavqtengine is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  clamavqtengine is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with clamavqtengine.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <QObject>
#include <QRunnable>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>

class Scanner : public QObject, public QRunnable
{
	Q_OBJECT
	
private:
	static bool m_exit;
	QMutex m_mutex;
	QWaitCondition m_pause_manager;
	bool m_is_paused;
	
protected:
	void checkPause() { if(m_is_paused) m_pause_manager.wait(&m_mutex); }
	class Sleeper : public QThread
	{
	public:
		static void usleep(unsigned long usecs) { QThread::usleep(usecs); }
		static void msleep(unsigned long msecs) { QThread::msleep(msecs); }
		static void sleep(unsigned long secs) { QThread::sleep(secs); }
	};
	
public:
	Scanner() : m_mutex(), m_pause_manager(), m_is_paused(false) {}
	virtual ~Scanner() {}
 	static void setExit(bool _exit = true) { Scanner::m_exit = _exit; }
	static bool exit() { return Scanner::m_exit; }

public Q_SLOTS:
	void pauseSlot() { m_is_paused = true; }
	void resumeSlot() { m_pause_manager.wakeAll(); m_is_paused = false; }
	
Q_SIGNALS:
	void resumeSignal();
};

#endif
