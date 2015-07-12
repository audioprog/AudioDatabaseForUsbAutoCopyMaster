#include "LsFileList.h"

#include <QDir>

#include "lsGlobalSettings.h"

LsFileList::LsFileList(QObject *parent)
	: QThread(parent)
	, globalSettings(NULL)
{
}

LsFileList::~LsFileList()
{
}

void LsFileList::changeCurrent(const QString& subPath, const QDate& newDate, const QString& newDateTime)
{
	this->mutexDirCapture.lock();
	this->dirCapture.clear();
	this->mutexDirCapture.unlock();

	this->mutexDirLocal.lock();
	this->dirLocal.clear();
	this->mutexDirLocal.unlock();

	this->mutexDirServer.lock();
	this->dirServer.clear();
	this->mutexDirServer.unlock();


	this->mutexGlobalData.lock();

	this->subPath = subPath;
	this->date = newDate;
	this->dateTime = newDateTime;

	if (this->subPath.isEmpty()) { this->mutexGlobalData.unlock(); return; }

	if (this->subPath.endsWith('/') == false)
	{
		this->subPath += "/";
	}

	this->mutexGlobalData.unlock();


	this->start();
}

void LsFileList::setGlobalSettings(lsGlobalSettings* newGlobalSettings)
{
	this->globalSettings = newGlobalSettings;
}

void LsFileList::sync(int titleNr, const QString& newFileName)
{

}

QList<int> LsFileList::nrsCapture()
{
	this->mutexDirCapture.lock();
	QList<int> toReturn = this->dirCapture.keys();
	this->mutexDirCapture.unlock();

	qSort(toReturn);

	return toReturn;
}

QList<int> LsFileList::nrsLocal()
{
	this->mutexDirLocal.lock();
	QList<int> toReturn = this->dirLocal.keys();
	this->mutexDirLocal.unlock();

	qSort(toReturn);

	return toReturn;
}

QList<int> LsFileList::nrsServer()
{
	this->mutexDirServer.lock();
	QList<int> toReturn = this->dirServer.keys();
	this->mutexDirServer.unlock();

	qSort(toReturn);

	return toReturn;
}

QString LsFileList::detPath(const QString& basePath)
{
	if (NULL == this->globalSettings) { return QStringLiteral(""); }

	QString path = basePath;

	if (path.endsWith('/') == false)
	{
		path += "/";
	}


	this->mutexGlobalData.lock();

	QString preSubPath = this->globalSettings->subPath(this->date);
	if (preSubPath.isEmpty() == false && ! preSubPath.endsWith('/'))
	{
		preSubPath += "/";
	}

	if (QDir(path + preSubPath).exists())
	{
		path += preSubPath;
	}

	if (QDir(path + this->subPath).exists())
	{
		path += this->subPath;
	}
	else
	{
		QString pathBase = this->subPath.section('/', 0, 0);
		QString pathSearch = path + pathBase;
		QString pathRest = this->subPath.section('/', 1, -1);
		while ( ! pathBase.isEmpty() )
		{
			QStringList subPaths = QDir(path).entryList(QStringList(pathSearch + "*"), QDir::AllDirs | QDir::NoDotAndDotDot);
			subPaths = subPaths.filter(pathBase);
			if ( ! subPaths.isEmpty())
			{
				path += subPaths.first() + "/";

				pathBase = pathRest.section('/', 0, 0);
				pathSearch = path + pathBase;
				pathRest = pathRest.section('/', 1, -1);
			}
			else
			{
				if (pathBase.contains(' '))
				{
					pathBase = pathBase.section(' ', 0, -2);
					pathSearch = pathSearch.section(' ', 0, -2);
				}
				else
				{
					path.clear();
					pathBase.clear();
				}
			}
		}
	}

	this->mutexGlobalData.unlock();

	return path;
}

void LsFileList::renameInt(QMutex& mutexDir, QHash<int,QString>& dirHash, const QString& basePath, int titleNr, const QString& newFileName)
{
	mutexDir.lock();

	if (dirHash.contains(titleNr) && dirHash.value(titleNr) != newFileName)
	{
		QString path = this->detPath(basePath);

		QStringList list = QDir(path).entryList(QStringList() << dirHash.value(titleNr), QDir::Files);

		if (list.count() == 1)
		{
			if (QFile(path + "/" + list.first()).rename(newFileName + "." + list.first().section('.', -1, -1)))
			{
				dirHash[titleNr] = list.first().section('.', 0, -2);
			}
		}
	}

	mutexDir.unlock();
}

void LsFileList::rename(int titleNr, const QString& newFileName)
{
	this->renameInt(this->mutexDirCapture, this->dirCapture,  this->globalSettings->capturePath(), titleNr, newFileName);
	this->renameInt(this->mutexDirLocal, this->dirLocal, this->globalSettings->mp3Path(), titleNr, newFileName);
	this->renameInt(this->mutexDirServer, this->dirServer, this->globalSettings->serverPath(), titleNr, newFileName);
}

QString LsFileList::fillDirHash(QHash<int,QString>& fileNrHash, QMutex& mutexDir, QString basePath)
{
	QString path = this->detPath(basePath);

	QStringList fileList = QDir(path).entryList(QStringList(QStringLiteral("*.mp3")) << QStringLiteral("*.wav"), QDir::Files);

	QRegExp regex(".*(\\d+)\\D.*");

	QHash<int,QString> fileHash;

	foreach (const QString& file, fileList)
	{
		if (regex.indexIn(file) > -1 && (file.startsWith(QStringLiteral("Tit")) || file.contains(QLatin1Char(' '))))
		{
			fileHash[regex.cap(1).toInt()] = file;
		}
	}

	mutexDir.lock();
	fileNrHash.swap(fileHash);
	mutexDir.unlock();

	return path;
}

void LsFileList::run()
{
	while ( ! this->isInterruptionRequested())
	{
		this->pathCapture = this->fillDirHash(this->dirCapture, this->mutexDirCapture, this->globalSettings->capturePath());
		this->pathMp3 = this->fillDirHash(this->dirLocal, this->mutexDirLocal, this->globalSettings->mp3Path());
		this->pathServer = this->fillDirHash(this->dirServer, this->mutexDirServer, this->globalSettings->serverPath());
		emit signalReadingFinished();
		sleep(5);
	}
}

