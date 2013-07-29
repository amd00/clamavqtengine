
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

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>

#include "clamavengine.h"
#include "filescanner.h"
#include "dirscanner.h"
#include "memscanner.h"

bool Scanner::m_exit = false;

int ClamavEngine::sigload_cb(const char *_type, const char *_name, void *_context)
{
		return ((ClamavEngine*)_context) -> loadSignature(_type, _name);
}

ClamavEngine::ClamavEngine(qint32 _thread_count, const QString &_db_path) :QObject(), m_db_path(_db_path), m_engine(NULL),
		m_processes(), m_pool(new QThreadPool()), m_dir_scan(false), m_mem_scan(false), m_count(0)
{
	if(m_db_path.isNull())
		m_db_path = cl_retdbdir();
	qint32 thread_count = _thread_count <= 0 ? QThread::idealThreadCount() : _thread_count;
	thread_count = (thread_count == -1 ? 1 : thread_count);
	qDebug("INFO: Thread count: %i", thread_count);
	m_pool -> setMaxThreadCount(thread_count);
}

ClamavEngine::~ClamavEngine()
{
	stop();
	if(m_engine)
		cl_engine_free(m_engine);
	m_engine = NULL;
 	delete m_pool;
}

bool ClamavEngine::init()
{
	qint32 init_res = cl_init(CL_INIT_DEFAULT);
	if(init_res)
	{
		qCritical("ERROR: Init error: %s", cl_strerror(init_res));
		Q_EMIT errorSignal(QString::null, cl_strerror(init_res));
		return false;
	}
	m_engine = cl_engine_new();
	return true;
}

qint32 ClamavEngine::dbAge() const
{
	QDir db_dir(m_db_path);
	QStringList files = db_dir.entryList(QStringList() << "*.cvd" << "*.cld", QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QDateTime res, tmp;
	foreach(QString file, files)
	{
		cl_cvd *cvd = cl_cvdhead(db_dir.absoluteFilePath(file).toLocal8Bit().data());
		if(!cvd)
			continue;
		tmp.setTime_t(cvd -> stime);
		cl_cvdfree(cvd);
		if(res.isNull())
			res = tmp;
		if(tmp > res)
			res = tmp;
	}
	return res.date().daysTo(QDate::currentDate());
}

qint32 ClamavEngine::loadDb()
{
	cl_engine_set_clcb_sigload(m_engine, ClamavEngine::sigload_cb, (void*)this);
	quint32 signo;
	int load_res = cl_load(m_db_path.toLocal8Bit().data(), m_engine, &signo, CL_DB_STDOPT);
	if(load_res)
	{
		qCritical("ERROR: Load error: %s", cl_strerror(load_res));
		Q_EMIT errorSignal(QString::null, cl_strerror(load_res));
		cl_engine_free(m_engine);
		m_engine = NULL;
		return -1;
	}
	return signo;
}

bool ClamavEngine::compile()
{
	int compile_res = cl_engine_compile(m_engine);
	if(compile_res)
	{
		qCritical("ERROR: Compile error: %s", cl_strerror(compile_res));
		Q_EMIT errorSignal(QString::null, cl_strerror(compile_res));
		cl_engine_free(m_engine);
		m_engine = NULL;
		return false;
	}
	return true;
}

bool ClamavEngine::scanFileThread(const QString &_file, bool _is_proc)
{
	if(_is_proc)
	{
		QString proc_name = QFileInfo(QFile::symLinkTarget(QFileInfo(_file).absoluteDir().absoluteFilePath("exe"))).baseName();
		QRegExp proc_reg("\\d+");
		proc_reg.indexIn(_file);
		qint32 pid = proc_reg.cap(0).toInt();
		Q_EMIT procScanStartedSignal(proc_name, pid);
	}
	else
		Q_EMIT fileScanStartedSignal(QFileInfo(_file).absoluteFilePath());
	FileScanner *scanner = new FileScanner(m_engine, _file, _is_proc);
	connect(scanner, SIGNAL(fileScanStartedSignal(const QString&)), this, SLOT(fileScanStartedSlot(const QString&)));
	connect(scanner, SIGNAL(fileScanCompletedSignal(const QString&, qint32, const QString&, bool)), this, SLOT(fileScanCompletedSlot(const QString&, qint32, const QString&, bool)));
	connect(scanner, SIGNAL(errorSignal(const QString&, const QString&)), this, SIGNAL(errorSignal(const QString&, const QString&)));
	connect(this, SIGNAL(pauseSignal()), scanner, SLOT(pauseSlot()));
	connect(this, SIGNAL(resumeSignal()), scanner, SLOT(resumeSlot()));
	m_pool -> start(scanner);
	return true;
}

bool ClamavEngine::scanDirThread(const QString &_dir, const QStringList &_excl_dirs)
{
	DirScanner *scanner = new DirScanner(_dir, _excl_dirs);
	connect(scanner, SIGNAL(dirScanStartedSignal()), this, SIGNAL(dirScanStartedSignal()));
	connect(scanner, SIGNAL(dirScanCompletedSignal()), this, SLOT(dirScanCompletedSlot()));
	connect(scanner, SIGNAL(fileFindedSignal(const QString&)), this, SLOT(fileFindedSlot(const QString&)));
	connect(this, SIGNAL(pauseSignal()), scanner, SLOT(pauseSlot()));
	connect(this, SIGNAL(resumeSignal()), scanner, SLOT(resumeSlot()));
	m_dir_scan = true;
	m_pool -> start(scanner);
	return true;
}

bool ClamavEngine::scanMemoryThread()
{
	MemScanner *scanner = new MemScanner();
	connect(scanner, SIGNAL(memScanStartedSignal()), this, SIGNAL(memScanStartedSignal()));
	connect(scanner, SIGNAL(memScanCompletedSignal()), this, SLOT(memScanCompletedSlot()));
	connect(scanner, SIGNAL(procFindedSignal(const QString&)), this, SLOT(procFindedSlot(const QString&)));
	connect(this, SIGNAL(pauseSignal()), scanner, SLOT(pauseSlot()));
	connect(this, SIGNAL(resumeSignal()), scanner, SLOT(resumeSlot()));
	m_mem_scan = true;
	m_pool -> start(scanner);
	return true;
}

bool ClamavEngine::scanFile(const QString &_file)
{
	return scanFileThread(_file, false);
}

bool ClamavEngine::scanDir(const QString &_dir, const QStringList &_excl_dirs)
{
	return scanDirThread(_dir, _excl_dirs);
}

bool ClamavEngine::scanMemory()
{
	return scanMemoryThread();
}

void ClamavEngine::stop()
{
	Scanner::setExit();
	Q_EMIT resumeSignal();
	m_pool -> waitForDone();
	Q_EMIT scanStoppedSignal();
	Scanner::setExit(false);
}

qint32 ClamavEngine::loadSignature(const QString &_type, const QString &_name) const
{
	Q_UNUSED(_type);
	Q_UNUSED(_name);
	return 0;
}

void ClamavEngine::fileScanCompletedSlot(const QString &_file, qint32 _result, const QString &_virname, bool _is_proc)
{
	switch(_result)
	{
		case CL_VIRUS:
			if(_is_proc)
			{
				qint32 pid = _file.split("_")[1].toInt();
				QString proc_name = QFileInfo(QFile::symLinkTarget(QDir("/proc/" + QString::number(pid)).absoluteFilePath("exe"))).baseName();
				Q_EMIT procVirusDetectedSignal(proc_name, pid, QString(_virname));
				qDebug("INFO: End process scanning: %s: INFECTED - %s", proc_name.toLocal8Bit().data(), _virname.toLocal8Bit().data());
			}
			else
			{
				Q_EMIT fileVirusDetectedSignal(QFileInfo(_file).absoluteFilePath(), QString(_virname));
				qDebug("INFO: End file scanning: %s: INFECTED - %s", QFileInfo(_file).absoluteFilePath().toLocal8Bit().data(), _virname.toLocal8Bit().data());
			}
			break;
		case CL_CLEAN:
			if(_is_proc)
			{
				qint32 pid = _file.split("_")[1].toInt();
				QString proc_name = QFileInfo(QFile::symLinkTarget(QDir("/proc/" + QString::number(pid)).absoluteFilePath("exe"))).baseName();
				Q_EMIT procScanCompletedSignal(proc_name, pid);
				qDebug("INFO: End process scanning: %i - %s: CLEAN", pid, proc_name.toLocal8Bit().data());
			}
			else
			{
				Q_EMIT fileScanCompletedSignal(QFileInfo(_file).absoluteFilePath());
				qDebug("INFO: End file scanning: %s: CLEAN", _file.toLocal8Bit().data());
			}
			break;
		default:
			qDebug("INFO: Error - %s", cl_strerror(_result));
			Q_EMIT errorSignal(_file, cl_strerror(_result));
	}
	m_count--;
	if(!m_count && !m_dir_scan && !m_mem_scan)
		Q_EMIT (_is_proc ? memScanCompletedSignal() : dirScanCompletedSignal());
}

void ClamavEngine::fileFindedSlot(const QString &_file)
{
	scanFileThread(_file, false);
}

void ClamavEngine::procFindedSlot(const QString &_file)
{
	scanFileThread(_file, true);
}

void ClamavEngine::memScanCompletedSlot()
{
	m_mem_scan = false;
	if(!m_count)
		Q_EMIT memScanCompletedSignal();
}

void ClamavEngine::dirScanCompletedSlot()
{
	m_dir_scan = false;
	if(!m_count)
		Q_EMIT dirScanCompletedSignal();
}
