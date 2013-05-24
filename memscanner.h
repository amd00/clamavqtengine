
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
};

#endif
