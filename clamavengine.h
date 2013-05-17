
#ifndef _CLAMAVENGINE_H_
#define _CLAMAVENGINE_H_

#include <clamav.h>

#include <QObject>
#include <QThreadPool>
#include <QStringList>
#include <QDateTime>

class ClamavEngine : public QThreadPool
{
	Q_OBJECT

private:
	QString m_db_path;
	cl_engine *m_engine;
	quint32 m_queue_size;
	QStringList m_processes;
	bool m_dir_scanning;

public:
	ClamavEngine(qint32 _thread_count = -1, const QString &_db_path = QString::null);
	virtual ~ClamavEngine();
	bool init();
	qint32 dbAge() const;
	qint32 loadDb();
	bool compile();
	bool scanFile(const QString &_file);
	bool scanDir(const QString &_dir, const QStringList &_excl_dirs = QStringList());
	bool scanMemory();

protected:
	virtual qint32 loadSignature(const QString &_type, const QString &_name) const;

private:
	static int sigload_cb(const char *_type, const char *_name, void *_context);
	bool scanFileThread(const QString &_file, bool _is_proc);
	bool scanDirThread(const QString &_dir, const QStringList &_excl_dirs);
	bool scanMemoryThread();

private Q_SLOTS:
	void fileScanCompletedSlot(const QString &_fd, qint32 _result, const QString &_virname, bool _is_proc);
	void memScanCompletedSlot();
	void dirScanCompletedSlot();
	void fileFindedSlot(const QString &_file);
	void procFindedSlot(const QString &_file);

Q_SIGNALS:
	void fileScanStartedSignal(const QString &_file);
	void procScanStartedSignal(const QString &_proc, qint32 _pid);
	void fileScanCompletedSignal(const QString &_file);
	void procScanCompletedSignal(const QString &_proc, qint32 _pid);
	void fileVirusDetectedSignal(const QString &_file, const QString &_virus);
	void procVirusDetectedSignal(const QString &_proc, qint32 _pid, const QString &_virus);
	void errorSignal(const QString &_file, const QString &_err);
	void memScanCompletedSignal();
	void dirScanCompletedSignal();
};

#endif
