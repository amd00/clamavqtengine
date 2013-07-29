
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

#ifndef _FILESCANNER_H_
#define _FILESCANNER_H_

#include <QFile>

#include "scanner.h"

class cl_engine;

class FileScanner : public Scanner
{
	Q_OBJECT

private:
	cl_engine *m_engine;
	QString m_file;
	bool m_is_proc;
	class FileRemover
	{
	private:
		QString m_file;

	public:
		FileRemover(const QString &_file) : m_file(_file) {}
		~FileRemover() { if(!m_file.isNull()) QFile::remove(m_file); }
	};
	
public:
	FileScanner(cl_engine *_engine, const QString &_file, bool _is_proc) : Scanner(), m_engine(_engine), m_file(_file), m_is_proc(_is_proc) {}
	~FileScanner() {}

protected:
	void run();

Q_SIGNALS:
	void fileScanStartedSignal(const QString &_file);
	void fileScanCompletedSignal(const QString &_fd, qint32 _result, const QString &_virname, bool _is_proc);
	void errorSignal(const QString &_file, const QString &_err);
};

#endif
