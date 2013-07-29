
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

#include <QObject>
#include <QRegExp>
#include <QFileInfo>
#include <QDir>

#include <clamav.h>

#include "filescanner.h"

void FileScanner::run()
{
	FileRemover frm(m_is_proc ? m_file : QString::null);
	
	checkPause();
	if(Scanner::exit())
		return;
	
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
	Q_EMIT fileScanStartedSignal(m_file);
	
	const char *virname = NULL;
	long unsigned int scanned = 0;
	int result = cl_scandesc(f.handle(), &virname, &scanned, m_engine, CL_SCAN_STDOPT);
	f.close();

	Q_EMIT fileScanCompletedSignal(m_file, result, virname, m_is_proc);
}
