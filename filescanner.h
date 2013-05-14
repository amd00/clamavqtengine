
#ifndef _FILESCANNER_H_
#define _FILESCANNER_H_

#include <QRunnable>
#include <QObject>

struct cl_engine;

class FileScanner : public QObject, public QRunnable
{
	Q_OBJECT

private:
	QString m_file;
	bool m_is_proc;
	cl_engine *m_engine;
	
public:
	FileScanner(cl_engine *_engine,const QString &_file, bool _is_proc, QObject *_par);
	~FileScanner();
	
protected:
	void run();
	
Q_SIGNALS:
	void fileScannedSignal(const QString &_fd, qint32 _result, const QString &_virname, bool _is_proc);
	void errorSignal(const QString &_file, const QString &_err);
};

#endif
