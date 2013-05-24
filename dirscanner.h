
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

#ifndef _DIRSCANNER_H_
#define _DIRSCANNER_H_

#include <QStringList>

#include "scanner.h"

class DirScanner : public Scanner
{
	Q_OBJECT

private:
	QString m_dir;
	QStringList m_excl_dirs;

public:
	DirScanner(const QString &_dir, const QStringList &_excl_dirs)  : Scanner(), m_dir(_dir), m_excl_dirs(_excl_dirs) {}
	~DirScanner() {}

protected:
	void run();

private:
	void scanDir(const QString &_dir);

Q_SIGNALS:
	void fileFindedSignal(const QString &_file);
};

#endif
