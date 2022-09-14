#ifndef PATHSCANNERTHREAD_H
#define PATHSCANNERTHREAD_H

#include <QThread>

#include <QDate>
#include <QFileInfo>

class PathScannerThread : public QThread
{
	Q_OBJECT

public:
	struct SDirInfo
	{
		QString	path;
		QDate	date;
		QString dayTime;
		QString	sizeStr;
		int size;
	};

public:
	explicit PathScannerThread(QObject *parent = 0);

	~PathScannerThread();

	QString baseDir;

    QMultiHash<QDate,SDirInfo> hashDateDirInfo;

protected:
	SDirInfo dateFromPath( const QString& path ) const;

	int detDirSize( const QString& path ) const;

	bool recursiveSearch( const QFileInfo& infoItem );

	void run();

protected:
	QList<SDirInfo> dateSDirInfoList;

	QList<SDirInfo> noDateSDirInfoList;

};

#endif // PATHSCANNERTHREAD_H
