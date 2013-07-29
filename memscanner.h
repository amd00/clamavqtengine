
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

#ifndef _MEMSCANNER_H_
#define _MEMSCANNER_H_

#include "scanner.h"

class MemScanner : public Scanner
{
	Q_OBJECT

public:
	MemScanner()  : Scanner() {}
	~MemScanner() {}

protected:
	void run();

private:
	void scanMemory();

Q_SIGNALS:
	void procFindedSignal(const QString &_file);
	void fileScanStartedSignal(const QString &_file);
	void fileScanCompletedSignal(const QString &_fd, qint32 _result, const QString &_virname, bool _is_proc);
	void errorSignal(const QString &_file, const QString &_err);
	void memScanStartedSignal();
	void memScanCompletedSignal();
};

#endif
