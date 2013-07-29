
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

#include <QDir>
#include <QTextStream>
#include <QTemporaryFile>
#include <QThreadPool>

#include "memscanner.h"
#include "filescanner.h"

void MemScanner::run()
{
	Q_EMIT memScanStartedSignal();
	scanMemory();
}

void MemScanner::scanMemory()
{
	QDir proc_dir("/proc");
	QStringList proc_list = proc_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot).filter(QRegExp("\\d+"));
	QRegExp mem_addr_regex("^([0-9a-fA-F]+)-([0-9a-fA-F]+)\\sr");
	foreach(QString proc, proc_list)
	{
		checkPause();
		if(Scanner::exit())
			break;
		QString maps_file_str(QDir(proc_dir.absoluteFilePath(proc)).absoluteFilePath("maps"));
		QString mem_file_str(QDir(proc_dir.absoluteFilePath(proc)).absoluteFilePath("mem"));
		QFile maps_file(maps_file_str);
		QFile mem_file(mem_file_str);
		
		if(!maps_file.open(QIODevice::ReadOnly))
		{
			qCritical("ERROR: Open file error: %s", maps_file.errorString().toLocal8Bit().data());
			continue;
		}
		if(!mem_file.open(QIODevice::ReadOnly))
		{
			qCritical("ERROR: Open file error: %s", mem_file.errorString().toLocal8Bit().data());
			continue;
		}
		QTextStream f_str(&maps_file);
		QTemporaryFile f(QDir::temp().absoluteFilePath("proc_" + proc + "_XXXXXXX"));
		f.setAutoRemove(false);
		f.open();
		for(QString line = f_str.readLine(); !line.isNull(); line = f_str.readLine())
		{
			qint32 pos = mem_addr_regex.indexIn(line);
			if(!pos)
			{
				quint32 start = mem_addr_regex.capturedTexts()[1].toUInt(NULL, 16);
				quint32 end = mem_addr_regex.capturedTexts()[2].toUInt(NULL, 16);
				mem_file.seek(start);
				QByteArray mem = mem_file.read(end - start);
				f.write(mem);
			}
		}
		f.close();
		Q_EMIT procFindedSignal(f.fileName());
	}
	Q_EMIT memScanCompletedSignal();
}
