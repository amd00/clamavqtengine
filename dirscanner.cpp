
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

#include "dirscanner.h"

void DirScanner::run()
{
	Scanner::run();
	scanDir(m_dir);
}

void DirScanner::scanDir(const QString &_dir)
{
	QDir dir(_dir);
	if(m_excl_dirs.contains(dir.absolutePath()))
		return;
	QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks);
	foreach(QString d, dirs)
		scanDir(dir.absoluteFilePath(d));
	QStringList files = dir.entryList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	foreach(QString f, files)
		Q_EMIT fileFindedSignal(dir.absoluteFilePath(f));
}
