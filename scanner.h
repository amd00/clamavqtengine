
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
// #include <QEventLoop>

class QThreadPool;

class Scanner : public QObject, public QRunnable
{
	Q_OBJECT
	
private:
	static bool m_exit;
	QThreadPool *m_pool;
	cl_engine *m_engine;
	QMutex m_mutex;
	QWaitCondition m_pause_manager;
	bool m_is_paused;
// 	QEventLoop m_loop;
	
protected:
	QThreadPool *pool() const { return m_pool; }
	cl_engine *engine() const { return m_engine; }
	void checkPause() { if(m_is_paused) m_pause_manager.wait(&m_mutex); }
	
public:
	Scanner(QThreadPool *_pool, cl_engine *_engine) : m_pool(_pool), m_engine(_engine), 
		m_mutex(), m_pause_manager(), m_is_paused(false) {}
	virtual ~Scanner() {}
 	static void setExit(bool _exit = true) { Scanner::m_exit = _exit; }
	static bool exit() { return Scanner::m_exit; 

public Q_SLOTS:
	void pauseSlot() { m_is_paused = true; }
	void resumeSlot() { m_pause_manager.wakeAll(); m_is_paused = false; }
	
Q_SIGNALS:
	void resumeSignal();
};

#endif
