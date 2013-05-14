
#ifndef _MEMSCANNER_H_
#define _MEMSCANNER_H_

#include <QObject>
#include <QRunnable>

class MemScanner : public QObject, public QRunnable
{
	Q_OBJECT

public:
	MemScanner()  : QObject(), QRunnable() {}
	~MemScanner() {}

protected:
	void run();

private:
	void scanMemory();

Q_SIGNALS:
	void procFindedSignal(const QString &_file);
	void memEndScanSignal();
};

#endif
