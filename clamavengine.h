
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

#ifndef _CLAMAVENGINE_H_
#define _CLAMAVENGINE_H_

#include <clamav.h>

#include <QObject>
#include <QThreadPool>
#include <QStringList>
#include <QDateTime>

class FileScanner;

class ClamavEngine : public QObject
{
	Q_OBJECT

private:
	QString m_db_path;
	cl_engine *m_engine;
	QStringList m_processes;
	QThreadPool *m_pool;
	qint32 m_files_count;
	bool m_dir_scan;
	bool m_mem_scan;

public:
	ClamavEngine(qint32 _thread_count = -1, const QString &_db_path = QString::null);
	virtual ~ClamavEngine();
	bool init();
	qint32 dbAge() const;
	qint32 loadDb();
	bool compile();
	bool scanFile(const QString &_file);
	bool scanDir(const QString &_dir, const QStringList &_excl_dirs = QStringList());
	bool scanMemory();
	void stop();

protected:
	virtual qint32 loadSignature(const QString &_type, const QString &_name) const;

private:
	static int sigload_cb(const char *_type, const char *_name, void *_context);
	bool scanFileThread(const QString &_file, bool _is_proc);
	bool scanDirThread(const QString &_dir, const QStringList &_excl_dirs);
	bool scanMemoryThread();

private Q_SLOTS:
	void fileScanCompletedSlot(const QString &_fd, qint32 _result, const QString &_virname, bool _is_proc);
	void fileFindedSlot(const QString &_file);
	void procFindedSlot(const QString &_file);
	
	void memScanCompletedSlot();
	void dirScanCompletedSlot();

Q_SIGNALS:
	void fileScanStartedSignal(const QString &_file);
	void procScanStartedSignal(const QString &_proc, qint32 _pid);
	void fileScanCompletedSignal(const QString &_file);
	void procScanCompletedSignal(const QString &_proc, qint32 _pid);
	void fileVirusDetectedSignal(const QString &_file, const QString &_virus);
	void procVirusDetectedSignal(const QString &_proc, qint32 _pid, const QString &_virus);
	void errorSignal(const QString &_file, const QString &_err);
	void memScanStartedSignal();
	void dirScanStartedSignal();
	void memScanCompletedSignal();
	void dirScanCompletedSignal();
	void scanStoppedSignal();
};

#endif
