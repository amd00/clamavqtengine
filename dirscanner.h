
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
