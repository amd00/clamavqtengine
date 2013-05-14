
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QTemporaryFile>

#include "clamavengine.h"
#include "filescanner.h"
#include "dirscanner.h"
#include "memscanner.h"

int ClamavEngine::sigload_cb(const char *_type, const char *_name, void *_context)
{
		return ((ClamavEngine*)_context) -> loadSignature(_type, _name);
}

ClamavEngine::ClamavEngine(qint32 _thread_count, const QString &_db_path) :QThreadPool(), m_db_path(_db_path), m_engine(NULL), m_queue_size(0), 
		m_processes(), m_dir_scanning(false)
{
	if(m_db_path.isNull())
		m_db_path = cl_retdbdir();
	qint32 thread_count = _thread_count <= 0 ? QThread::idealThreadCount() : _thread_count;
	thread_count = (thread_count == -1 ? 1 : thread_count);
	qDebug("INFO: Thread count: %i", thread_count);
	setMaxThreadCount(thread_count);
}

ClamavEngine::~ClamavEngine()
{
	if(m_engine)
		cl_engine_free(m_engine);
	m_engine = NULL;
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
		Q_EMIT procStartScanSignal(proc_name, pid);
	}
	else
		Q_EMIT fileStartScanSignal(QFileInfo(_file).absoluteFilePath());
	FileScanner *scanner = new FileScanner(m_engine, _file, _is_proc, this);
	connect(scanner, SIGNAL(fileScannedSignal(const QString&, qint32, const QString&, bool)), this, SLOT(fileScannedSlot(const QString&, qint32, const QString&, bool)));
	connect(scanner, SIGNAL(errorSignal(const QString&, const QString&)), this, SIGNAL(errorSignal(const QString&, const QString&)));
	m_queue_size++;
	start(scanner);
	return true;
}

bool ClamavEngine::scanDirThread(const QString &_dir, const QStringList &_excl_dirs)
{
	DirScanner *scanner = new DirScanner(_dir, _excl_dirs);
	connect(scanner, SIGNAL(fileFindedSignal(const QString&)), this, SLOT(fileFindedSlot(const QString&)));
	connect(scanner, SIGNAL(dirEndScanSignal()), this, SLOT(dirEndScanSlot()));
	start(scanner);
	return true;
}

bool ClamavEngine::scanMemoryThread()
{
	MemScanner *scanner = new MemScanner();
	connect(scanner, SIGNAL(procFindedSignal(const QString&)), this, SLOT(fileFindedSlot(const QString&)));
	connect(scanner, SIGNAL(memEndScanSignal()), this, SLOT(endScanSlot()));
	start(scanner);
	return true;
}

bool ClamavEngine::scanFile(const QString &_file)
{
	return scanFileThread(_file, false);
}

bool ClamavEngine::scanDir(const QString &_dir, const QStringList &_excl_dirs)
{
	m_dir_scanning = true;
	return scanDirThread(_dir, _excl_dirs);
}

bool ClamavEngine::scanMemory()
{
	m_dir_scanning = true;
	return scanMemoryThread();
}

qint32 ClamavEngine::loadSignature(const QString &_type, const QString &_name) const
{
	Q_UNUSED(_type);
	Q_UNUSED(_name);
	return 0;
}

void ClamavEngine::fileScannedSlot(const QString &_file, qint32 _result, const QString &_virname, bool _is_proc)
{
	m_queue_size--;
	switch(_result)
	{
		case CL_VIRUS:
			if(_is_proc)
			{
				QFile::remove(_file);
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
				QFile::remove(_file);
				qint32 pid = _file.split("_")[1].toInt();
				QString proc_name = QFileInfo(QFile::symLinkTarget(QDir("/proc/" + QString::number(pid)).absoluteFilePath("exe"))).baseName();
				Q_EMIT procEndScanSignal(proc_name, pid);
				qDebug("INFO: End process scanning: %i - %s: CLEAN", pid, proc_name.toLocal8Bit().data());
			}
			else
			{
				Q_EMIT fileEndScanSignal(QFileInfo(_file).absoluteFilePath());
				qDebug("INFO: End file scanning: %s: CLEAN", _file.toLocal8Bit().data());
			}
			break;
		default:
			qDebug("INFO: Error - %s", cl_strerror(_result));
			Q_EMIT errorSignal(_file, cl_strerror(_result));
	}
	if(!m_queue_size && !m_dir_scanning)
		Q_EMIT endScanSignal();
}

void ClamavEngine::fileFindedSlot(const QString &_file)
{
	scanFileThread(_file, false);
}

void ClamavEngine::endScanSlot()
{
	m_dir_scanning = false;
	if(!m_queue_size)
		Q_EMIT endScanSignal();
}