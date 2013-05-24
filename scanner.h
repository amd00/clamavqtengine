
#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <QObject>
#include <QRunnable>
#include <QThread>

class Scanner : public QObject, public QRunnable
{
	Q_OBJECT
	
public:
	Scanner() {}
	virtual ~Scanner() {}
	virtual void run() { Q_EMIT threadStartedSignal(QThread::currentThread());}
	
Q_SIGNALS:
	void threadStartedSignal(QThread *_thread);
};

#endif
