#include "PathScannerThread.h"

#include <QDir>
#include <QRegExp>
#include <QStringList>

PathScannerThread::PathScannerThread(QObject *parent) :
	QThread(parent)
{
}

PathScannerThread::SDirInfo PathScannerThread::dateFromPath(const QString& path) const
{
	QDate toReturnDate;

	QString toInterpret = path.mid(this->baseDir.count());
	QRegExp regexp("\\d{1,2}\\.\\d{1,2}\\.\\d{4}");
	int index = regexp.indexIn(toInterpret);
	if (index > -1)
	{
		toReturnDate = QDate::fromString(regexp.capturedTexts().first(), "d.M.yyyy");
	}
	else
	{
		regexp.setPattern("\\d{4}\\.\\d{1,2}\\.\\d{1,2}");
		index = regexp.indexIn(toInterpret);
		if (index > -1)
		{
			toReturnDate = QDate::fromString(regexp.capturedTexts().first(), "yyyy.M.d");
		}
		else
		{
			regexp.setPattern("\\d{4}-\\d{1,2}-\\d{1,2}");
			index = regexp.indexIn(toInterpret);
			if (index > -1)
			{
				toReturnDate = QDate::fromString(regexp.capturedTexts().first(), "yyyy-M-d");
			}
		}
	}

	if (index > -1)
	{
		toInterpret = toInterpret.mid(index + 10);
	}
	toInterpret = toInterpret.section('/', -1, -1);
	regexp.setPattern("^[^A-Za-z]*");
	if (regexp.indexIn(toInterpret) == 0)
	{
		toInterpret = toInterpret.mid(regexp.capturedTexts().first().length()).simplified();
	}

	SDirInfo toReturn;
	toReturn.date = toReturnDate;
	toReturn.dayTime = toInterpret;
	toReturn.path = path;

	int sizes = this->detDirSize(path);
	toReturn.size = sizes;

	toReturn.sizeStr = QString("%1 MB").arg(toReturn.size);

	return toReturn;
}

int PathScannerThread::detDirSize(const QString& path) const
{
	int size = 0;

	QFileInfoList list = QDir(path).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
	foreach (const QFileInfo& item, list)
	{
		if (item.isDir())
		{
			int sizes = this->detDirSize(item.absoluteFilePath());
			size += sizes;
		}
		else
		{
			quint64 singleSize = item.size();
			int blocks = ((singleSize + 0x000fffff) >> 20);
			size += blocks;
		}
	}

	return size;
}

bool PathScannerThread::recursiveSearch(const QFileInfo& infoItem)
{
	QFileInfoList infoList = QDir(infoItem.absoluteFilePath()).entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
	foreach (const QFileInfo& item, infoList)
	{
		if ( ! recursiveSearch(item))
		{
			QString path = item.absoluteFilePath();
			SDirInfo dirInfo = PathScannerThread::dateFromPath(path);
			if (dirInfo.date.isValid())
			{
				this->dateSDirInfoList << dirInfo;
			}
			else
			{
				this->noDateSDirInfoList << dirInfo;
			}
		}
	}
	return infoList.count();
}

void PathScannerThread::run()
{
//	emit signalReadingStarted();

	this->dateSDirInfoList.clear();
	this->noDateSDirInfoList.clear();

	recursiveSearch(QFileInfo(this->baseDir));

	QHash<QDate,SDirInfo> newDateHash;
	foreach (const SDirInfo& item, this->dateSDirInfoList)
	{
		newDateHash.insertMulti(item.date, item);
	}

	this->hashDateDirInfo.swap(newDateHash);
}
