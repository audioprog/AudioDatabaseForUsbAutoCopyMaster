#ifndef LSFILELIST_H
#define LSFILELIST_H

#include <QThread>

#include <QDate>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>

class lsGlobalSettings;

class LsFileList : public QThread
{
    Q_OBJECT

public:
    explicit LsFileList(QObject *parent = 0);
    ~LsFileList();

signals:
	void signalReadingFinished();

public:

	void changeCurrent( const QString& subPath, const QDate& newDate, const QString& newDateTime );

	QString detPath( const QString& basePath );

	const QDate& getDate() const { return this->date; }
	const QString& getDateTime() const { return this->dateTime; }

	QHash<int,QString> getDirCapture() { QMutexLocker locker(&this->mutexDirCapture); return this->dirCapture; }
	QHash<int,QString> getDirLocal() { QMutexLocker locker(&this->mutexDirLocal); return this->dirLocal; }
	QHash<int,QString> getDirServer() { QMutexLocker locker(&this->mutexDirServer); return this->dirServer; }

	const QString& getPathCapture() const { return this->pathCapture; }
	const QString& getPathMp3() const { return this->pathMp3; }
	const QString& getPathServer() const { return this->pathServer; }

	QList<int> nrsCapture();

	QList<int> nrsLocal();

	QList<int> nrsServer();

	void rename( int titleNr, const QString& newFileName );

	void setGlobalSettings( lsGlobalSettings* newGlobalSettings );

	void sync( int titleNr, const QString& newFileName );

protected:
	QString fillDirHash( QHash<int,QString>& fileNrHash, QMutex& mutexDir, QString basePath );
	void renameInt( QMutex& mutexDir, QHash<int,QString>& dirHash, const QString& basePath, int titleNr, const QString& newFileName );
	void run();

private:
	QMutex mutexDirCapture;
	QHash<int,QString> dirCapture;

	QMutex mutexDirLocal;
	QHash<int,QString> dirLocal;

	QMutex mutexDirServer;
	QHash<int,QString> dirServer;

private:
	QMutex mutexGlobalData;
	QDate date;
	QString dateTime;
	QString subPath;

	QString pathCapture;
	QString pathMp3;
	QString pathServer;

	lsGlobalSettings* globalSettings;
};

#endif // LSFILELIST_H
