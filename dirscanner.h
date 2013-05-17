
#ifndef _DIRSCANNER_H_
#define _DIRSCANNER_H_

#include <QObject>
#include <QRunnable>
#include <QStringList>

class DirScanner : public QObject, public QRunnable
{
	Q_OBJECT

private:
	QString m_dir;
	QStringList m_excl_dirs;

public:
	DirScanner(const QString &_dir, const QStringList &_excl_dirs)  : QObject(), QRunnable(), m_dir(_dir), m_excl_dirs(_excl_dirs) {}
	~DirScanner() {}

protected:
	void run();

private:
	void scanDir(const QString &_dir, bool _is_root = true);

Q_SIGNALS:
	void fileFindedSignal(const QString &_file);
	void dirScanCompletedSignal();
};

#endif
