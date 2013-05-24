
#include <QFile>
#include <QObject>
#include <QRegExp>
#include <QFileInfo>
#include <QDir>

#include <clamav.h>

#include "filescanner.h"

void FileScanner::run()
{
	Scanner::run();
	QFile f(m_file);
	if(!f.open(QIODevice::ReadOnly))
	{
		qCritical("ERROR: Open file error: %s : %s", m_file.toLocal8Bit().data(), f.errorString().toLocal8Bit().data());
		Q_EMIT errorSignal(m_file, f.errorString());
		return;
	}
	if(m_is_proc)
	{
		qint32 pid = m_file.split("_")[1].toInt();
			QString proc_name = QFileInfo(QFile::symLinkTarget(QDir("/proc/" + QString::number(pid)).absoluteFilePath("exe"))).baseName();
		qDebug("INFO: Scanning process: %s", proc_name.toLocal8Bit().data());
	}
	else
		qDebug("INFO: Scanning file: %s", m_file.toLocal8Bit().data());
	const char *virname = NULL;
	long unsigned int scanned = 0;
	int result = cl_scandesc(f.handle(), &virname, &scanned, m_engine, CL_SCAN_STDOPT);
	f.close();
	Q_EMIT fileScanCompletedSignal(m_file, result, virname, m_is_proc);
}
