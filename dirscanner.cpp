
#include <QDir>

#include "dirscanner.h"

void DirScanner::run()
{
	Scanner::run();
	scanDir(m_dir);
}

void DirScanner::scanDir(const QString &_dir)
{
	QDir dir(_dir);
	if(m_excl_dirs.contains(dir.absolutePath()))
		return;
	QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks);
	foreach(QString d, dirs)
		scanDir(dir.absoluteFilePath(d));
	QStringList files = dir.entryList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	foreach(QString f, files)
		Q_EMIT fileFindedSignal(dir.absoluteFilePath(f));
}
