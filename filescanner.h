
#ifndef _FILESCANNER_H_
#define _FILESCANNER_H_

#include "scanner.h"

struct cl_engine;

class FileScanner : public Scanner
{
	Q_OBJECT

private:
	QString m_file;
	bool m_is_proc;
	cl_engine *m_engine;
	
public:
	FileScanner(cl_engine *_engine,const QString &_file, bool _is_proc) : Scanner(), m_file(_file), m_is_proc(_is_proc), m_engine(_engine) {}
	~FileScanner() {}
	
protected:
	void run();
	
Q_SIGNALS:
	void fileScanCompletedSignal(const QString &_fd, qint32 _result, const QString &_virname, bool _is_proc);
	void errorSignal(const QString &_file, const QString &_err);
};

#endif
